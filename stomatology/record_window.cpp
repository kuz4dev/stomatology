#include "record_window.h"
#include "ui_record_window.h"

record_window::record_window(QSqlDatabase *db, QWidget *parent, int id_tt, int id_d,int user) :
    QDialog(parent),
    database (parent,db),
    ui(new Ui::record_window)//конструктор
{
    ui->setupUi(this);
    id_timetable = id_tt;
    id_doctor = id_d;
    id_user = user;

    ui->timeEdit->setReadOnly(true);
    ui->timeEdit->setMinimumTime(QTime::fromMSecsSinceStartOfDay(10 * 3600000));
    this->setWindowTitle("Запись");
    QSqlQuery qw(*db);
    qw.prepare("SELECT * FROM data.doctor WHERE id_doctor = :1");
    qw.bindValue(":1",id_doctor);
    qw.exec();
    qw.next();

    QString name = qw.value(3).toString() + " " + qw.value(4).toString();
    int id_specialization = qw.value(7).toInt();

    ui->lineEdit_price->setText(QString::number(qw.value(6).toInt()));

    qw.prepare("SELECT * FROM data.specialization WHERE id_specialization = :1");
    qw.bindValue(":1",id_specialization);
    qw.exec();
    qw.next();

    name = name +"("+qw.value(1).toString()+")";

    qw.prepare("SELECT * FROM data.timetable WHERE id_timetable = :1");
    qw.bindValue(":1",id_timetable);
    qw.exec();
    qw.next();


    auto date = QDate::fromString(qw.value(2).toString(),"yyyy-MM-dd").toString("dd.MM.yyyy");


    if(QDate::fromString(date,"dd.MM.yyyy") == QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
    {
        int step = 3600000;
        int new_time = (QTime::currentTime().hour()*3600000+QTime::currentTime().minute()*60000+QTime::currentTime().second()*1000 + step)/step;
        ui->timeEdit->setMinimumTime(QTime::fromMSecsSinceStartOfDay(new_time * step));
    }
    ui->lineEdit_doctor->setText(name);
    ui->dateEdit->setDate(QDate::fromString(date,"dd.MM.yyyy"));
}

record_window::~record_window()//деструктор
{
    delete ui;
}

void record_window::on_pushButton_record_clicked()//обработка отправки запроса на запись
{
    auto time = ui->timeEdit->time().toString("hh:mm");

    QSqlQuery qw(*db);
    qw.prepare("SELECT * FROM data.record WHERE id_doctor = :1 AND record_date = :2 AND record_time = :3");
    qw.bindValue(":1",id_doctor);
    qw.bindValue(":2",ui->dateEdit->date().toString("yyyy-MM-dd"));
    qw.bindValue(":3",time);
    qw.exec();

    if(qw.size()>0)
    {
        QMessageBox::warning(this,"Ошибка","Данное время занято");
    }
    else
    {
        qw.prepare("INSERT INTO data.record (id_user,id_doctor,record_date,record_time) VALUES (:1,:2,:3,:4)");
        qw.bindValue(":1",id_user);
        qw.bindValue(":2",id_doctor);
        qw.bindValue(":3",ui->dateEdit->date().toString("yyyy-MM-dd"));
        qw.bindValue(":4",time);
        qw.exec();

        if(qw.lastError().type() == QSqlError::NoError)
        {
            QMessageBox::information(this,"Успешно","Запись успешно создана");
            reject();
        }
        else
        {
            QMessageBox::warning(this,"Ошибка",qw.lastError().text());
        }
    }
}

void record_window::on_pushButton_plus_clicked()//устанавливаем шаг времени в полчаса для записи при прокрутке вверх
{
    ui->timeEdit->setTime(ui->timeEdit->time().addMSecs(1800000));
}

void record_window::on_pushButton_minus_clicked()//устанавливем шаг времени в полчаса для записи при прокрутке вверх
{
    ui->timeEdit->setTime(ui->timeEdit->time().addMSecs(-1800000));
}
