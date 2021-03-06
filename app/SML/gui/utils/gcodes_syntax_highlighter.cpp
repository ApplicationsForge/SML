#include "gcodes_syntax_highlighter.h"

GCodesSyntaxHighlighter::GCodesSyntaxHighlighter(QObject *parent) : QSyntaxHighlighter(parent)
{

}



void GCodesSyntaxHighlighter::highlightBlock(const QString &text)
{
    setHighlightColor(text, gPattern, Qt::darkGreen);
    setHighlightColor(text, mPattern, Qt::darkYellow);
    setHighlightColor(text, argumentsPattern, Qt::darkMagenta);
    setHighlightColor(text, commentsPattern, Qt::lightGray);
}

void GCodesSyntaxHighlighter::setPattern()
{
    QString g = "G";
    QString m = "M";

    for(int i = 0; i < 10; i++)
    {
        QString tmp = QString::number(i);
        gPattern.push_back(g + tmp);
        mPattern.push_back(m + tmp);
    }

    for(int i = 10; i <= 99; i++)
    {
        QString tmp = QString::number(i);
        gPattern.push_back(g + tmp);
        mPattern.push_back(m + tmp);
    }
    argumentsPattern =
    {
        "X",
        "Y",
        "Z",
        "P",
        "O",
        "F",
        "S",
        "R",
        "D",
        "L",
        "I",
        "J",
        "K"
    };
    commentsPattern =
    {
        "[(]([^{}]*)[)]"
    };
}

void GCodesSyntaxHighlighter::setHighlightColor(const QString &text, QList<QString> pattern, const QBrush &brush)
{
    QTextCharFormat format;

    for(auto i : pattern)
    {
        QRegExp rx(i);
        if(!rx.isValid() || rx.isEmpty() || rx.exactMatch(""))
        {
            setFormat(0, text.length(), format);
            return;
        }
        format.setForeground(brush);
        int index = rx.indexIn(text);
        while(index >= 0)
        {
            int length = rx.matchedLength();
            setFormat(index, length, format);
            index = rx.indexIn(text, index + length);
        }
    }
}
