#ifndef LINE_H
#define LINE_H

#include "smlcommand.h"

class SMLCommand;

class Line : public SMLCommand
{
public:
    Line(QString dx, QString dy, QString dz, QString v = QString::number(1));

    byte_array getDataForMachineTool() const override;
    void draw(OGLWidget* w) const override;

    size_t getId() const override;
    QString getName() const override;

    /**
     * @return строковое представление каждого из аргументов текущей команды
     */
    QStringList getArguments() const override;

    void setArguments(const QStringList arguments) override;

    /**
     * @return подробное строковое представление аргументов текущей команды
     */
    QString getArgumentsString() const override;

    QColor getColor() const override;

    bool isArgumentsCorrect() const override;
private:
    size_t id = CMD_LINE;
    const QString name = "Линия";
    QString dx, dy, dz;
    QString v;
    QColor color = SmlColors::gray();
};

#endif // LINE_H
