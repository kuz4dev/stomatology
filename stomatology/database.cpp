#include "database.h"

database::database(QWidget *parent, QSqlDatabase *newDb)
    : parent(parent), db(newDb) {}
