#include "dialogforreview.h"
#include "ui_dialogforreview.h"
#include <QMessageBox>

DialogForReview::DialogForReview(QSqlDatabase *db, QWidget *parent, int doctor, int user, bool fl) :
    QDialog(parent),
    database(parent, db),
    ui(new Ui::DialogForReview)//конструктор
{
    ui->setupUi(this);
    //Считываем переданные значения
    id_doctor = doctor;
    id_user = user;
    status = fl;

    if(status)//если это режим изменения отзыва , то ищем и выводим старые значения
    {
        QSqlQuery qw(*db);
        qw.prepare("SELECT * FROM data.reviews WHERE id_doctor = :1 AND id_user = :2");
        qw.bindValue(":1",id_doctor);
        qw.bindValue(":2",id_user);
        qw.exec();
        qw.next();

        ui->spinBox_estimation->setValue(qw.value(3).toInt());
        ui->lineEdit_review->setPlaceholderText(qw.value(4).toString());
    }
}

DialogForReview::~DialogForReview()//деструктор
{
    delete ui;
}

void DialogForReview::on_pushButton_add_clicked()//обработка добавления отзыва
{
    QSqlQuery qw(*db);

    if(status)
    {
        qw.prepare("UPDATE data.reviews SET rating =:3,description =:4,review_date= :5 WHERE id_doctor = :1 AND id_user = :2");
        qw.bindValue(":1",id_doctor);
        qw.bindValue(":2",id_user);
        qw.bindValue(":3",ui->spinBox_estimation->value());
        qw.bindValue(":4",ui->lineEdit_review->text().trimmed());
        qw.bindValue(":5",QDate::currentDate().toString("yyyy-MM-dd"));
    }
    else
    {
        qw.prepare("INSERT INTO data.reviews (id_doctor,id_user,rating,description,review_date) VALUES (:1,:2,:3,:4,:5)");
        qw.bindValue(":1",id_doctor);
        qw.bindValue(":2",id_user);
        qw.bindValue(":3",ui->spinBox_estimation->value());
        qw.bindValue(":4",ui->lineEdit_review->text().trimmed());
        qw.bindValue(":5",QDate::currentDate().toString("yyyy-MM-dd"));
    }
    qw.exec();
    if(qw.lastError().type() == QSqlError::NoError)
    {
        if(status)
        {
            QMessageBox::information(this,"Успешно","Отзыв изменен");
        }
        else
        {
            QMessageBox::information(this,"Успешно","Отзыв отправлен");
        }
        reject();//закрытие окна
    }
    else
    {
        QMessageBox::warning(this,"Ошибка",qw.lastError().text());
    }
}

void DialogForReview::on_pushButton_close_clicked()//обработка закрытия окна
{
    reject();//закрытие окна
}
