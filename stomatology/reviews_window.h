#ifndef REVIEWS_WINDOW_H
#define REVIEWS_WINDOW_H


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
class reviews_window;
}

class reviews_window : public QDialog,private database
{
    Q_OBJECT

public:
    explicit reviews_window(QSqlDatabase *db, QWidget *parent = nullptr, int id = 0);
    ~reviews_window();

private:
    Ui::reviews_window *ui;
    int id_doctor;
};

#endif // REVIEWS_WINDOW_H
