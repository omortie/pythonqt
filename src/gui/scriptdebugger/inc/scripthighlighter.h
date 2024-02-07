
#ifndef SCRIPT_HIGHLIGHTER_H
#define SCRIPT_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include "scriptdebugger.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class ScriptHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
	
    ScriptHighlighter(QTextDocument *parent = 0);
	void setLanguage(Language l);

private:
	void createRules();
	void createCRules();
	void createPythonRules();

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
	Language language;
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
	QTextCharFormat numberFormat;
	QTextCharFormat stringFormat;
};

#endif // SCRIPT_HIGHLIGHTER_H
