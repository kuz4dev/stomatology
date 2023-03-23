#ifndef TIMEEDIT_H
#define TIMEEDIT_H


#include <QtWidgets>

class TimeEdit: public QTimeEdit
{
public:
    using QTimeEdit::QTimeEdit;
    void stepBy(int steps) override{
        setTime(time().addMSecs(steps*1800000));
    }
};

#endif // TIMEEDIT_H
