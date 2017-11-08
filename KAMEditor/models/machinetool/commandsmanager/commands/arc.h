#ifndef ARC_H
#define ARC_H

#include <cmath>

#include "command.h"

class CArc : public Command
{
public:
    CArc(QString R, QString Al, QString Fi, QString v = QString::number(1));

    byte_array getDataForMachineTool() const override;
    void draw(OGLWidget* w) const override;

    size_t getId() const override;
    std::string getName() const override;

    /**
     * @return строковое представление каждого из аргументов текущей команды
     */
    QStringList getArguments() const override;

    QColor getColor() const override;

    /**
     * @return подробное строковое представление аргументов текущей команды
     */
    QString getArgumentsString() const override;

private:
    size_t id = CMD_ARC;

    std::string name = "Дуга";

    // радиус дуги
    QString R;

    // начальный угол
    QString Al;

    // величина поворота
    QString Fi;

    // скорость
    QString v;

    QColor color = SmlColors::gray();

    bool isArgumentsCorrect() const;
};

#endif // ARC_H
