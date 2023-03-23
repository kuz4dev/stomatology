#ifndef DATABASE_H
#define DATABASE_H
#include <QWidget>
#include <QtSql/QSqlDatabase> //БД
#include <QtSql/QSqlError>    //Ошибки
#include <QtSql/QSqlQuery>    //Запросы


class database
{
public:
    database(QWidget *parent, QSqlDatabase *newDb = nullptr);

protected:
    QWidget *parent;
    QSqlDatabase *db;//Бд для работы с приложением
};

#endif // DATABASE_H
