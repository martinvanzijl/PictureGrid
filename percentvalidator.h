#ifndef PERCENTVALIDATOR_H
#define PERCENTVALIDATOR_H

#include <QIntValidator>

class PercentValidator : public QIntValidator
{
public:
    PercentValidator(int min, int max, QObject *parent = nullptr);

    QValidator::State validate(QString &input, int &pos) const override;
};

#endif // PERCENTVALIDATOR_H
