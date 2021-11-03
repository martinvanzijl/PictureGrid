#include "percentvalidator.h"

PercentValidator::PercentValidator(int min, int max, QObject *parent) :
    QIntValidator(min, max, parent)
{

}

QValidator::State PercentValidator::validate(QString &input, int &pos) const
{
    QString number = input;
    if (number.endsWith("%")) {
        number.chop(1);
        return QIntValidator::validate(number, pos);
    }

    return PercentValidator::Invalid;
}
