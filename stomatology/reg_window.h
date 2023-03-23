#ifndef REG_WINDOW_H
#define REG_WINDOW_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QCryptographicHash>
#include "database.h"
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>>

namespace Ui {
class reg_window;
}

class reg_window : public QDialog,private database
{
    Q_OBJECT

public:
    explicit reg_window(QSqlDatabase *db, QWidget *parent = nullptr, bool st = false);
    QString hashPassword(QString str);
    ~reg_window();

private slots:
    void on_pushButton_reg_clicked();

private:
    Ui::reg_window *ui;
    bool status;//Для обработки режима регистрации
};

#endif // REG_WINDOW_H
