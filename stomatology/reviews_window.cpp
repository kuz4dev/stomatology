#include "reviews_window.h"
#include "ui_reviews_window.h"

reviews_window::reviews_window(QSqlDatabase *db, QWidget *parent, int id) :
    QDialog(parent),
    database (parent,db),
    ui(new Ui::reviews_window)//конструктор
{
    ui->setupUi(this);
    id_doctor = id;
    this->setWindowTitle("Отзыв");
    QSqlQuery qw(*db);
    qw.prepare("SELECT * FROM data.doctor WHERE id_doctor = :1");
    qw.bindValue(":1",id_doctor);
    qw.exec();
    qw.next();

    QString name = qw.value(3).toString() + " " + qw.value(4).toString();
    int id_specialization = qw.value(7).toInt();
    QStringList str = qw.value(5).toString().split("-");

    qw.prepare("SELECT * FROM data.specialization WHERE id_specialization = :1");
    qw.bindValue(":1",id_specialization);
    qw.exec();
    qw.next();

    QString spec = qw.value(1).toString();

    qw.prepare("SELECT AVG(rating),count(rating) FROM data.reviews WHERE id_doctor = :1");
    qw.bindValue(":1",id_doctor);
    qw.exec();
    qw.next();

    double rating = qw.value(0).toDouble();
    this->setWindowTitle(name);
    ui->label_name->setText("Врач - "+name);
    ui->lineEdit_rating->setText(QString::number(rating)+"/10");
    ui->lineEdit_specialization->setText(spec);

    qw.prepare("SELECT * FROM data.reviews WHERE id_doctor = :1");
    qw.bindValue(":1",id_doctor);
    qw.exec();

    //Вычисление возраста
    int age=QDate::currentDate().year()-str[0].toInt();
    if(QDate(0,str[1].toInt(),str[2].toInt())>QDate(0,QDate::currentDate().month (),QDate::currentDate().day()))
        --age;

    ui->lineEdit_age->setText(QString::number(age));

    while (qw.next())
    {
        int rating = qw.value(3).toInt();
        QString desc = qw.value(4).toString();
        QString date = qw.value(5).toString();

        QSqlQuery qw1(*db);
        qw1.prepare("SELECT first_name,second_name FROM data.user WHERE id_user = :1");
        qw1.bindValue(":1",qw.value(1).toInt());
        qw1.exec();
        qw1.next();

        QString name = qw1.value(0).toString() + " "+qw1.value(1).toString();

        QString review = "Пациент "+name+"("+date+"). "+ desc +".Оценка: "+QString::number(rating)+".";
        ui->textEdit_reviews->append(review+"\n");
    }
}

reviews_window::~reviews_window()//деструктор
{
    delete ui;
}
