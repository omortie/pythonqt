/*---------------------Legitimacy---------------------
 * Author: Mortie (sherafati.morteza@gmail.com)
 * Company: Pars Process Company
 * Date: 19 August 2018
 * Description: Header file for Code Editor sub-components of
 * Script Editor component (Breakpoint System , Line Number Area and Highlighting)
 * ---------------------------------------------------*/

#ifndef CODEEDITOR_H
#define CODEEDITOR_H
#include "PythonQt.h"
#include <QtWidgets>
#include <QPlainTextEdit>

class LineNumberArea;
class BreakData;
class BreakArea;
class QCompleter;

/* This class will modify QPlainTextEdit class and prepares it as a code-editor-like
 * with line numbers beside the text and also an interactive highlighter that will
 * highlight wherever user clicks or moves the cursor. it will be suitable for a basic
 * IDE , it also supports breakpoint system and visual interactions */
class PYTHONQT_EXPORT CodeEditor : public QPlainTextEdit
{
    friend class LineNumberArea;
    friend class BreakArea;

Q_OBJECT

public:
    explicit CodeEditor(PythonQtObjectPtr context,QWidget *parent = 0);

    int lineNumberAreaWidth();
    QTextBlock findBlockByVerticalPos(int Y);
    QRect getRect(int blockNumber);
    void updateAllBP();
    void highlightMarkedLine(int lineNumber);
    void clearHighlights();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;
    void handleCompletion(bool showByCtrlSpace = false);

public slots:
    void keyPressEvent (QKeyEvent * event) override;
    void insertCompletion(const QString&);
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

signals:
    void updateBP(QList<int>);

private:
    LineNumberArea *lineNumberArea;
    BreakArea *ba;
    QTextEdit::ExtraSelection userSelection;
    int runningLine = -1;
    QCompleter* _completer;
    PythonQtObjectPtr _context;


};

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor)
        , codeEditor(editor)
    {}

    QSize sizeHint() const override {
        return QSize(30 + codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *codeEditor;
};

class BreakArea : public QWidget
{
    friend class CodeEditor;
public:
    explicit BreakArea(CodeEditor *_editor, QWidget *parent = 0);
    void BPUpdate();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    QSize sizeHint() const override {
            return QSize(30, 0);
        }

private:
    CodeEditor *editor;
};

class BreakData : public QTextBlockUserData
{
public:
    explicit BreakData(BreakArea *ba)
        : ba(ba)
    {}
    ~BreakData() {ba->BPUpdate();}
    bool breakpoint() {return isBreakpoint;}
    void setBreakpoint(bool ibreak) {isBreakpoint = ibreak;}


private:
    bool isBreakpoint = false;
    BreakArea *ba;
    QTextBlock *block;
};

#endif // CODEEDITOR_H
