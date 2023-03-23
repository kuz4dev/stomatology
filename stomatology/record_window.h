#ifndef RECORD_WINDOW_H
#define RECORD_WINDOW_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "database.h"
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class record_window;
}

class record_window : public QDialog,private database
{
    Q_OBJECT

public:
    explicit record_window(QSqlDatabase *db, QWidget *parent = nullptr, int id_tt = 0, int id_d = 0, int user = 0);
    ~record_window();

private slots:
    void on_pushButton_record_clicked();

    void on_pushButton_plus_clicked();

    void on_pushButton_minus_clicked();

private:
    Ui::record_window *ui;
    int id_timetable;
    int id_doctor;
    int id_user;
};

#endif // RECORD_WINDOW_H
