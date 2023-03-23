#ifndef DIALOGFORREVIEW_H
#define DIALOGFORREVIEW_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "database.h"
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QDebug>

namespace Ui {
class DialogForReview;
}

class DialogForReview : public QDialog,private database
{
    Q_OBJECT

public:
    explicit DialogForReview(QSqlDatabase *db,QWidget *parent = nullptr,int doctor=0,int user=0,bool fl = false);
    ~DialogForReview();

private slots:
    void on_pushButton_add_clicked();

    void on_pushButton_close_clicked();

private:
    Ui::DialogForReview *ui;

    int id_user;
    int id_doctor;
    bool status;
};

#endif // DIALOGFORREVIEW_H
