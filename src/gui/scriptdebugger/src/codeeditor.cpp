#include "codeeditor.h"

#define BPSize 30 //Breakpoint reserved horizontal space

/*! Editor constructor will create line number area , breakpoint area and also connects signals and slots
 * of the properties of the widget.
 * this is a customized QPlainTextEdit Widget implementation to looks like an IDE editor */
CodeEditor::CodeEditor(PythonQtObjectPtr context,QWidget *parent) : QPlainTextEdit(parent) ,_context(context)
{
    // creates the line number area to show the line number beside each block line (left side of editor)
    lineNumberArea = new LineNumberArea(this);
    // creates the breakpoint area to offer the user the ability to click on each line and select or deselect breakpoints
    ba = new BreakArea(this,this); // passes the parent of breakpoint area to the code editor plaintextedit
    setLineWrapMode(QPlainTextEdit::NoWrap); // will ensure that QPlainTextEdit will not wrap any line, so each block is exactly one single line
    setFont(QFont("Consolas", 12)); // Defalt font for coding in windows!

    _completer = new QCompleter(this);
    _completer->setWidget(this);
    QObject::connect(_completer, SIGNAL(activated(const QString&)),
                     this, SLOT(insertCompletion(const QString&)));

    // now connects all of the signals to their appropriate slots for highlighting and line number ability (Qt's documentation example)
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    //    updateLineNumberAreaWidth(0);
    // This line will instantly highlight the first line of the text without the user interaction
    highlightCurrentLine();
}

/*! This will calculate the horizontal size that line number area would take from the whole editor
 * the method is to calculate the appripriate horizontal size based on the number of digits of the line numbers
 * so first it will calculate the number of digits of the maximum line numbers and then based on the horizontal size
 * of number '9' it will calculate the appropriate horizontal size of the line number area. it will also preserve
 * a horizontal width for the breakpoint area that will nest in the line number area
 * as you can see the GUI elements shifted in the below picture:
 * \image html userInterface.PNG
 * the left bar has been created for break point area and line number area to nest close to each other in the place
 * \return calculated horizontal space of the line number area as an integer that is required
 * \note that this function will also preserve a constant (around BPSize pixels) for nesting breakpoint area in line number area
 * \sa updateLineNumberAreaWidth() updateLineNumberArea() */
int CodeEditor::lineNumberAreaWidth()
{
    // calculates the maximum number of digits of line numbers
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // calculates the horizontal space needed for line number area and preserves +BPSize for breakpoint area
    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits + BPSize;
    return space;
}

/*! This procedure will update the vertical size of line number area if line numbers change to different order
* also will set margins for editor to move all of the texts after calculated horizontal space and preserve it for line number area
* \sa lineNumberAreaWidth() updateLineNumberArea() */
void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/*! This slot will be activated when user scrolls the editor by mouse scroll or by keyboard commands
 * so it ensures the line numbers and also breakpoints (nested widgets) should scroll exactly the same
 * \todo Scroll when line number area or breakpoint area will send scroll command also
 * \param rect as a rectangle which the update event has been occured in there
 * \param dy as the number of scrolled pixels when the user scrolls the editor
 * \sa updateLineNumberAreaWidth() lineNumberAreaWidth() */
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy){
        lineNumberArea->scroll(0, dy);
        ba->scroll(0,dy);
    }
    else{
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
        ba->update(0,rect.y(),BPSize,rect.height());
    }

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

/*! This slot will be called when user interacts with the editor by adding or removing the breakpoint
 * or by editing the code, this line will again investigate on each code block in the editor and
 * analyzes it's BreakData (reimplemented QTextBlockUserData class) to see if that block is set to be
 * a breakpoint or not , then will create a list of breakpoint line numbers and will send the list
 * with a signal to the user.
 * this method will prevent wrong breakpoint line numbers and will send always correct breakpoint line numbers (updated) */
void CodeEditor::updateAllBP()
{
    QTextBlock block; // for accessing to each block address
    BreakData *bData; // for accessing to each block user data (breakpoint status)
    QList<int> bpList; // this list will be populated during the loop by true breakpoint lines

    // loop over the whole document and extract each block's user data (breakpoint status) to populate the breakpointList
    for (int i = 0 ; i < document()->lineCount() ; i++){
        // finds the address of the appropriate block by the line number parameter
        block = document()->findBlockByLineNumber(i);
        // extract current block's userData
        bData = ((BreakData*) (block.userData()));

        // populate the breakpointList by true breakpoint blocks otherwise will leave it alone and will go to next block
        if (bData == nullptr || !bData->breakpoint()) continue;
        else if (bData->breakpoint())
            bpList.append(block.firstLineNumber() + 1); // uses the real actual line number instead of an array-like element index by adding 1 unit
    }
    //!updateBP(QList<int>) signal will send the populated list (breakpoint line numbers) to the user by a signal
    emit updateBP(bpList);
}

/*! This slot will highlight the requested line number in the editor by a permanent different color line highlighter
 * \todo solve permanent yellow highlighting in the window until clearHighlights() called
 * \param lineNumber a specific line number to be highlighted requested by user (normally indicating the running line)
 * \note that it will also sets a parameter called runningLine to the input line number then an arrow indicator for active
 * running line will be visible beside the line rectangle in the breakpoint area. this will happen in BreakArea paintEvent()
 * \sa clearHighlights() BreakArea::paintEvent() */
void CodeEditor::highlightMarkedLine(int lineNumber)
{
    QTextCursor cursor = textCursor(); // takes a copy of the editor's text cursor
    QTextBlock block = document()->findBlockByLineNumber(lineNumber); // finds the correspondent block to recieved line number
    if (block.position() == -1) return; // if requested line number was not a valid number cancels the operation
    cursor.setPosition(block.position()); // sets the position of first character in the block to the position of copied text cursor
    QColor lineColor = QColor(Qt::yellow).lighter(150);
    userSelection.format.setBackground(lineColor); // sets the color of the highlighter
    userSelection.format.setProperty(QTextFormat::FullWidthSelection, true); // will highlight all the line to the end of the editor
    userSelection.cursor = cursor; // will set the user selection address to the copied cursor so it will append to the list of highlightings
    runningLine = lineNumber;
    highlightCurrentLine(); // commands the highlighter to highlight all of the selected lines (user demanded and current cursor) | different colors
}

/*! This slot will set the requested highlighted line (userSelecttion highlight) to a null text cursor (clears the user-requested highlight) */
void CodeEditor::clearHighlights()
{
    QTextCursor cursor;
    userSelection.cursor = cursor;
    runningLine = -1;
    highlightCurrentLine();
}

/*! this procedure will repaint the whole Editor widget and also will repaint line number area internally
 * by setting it's geometry
 * \todo optimize size hint of the editor */
void CodeEditor::resizeEvent(QResizeEvent *e)
{
    // resizes the line number area widget to the changed size of the editor
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    ba->setGeometry(QRect(cr.left(),cr.top(),BPSize,cr.height()));
    QPlainTextEdit::resizeEvent(e);
}

/*! this overridden procedure will implement the functionality to zoom-in and zoom-out by holding control key on keyboard
 * and by scrolling the mouse wheel */
void CodeEditor::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier){
        event->delta() > 0 ?  zoomIn(2): zoomOut(2);
    }
    else{
        event->delta() > 0 ? verticalScrollBar()->setValue(verticalScrollBar()->value() - 1) :
                             verticalScrollBar()->setValue(verticalScrollBar()->value() + 1);
    }
}

void CodeEditor::handleCompletion(bool showByCtrlSpace)
{
    static QStringList found;
    static int keywordlen;

    if(found.size() == 0){
        // add python keys to found for the first time
        found << "False"<< "True" << "None"<<"and"<<"as"<<"assert"<<"break"<<"class"<<"continue"<<"def"<<"del"<<"elif"<<"else"<<"except"<<"finally" <<
                 "for"<<"from"<<"global"<<"if"<<"import"<<"in"<<"is"<<"lambda"<<"nonlocal"<<"not"<<"or"<<"pass"<<"rais"<<"return"<<"try"<<"while"<<"with"<<
                 "yield" << "self";
        keywordlen =found.size();

    }else{
        // remove old value except python key value;
        for(int i=keywordlen;i<found.size();i++)
            found.removeAt(i);
        found=found.toSet().toList();

    }
    QTextCursor textCursor   = this->textCursor();
    int pos = textCursor.position();
    textCursor.setPosition(0, QTextCursor::MoveAnchor);
    textCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    int startPos = textCursor.selectionStart();
    int offset=0;
     QString text;
    if((startPos == pos) && pos < 5){
         offset=pos;
         text=toPlainText();
    }
    else{
       offset= pos-startPos;
       text= textCursor.selectedText();
    }

    QString textToComplete;
    int cur = offset;
    while (cur--) {
        QChar c = text.at(cur);
        if(c == " ")
            break;
        if (c.isLetterOrNumber() || c == '.' || c == '_') {
            textToComplete.prepend(c);
        } else {
            break;
        }
    }
    QString lookup;
    QString compareText = textToComplete;
    int dot = compareText.lastIndexOf('.');
    if (dot!=-1) {
        lookup = compareText.mid(0, dot);
        compareText = compareText.mid(dot+1, offset);
    }
    if (!lookup.isEmpty() || !compareText.isEmpty()) {
        compareText = compareText.toLower();

        QStringList l = PythonQt::self()->introspection(_context, lookup, PythonQt::Anything);
        Q_FOREACH (QString n, l) {
            if (n.toLower().startsWith(compareText)) {
                found << n;
            }
        }
        found=found.toSet().toList();
        _completer->setCompletionPrefix(compareText);
        _completer->setCompletionMode(QCompleter::PopupCompletion);
        _completer->setModel(new QStringListModel(found, _completer));
        _completer->setCaseSensitivity(Qt::CaseInsensitive);
        QTextCursor c = this->textCursor();
        c.movePosition(QTextCursor::StartOfWord);
        QRect cr = cursorRect(c);
        cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                    + _completer->popup()->verticalScrollBar()->sizeHint().width());
        cr.translate(0,8);
        _completer->complete(cr);

    }else {
        if(showByCtrlSpace){
            _completer->setCompletionPrefix("");
            _completer->setCompletionMode(QCompleter::PopupCompletion);
            _completer->setModel(new QStringListModel(found, _completer));
            _completer->setCaseSensitivity(Qt::CaseInsensitive);
            QTextCursor c = this->textCursor();
            c.movePosition(QTextCursor::StartOfWord);
            QRect cr = cursorRect(c);
            cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                        + _completer->popup()->verticalScrollBar()->sizeHint().width());
            cr.translate(0,8);
            _completer->complete(cr);

        }else
            _completer->popup()->hide();
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (_completer && _completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (event->key()) {
        case Qt::Key_Return:
            if (!_completer->popup()->currentIndex().isValid()) {
                insertCompletion(_completer->currentCompletion());
                _completer->popup()->hide();
                event->accept();
            }
            event->ignore();
            return;
            break;
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:

            event->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }
    QPlainTextEdit::keyPressEvent(event);
    if(((event->key() == Qt::Key_Space)  && (event->modifiers() & Qt::ControlModifier))){
        handleCompletion(true);
    }else {
        QString text = event->text();
        if (!text.isEmpty()) {
            handleCompletion();
        } else {
            _completer->popup()->hide();
        }
    }
}

void CodeEditor::insertCompletion(const QString &completion)
{
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
    if (tc.selectedText()==".") {
        tc.insertText(QString(".") + completion);
    } else {
        tc = textCursor();
        tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        tc.insertText(completion);
        setTextCursor(tc);
    }
}

/*! This procedure will highlight the currently selected line on the current position of the cursor
 * also will highlight user-requested line to a different line color if there were any request in place
 * \todo highlight current line when user clicks on the correspondent line number and also highlight that line number itself */
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections; // this line will ensures that old highlights will be cleared

    if (!isReadOnly()) {
        // will create a selection based on the current text cursor's position
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::blue).lighter(190);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();

        // appends current text cursor and user-requested selections to the final highlightin selection
        extraSelections.append(selection);
        extraSelections.append(userSelection);
    }
    setExtraSelections(extraSelections); // applies the created list of highlighting selections to the editor
}

/*! This function will recieve a vertical coordinate in the breakpoint area and will calculate it's correspondent block
 * the correspondent block will be used to set or take a breakpoint on the selected line by the user.
 * the method is to loop over the whole text and if the vertical coordinate selected by the user passed the vertical coordinate of a
 * line , that line will be selected as the selected line by the user
 * \param Y vertical coordinate as an integer
 * \return number correspondent to the recieved coordinate */
QTextBlock CodeEditor::findBlockByVerticalPos(int Y)
{
    QTextBlock block; // for storing block's variable information
    // loops over the whole text for find the correspondent line to the recieved vertical coordinate
    for (int i = 0 ; i < document()->lineCount() ; i++)
    {
        block = document()->findBlockByLineNumber(i);
        // this line will check if the recieved vertical coordinate passes the current block's vertical coordinate
        if (blockBoundingGeometry(block).translated(contentOffset()).top() + blockBoundingRect(block).translated(contentOffset()).height() > Y)
            break;
    }
    // if user clicks on a misleading coordinate in the breakpoint area the last line will be selected and returned
    return block;
}

/*! This function will return the block's correspondent rectangle for geometry uses based on block's number
 * \param blockNumber block's number as an integer
 * \return correspondent rectangle calculated based on the block's number */
QRect CodeEditor::getRect(int blockNumber)
{
    return blockBoundingGeometry(document()->findBlockByNumber(blockNumber)).translated(contentOffset()).toRect();
}

/*! Breakpoint's area class constructor will just set the parent editor and also will define the default cursor's shape
 * when user will move the cursor to the breakpoint area as a pointing hand cursor */
BreakArea::BreakArea(CodeEditor *_editor, QWidget *parent)
    : QWidget(parent)
    , editor(_editor)
{
    setCursor(Qt::PointingHandCursor);
}

/*! Will calculate the sizes of each number in the line number area when a paint event called
 * this will automatically regenerate the numbers area and the vertical size of numbers
 * also generates the number of each line and writes the number in the line number area by a simple painter */
void LineNumberArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); // creates the painter used to write the line numbers
    painter.fillRect(event->rect(), Qt::lightGray); // creates the background of line number area as a rectangle (also nested breakpoint)
    painter.setFont(codeEditor->font());

    // creates neeeded variables to store block's information
    QTextBlock block = codeEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    // for storing coordinate of the block's rectangle to write the line number (top and bottom)
    int top = (int) codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top();
    int bottom = top + (int) codeEditor->blockBoundingRect(block).height();

    // loops over the whole text in the editor and will write their appropriate line numbers beside them in the line number area
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, codeEditor->lineNumberArea->width(), codeEditor->fontMetrics().height(),
                             Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom; // the top coordinate of the current block is the bottom coordinate of the previous block
        bottom = top + (int) codeEditor->blockBoundingRect(block).height(); // calculates the bottom coordinate of the current block by a simple trick
        ++blockNumber;
    }

    QWidget::paintEvent(event);
}

/*! This procedure is a reimplementation of paintEvent procedure of BreakArera widget and it will analyze and will paint a red circle
 * as a breakpoint mark on the breakpoint area so the user knows where are the breakpoints has been set and can take them or set more breakpoints.
 * the method is to use the reimplemented class BreakData (reimplemented from QTextBlockUserData) to save breakpoint status on each block in it's
 * own address to be accessed whenever it needs to.
 * \note that there is paintEvent signal in editor raised (when the cursor is blinking) and so the signal will propagate in all it's child
 * widgets like line number area and break point area like each second (as it seems) so painting breakpoints on breakpoint area will
 * be synchronized with blinking cursor's signal or other source of raising paintEvent signal. but because Painter can only be used in a
 * painEvent procedure or any function called by paintEvent procedure (as said in Qt's documentation) so I thought this would be the most
 * efficient way
 * \todo efficiency in too many line numbers by using firstVisibleLine
 * \todo write a more efficient and neat procedure */
void BreakArea::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    // defining variables for storing block's informations
    QRect rect; // correspondent rectangle to the current block
    QTextBlock block; // the block itself
    BreakData *bData; // the BreakData to store breakpoint status data saved in the block
    QPainter painter(this); // our tiny-beauty painter
    // looping over the whole text to see which blocks are set as breakpoints to paint a nice simple circle beside them in breakpoint area
    QPixmap breakpoint(":icons/scriptdebugger/breakpoint.png");
    QPixmap activeLine(":icons/scriptdebugger/forwd_32.png");
    for (int i = 0 ; i < editor->document()->lineCount() ; i++){
        // finding block's information
        block = editor->document()->findBlockByLineNumber(i);
        bData = ((BreakData*) (block.userData()));
        if (bData != nullptr && bData->breakpoint())
        {
            rect = editor->getRect(block.blockNumber());
            rect.setWidth(BPSize); // BPSize-pixels are enough for a simple breakpoint area
            QRect circleRect(rect.x() + rect.width() / 2 - 10, rect.y() + rect.height() / 2 - 5, 10 , 10);
            painter.drawPixmap(circleRect,breakpoint);
        }
        if (block.firstLineNumber() == editor->runningLine)
        {
            rect = editor->getRect(block.blockNumber());
            rect.setWidth(BPSize);
            QRect circleRect(rect.x() + rect.width() / 2 + 5, rect.y() + rect.height() / 2 - 5, 10 , 10);
            painter.drawPixmap(circleRect,activeLine);
        }
    }
}


/*! This slot will be called when user clicks on any parts of the breakpoint area to the upper level so the editor will recalculate
 * the breakpoint list and will send it to the user */
void BreakArea::BPUpdate()
{
    update();
    editor->updateAllBP();
}

/*! Breakpoints are usually set when user clicks on the breakpoint area or will be taken out in exactly the same procedure
 * in this reimplemented procedure (again from QWidget class) we interrupted the mouse clicking event and will calculate the
 * correpondent line number and will set a breakpoint status for that block line.
 * if that line had a breakpoint status set to true before , we make the status false to take the breakpoint from that line
 * \todo disable right click action on breakpoint area (just leftclick) */
void BreakArea::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);

    // find the correspondent block information from the click's position from mouseEvent
    QTextBlock block = editor->findBlockByVerticalPos(e->pos().y());
    BreakData *bdata = ((BreakData*)(block.userData()));

    // if there is no BreakData created as UserData in the block so we will create a new one and apply it to the block
    if (bdata == nullptr) {
        bdata = new BreakData(this);
        block.setUserData(bdata);
    }

    // if the breakpoint's status has been set to false so by user's click we set it to true and vise-versa
    bdata->setBreakpoint(!bdata->breakpoint());
    // call BreakPoint Update to inform upper-calsses that breakpoints changed
    BPUpdate();
}
