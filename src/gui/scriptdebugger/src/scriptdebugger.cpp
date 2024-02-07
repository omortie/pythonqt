
#include <QtWidgets>
#include <QSplitter>

#include "scriptdebugger.h"
#include "scripthighlighter.h"
#include "codeeditor.h"

#include "gui/PythonQtScriptingConsole.h"

ScriptDebugger::ScriptDebugger(PythonQtObjectPtr *pythonengine,QWidget *parent) :QWidget(parent), mainpyengine(pythonengine), textEdit(new CodeEditor(*pythonengine,this))
{
    initEditor();
    initConsole();
    initDebugger();
    initTableVariable();
    connect(debuggerThread,&DebugThread::debugInfo,this,&ScriptDebugger::showVariable);

    QVBoxLayout *vlayout = new QVBoxLayout(this);

    Vsplitter=new QSplitter();
    Vsplitter->setOrientation(Qt::Vertical);
    Vsplitter->setStyleSheet("QSplitter::handle{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc);  border: 1px solid #666; width: 13px; margin-top: 2px; margin-bottom: 2px; border-radius: 5px;  }"
                             "QSplitter::handle:horizontal { width: 2px;}"
                             "QSplitter::handle:vertical {height: 2px;}");

    Vsplitter->addWidget(editor);
    Vsplitter->addWidget(console);
    Vsplitter->setSizes({600,200});

    Hsplitter=new QSplitter();
    Hsplitter->setOrientation(Qt::Horizontal);
    Hsplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    Hsplitter->addWidget(Vsplitter);
    Hsplitter->addWidget(tableVariable);
    Hsplitter->setSizes({600,200});

    vlayout->addWidget(Hsplitter);
    setLayout(vlayout);
}

void ScriptDebugger::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void ScriptDebugger::newFile()
//! [5] //! [6]
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void ScriptDebugger::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, "", "", "Python Files(*.py)");
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool ScriptDebugger::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool ScriptDebugger::saveAs()
{
    QFileDialog dialog(this,"","","Python Files(*.py)");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void ScriptDebugger::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void ScriptDebugger::run()
{
    mainpyengine->evalScript(text());
}

void ScriptDebugger::debug()
{
    debuggerThread->setScript(textEdit->toPlainText());

    if(!debuggerThread->isRunning()){
        debuggerThread->start();
    }
}

void ScriptDebugger::_stepOver()
{
    textEdit->clearHighlights();
    debuggerThread->stepOver();
}

void ScriptDebugger::_continue()
{
    textEdit->clearHighlights();
    debuggerThread->continueDebug();
}

void ScriptDebugger::_stepInto()
{
    textEdit->clearHighlights();
    debuggerThread->stepInto();
}

void ScriptDebugger::_profileCode()
{
    if(!text().isEmpty())
        mainpyengine->call("dbg_execTime",QVariantList()<< text());
}

void ScriptDebugger::_stop()
{
    textEdit->clearHighlights();
    debuggerThread->stopDebug();
}

void ScriptDebugger::clearConsole()
{
    console->clear();
}

void ScriptDebugger::minifyFont()
{
    textEdit->zoomOut(2);
}

void ScriptDebugger::maxifyFont()
{
    textEdit->zoomIn(2);
}

void ScriptDebugger::showVariable(QString info)
{
    info.remove(0,1);
    info.remove(info.length()-1,1);
    info.replace(QRegExp("'"), "");
    QStringList allVar=info.split(',');
    tableVariable->clearContents();
    tableVariable->setRowCount(0);
    tableVariable->setRowCount(allVar.count());

    int row=0;
    foreach (QString var, allVar) {
        QStringList itemvalue = var.split(':');
        if(itemvalue.count()>1){
            tableVariable->setItem(row,1,new QTableWidgetItem(itemvalue.at(0)));
            tableVariable->setItem(row,0,new QTableWidgetItem(itemvalue.at(1)));
            tableVariable->item(row,1)->setFlags(tableVariable->item(row,1)->flags() & ~Qt::ItemIsEditable);
            row++;
        }
    }
}

void ScriptDebugger::createToolbar()
{	
    toolbar = new QToolBar(tr("toolbar"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/icons/scriptdebugger/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &ScriptDebugger::newFile);
    toolbar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/icons/scriptdebugger/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &ScriptDebugger::open);
    toolbar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/icons/scriptdebugger/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &ScriptDebugger::save);
    toolbar->addAction(saveAct);

#ifndef QT_NO_CLIPBOARD
    toolbar->addSeparator();

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/icons/scriptdebugger/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    toolbar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/icons/scriptdebugger/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    toolbar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/icons/scriptdebugger/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    toolbar->addAction(pasteAct);

    toolbar->addSeparator();

    const QIcon maxifyFont = QIcon::fromTheme("font-minify", QIcon(":/icons/scriptdebugger/zoomi_32.png"));
    QAction *maxifyFontAct = new QAction(maxifyFont, tr("Zoom &In"), this);
    maxifyFontAct->setShortcuts(QKeySequence::ZoomIn);
    maxifyFontAct->setStatusTip(tr("Zoom in the editor"));
    connect(maxifyFontAct,&QAction::triggered,this,&ScriptDebugger::maxifyFont);
    toolbar->addAction(maxifyFontAct);

    const QIcon minifyIcon = QIcon::fromTheme("font-minify", QIcon(":/icons/scriptdebugger/zoomo_32.png"));
    QAction *minifyFontAct = new QAction(minifyIcon, tr("Zoom &Out"), this);
    minifyFontAct->setShortcuts(QKeySequence::ZoomOut);
    minifyFontAct->setStatusTip(tr("Zoom out the editor"));
    connect(minifyFontAct,&QAction::triggered,this,&ScriptDebugger::minifyFont);
    toolbar->addAction(minifyFontAct);

    toolbar->addSeparator();

    const QIcon runIcon = QIcon::fromTheme("run-script", QIcon(":/icons/scriptdebugger/run.png"));
    QAction *runAct = new QAction(runIcon, tr("&Run"), this);
    runAct->setStatusTip(tr("Run Script"));
    connect(runAct, &QAction::triggered, this, &ScriptDebugger::run);
    toolbar->addAction(runAct);

    toolbar->addSeparator();

    const QIcon debugIcon = QIcon::fromTheme("debug-script", QIcon(":/icons/scriptdebugger/debug.png"));
    QAction *debugAct = new QAction(debugIcon, tr("&Debug"), this);
    debugAct->setStatusTip(tr("Debug Script"));
    connect(debugAct, &QAction::triggered, this, &ScriptDebugger::debug);
    toolbar->addAction(debugAct);

    const QIcon continueIcon = QIcon::fromTheme("continue-script", QIcon(":/icons/scriptdebugger/debugger_continue.png"));
    QAction *continueAct = new QAction(continueIcon, tr("Continue"), this);
    continueAct->setStatusTip(tr("Contiue"));
    connect(continueAct, &QAction::triggered, this, &ScriptDebugger::_continue);
    toolbar->addAction(continueAct);

    const QIcon stopIcon = QIcon::fromTheme("stop-script", QIcon(":/icons/scriptdebugger/debugger_stop.png"));
    QAction *stopAct = new QAction(stopIcon, tr("&Stop"), this);
    stopAct->setStatusTip(tr("Stop"));
    connect(stopAct, &QAction::triggered, this, &ScriptDebugger::_stop);
    toolbar->addAction(stopAct);

    const QIcon stepOverIcon = QIcon::fromTheme("stepOver-script", QIcon(":/icons/scriptdebugger/stepover.png"));
    QAction *stepOverAct = new QAction(stepOverIcon, tr("&Step Over"), this);
    stepOverAct->setStatusTip(tr("StepOver"));
    connect(stepOverAct, &QAction::triggered, this, &ScriptDebugger::_stepOver);
    toolbar->addAction(stepOverAct);

    const QIcon stepIntoIcon = QIcon::fromTheme("stepInto-script", QIcon(":/icons/scriptdebugger/stepinto.png"));
    QAction *stepIntoAct = new QAction(stepIntoIcon, tr("&Step In"), this);
    stepIntoAct->setStatusTip(tr("StepIn"));
    connect(stepIntoAct, &QAction::triggered, this, &ScriptDebugger::_stepInto);
    toolbar->addAction(stepIntoAct);

    toolbar->addSeparator();

    const QIcon profilerIcon = QIcon::fromTheme("profile-code", QIcon(":icons/scriptdebugger/profiler.png"));
    QAction *profilerAct = new QAction(profilerIcon, tr("&Profile Code"), this);
    profilerAct->setStatusTip(tr("Profiler Code"));
    connect(profilerAct, &QAction::triggered, this, &ScriptDebugger::_profileCode);
    toolbar->addAction(profilerAct);

    toolbar->addSeparator();

    const QIcon clearIcon = QIcon::fromTheme("clear-console", QIcon(":/icons/scriptdebugger/clear.png"));
    QAction *clearAct = new QAction(clearIcon, tr("&Clear Console"), this);
    clearAct->setStatusTip(tr("Clear Console"));
    connect(clearAct, &QAction::triggered, this, &ScriptDebugger::clearConsole);
    toolbar->addAction(clearAct);


#endif 

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD

}

void ScriptDebugger::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void ScriptDebugger::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool ScriptDebugger::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void ScriptDebugger::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll().toUtf8());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
}

void ScriptDebugger::setLanguage(Language l)
{
    highlighter->setLanguage(l);
}

void ScriptDebugger::setText(QString script)
{
    textEdit->setPlainText(script);
}

QString ScriptDebugger::text()
{
    return textEdit->toPlainText();
}

// This slot will call another slot inside the editor to highlight the requested line number by user
void ScriptDebugger::markLine(int lineNum)
{
    textEdit->highlightMarkedLine(lineNum);
}

// This slot will call another slot inside the editor to clear the highlighted lines
void ScriptDebugger::clearMark()
{
    textEdit->clearHighlights();
}

bool ScriptDebugger::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    return true;
}

void ScriptDebugger::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.py";
    setWindowFilePath(shownName);

    changedCurrentFile(curFile);
}

QString ScriptDebugger::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void ScriptDebugger::initConsole()
{
    console = new PythonQtScriptingConsole(NULL,*mainpyengine);
    console->resize(size().width()-10,40);
}

void ScriptDebugger::initEditor()
{
    editor=new QWidget(this);

    createToolbar();
    QVBoxLayout *vlayout = new QVBoxLayout(editor);
    vlayout->addWidget(toolbar);
    vlayout->addWidget(textEdit);
    vlayout->setSpacing(0);
    vlayout->setMargin(0);

    editor->setLayout(vlayout);
    readSettings();
    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &ScriptDebugger::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &ScriptDebugger::commitData);
#endif

    setCurrentFile(QString());
    //setUnifiedTitleAndToolBarOnMac(true);

    highlighter = new ScriptHighlighter(textEdit->document());
}

void ScriptDebugger::initDebugger()
{
    debuggerThread=new DebugThread(mainpyengine);
    connect(debuggerThread,&DebugThread::currentLine,textEdit,&CodeEditor::highlightMarkedLine);
    connect(textEdit,&CodeEditor::updateBP,debuggerThread,&DebugThread::setBrekPoints);
}

void ScriptDebugger::initTableVariable()
{
    tableVariable=new QTableWidget(this);
    tableVariable->setColumnCount(2);
    tableVariable->setHorizontalHeaderItem(0, new QTableWidgetItem());
    tableVariable->setHorizontalHeaderItem(1,new QTableWidgetItem());
    tableVariable->horizontalHeaderItem(0)->setText("Value");
    tableVariable->horizontalHeaderItem(1)->setText("Name");
    tableVariable->resizeColumnsToContents();
    tableVariable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

#ifndef QT_NO_SESSIONMANAGER
void ScriptDebugger::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (textEdit->document()->isModified())
            save();
    }
}
#endif

void ScriptDebugger::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    QPen pen(Qt::gray);
    painter.setPen(pen);
    painter.drawRect(rect().adjusted(0, 0, -3, -3));
}

