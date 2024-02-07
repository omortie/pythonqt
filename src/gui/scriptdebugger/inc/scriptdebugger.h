
#ifndef SCRIPT_DEBUGGER_H
#define SCRIPT_DEBUGGER_H

#include <QMainWindow>
#include "PythonQt.h"
#include <QThread>
#include <QDebug>
#include <QList>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class PythonQtScriptingConsole;
class PythonQtObjectPtr;
class QSplitter;
class CodeEditor;
class QTableWidget;

QT_END_NAMESPACE

class ScriptHighlighter;

enum Language
{
    C,
    Python
};

class DebugThread : public QThread
{
    Q_OBJECT
public:
    enum Command{
        None,
        InitScript,
        StepOver,
        StepInto,
        Continue,
        Stop
    };

    DebugThread(PythonQtObjectPtr *pyengine,QObject *parent = Q_NULLPTR):QThread(parent),
        cmd(Command::None),
        script(QString("")),
        isrun(false){
        engine=pyengine;
        engine->evalFile(":scripts/scriptdebugger/debugger.py");
    }
    void setScript(QString _script){
        script=_script;
        cmd=Command::InitScript;
    }

    void run() override {

        if(script.isEmpty())
            return;

        isrun=true;
        int line=0;
        QString dbgInfo;

        while(isrun){
            line =0;
            dbgInfo ="";

            switch (cmd) {
            case Command::InitScript:
                engine->call("debugger",QVariantList()<<script);
                cmd=Command::None;
                break;

            case Command::StepOver:
                engine->call("dbg_stepOver");

                line=engine->call("dbg_currentline").toInt();
                dbgInfo=engine->call("dbg_debugInfo").toString();

                if(line)
                    emit currentLine(line - 1);

                if(!dbgInfo.isEmpty())
                    emit debugInfo(dbgInfo);

                cmd=Command::None;
                break;

            case Command::StepInto:
                engine->call("dbg_stepInto");
                line=engine->call("dbg_currentline").toInt();
                dbgInfo=engine->call("dbg_debugInfo").toString();

                if(line)
                    emit currentLine(line - 1);

                if(!dbgInfo.isEmpty())
                    emit debugInfo(dbgInfo);

                cmd=Command::None;
                break;

            case Command::Stop:
                engine->call("dbg_stop");
                cmd=Command::None;
                break;

            case Command::Continue:
                engine->call("dbg_continue");
                line=engine->call("dbg_currentline").toInt();
                dbgInfo=engine->call("dbg_debugInfo").toString();

                if(line)
                    emit currentLine(line - 1);

                if(!dbgInfo.isEmpty())
                    emit debugInfo(dbgInfo);

                cmd=Command::None;
                break;

            default:
                break;
            }
            msleep(200);
        }
    }

    void stop(){
        isrun=false;
    }

    void stepOver(){
        cmd = Command::StepOver;
    }

    void stepInto(){
        cmd = Command::StepInto;
    }

    void continueDebug(){
        cmd = Command::Continue;
    }

    void stopDebug(){
        cmd = Command::Stop;
    }
signals:
    void currentLine(int);
    void debugInfo(QString);

public slots:

    void setBrekPoints(QList<int> breakpoints){
        engine->call("dbg_clear_all_breaks");
        for(int i=0;i<breakpoints.count();i++){
            engine->call("dbg_set_break", QVariantList() << breakpoints.at(i));
        }
    }

private:
    PythonQtObjectPtr *engine;
    Command cmd;
    QString script;
    bool isrun;
};

class PYTHONQT_EXPORT ScriptDebugger : public QWidget
{
    Q_OBJECT

public:
    ScriptDebugger(PythonQtObjectPtr *pythonengine,QWidget *parent=0);

    void loadFile(const QString &fileName);
    QString getCurrentFile() {
        return curFile;
    }
    void setLanguage(Language l);
	void setText(QString script);
    QString text();
    void markLine(int lineNum);
    void clearMark();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:
    void changedCurrentFile(QString file);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();
    void run();
    void debug();
    void clearConsole();
    void minifyFont();
    void maxifyFont();
    void showVariable(QString);

    void _continue();
    void _stop();
    void _stepOver();
    void _stepInto();
    void _profileCode();


#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createToolbar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void initConsole();
    void initEditor();
    void initDebugger();
    void initTableVariable();


    PythonQtObjectPtr *mainpyengine;
    CodeEditor *textEdit;
    QString curFile;
    QToolBar *toolbar;
    ScriptHighlighter *highlighter;
    PythonQtScriptingConsole *console;
    PythonQtObjectPtr rpdb;
    QTableWidget* tableVariable;

    QSplitter *Vsplitter;
    QSplitter *Hsplitter;
    QWidget *editor;
    DebugThread* debuggerThread;
};
#endif
