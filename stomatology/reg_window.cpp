#include "reg_window.h"
#include "ui_reg_window.h"

reg_window::reg_window(QSqlDatabase *db, QWidget *parent,bool st) :
    QDialog(parent),
    database (parent,db),
    ui(new Ui::reg_window)//конструктор
{
    ui->setupUi(this);
    this->setWindowTitle("Регистрация");
    status = st;//для обработки режима регистрации
    ui->lineEdit_polis->setMaxLength(16);
    if(status)//если регистрируют врача
    {
        ui->lineEdit_polis->hide();
        QSqlQuery qw(*db);
        qw.prepare("SELECT * FROM data.specialization");//Запрос для вывода списка специальностей
        qw.exec();
        while(qw.next())
        {
            ui->comboBox_specialization->addItem(qw.value(1).toString());
        }
    }
    else
    {
        ui->label_price->hide();
        ui->spinBox_price->hide();
        ui->comboBox_specialization->hide();
    }
}

reg_window::~reg_window()//деструктор
{
    delete ui;
}

QString reg_window::hashPassword(QString str)//функция хеширования вводимого пароля
{
    // Создания объекта хэша и запись данных в него
    QCryptographicHash crypto(QCryptographicHash::Md5);
    crypto.addData(str.toUtf8());
    return QString::fromUtf8(crypto.result().toHex());
}


void reg_window::on_pushButton_reg_clicked()//обработка регистрации
{
    QSqlQuery qw(*db);
    auto login = ui->lineEdit_login->text().trimmed();
    auto password = ui->lineEdit_password->text().trimmed();
    auto firstName = ui->lineEdit_firstName->text().trimmed();
    auto secondName = ui->lineEdit_secondName->text().trimmed();
    auto polis = ui->lineEdit_polis->text().trimmed();
    auto bd = QDate::fromString(ui->dateEdit_bd->date().toString("dd.MM.yyyy"),"dd.MM.yyyy").toString("yyyy-MM-dd");
    auto price = ui->spinBox_price->value();

    if(status)//если врач
    {
        if(login == nullptr || password == nullptr || firstName == nullptr || secondName == nullptr)
        {
            QMessageBox::warning(this,"Ошибка","Заполните все поля");
        }
        else
        {
            if(login.length() < 4)
            {
                QMessageBox::warning(this,"Ошибка","Логин должен содержать как минимум 4 символа");
            }
            else
            {
                if(password.length() < 6 || password.length() > 20)
                {
                    QMessageBox::warning(this,"Ошибка","Пароль должен содержать от 6 до 20 символов");
                }
                else
                {
                    if(ui->comboBox_specialization->currentIndex() ==0)
                    {
                        QMessageBox::warning(this,"Ошибка","Выберите специальность врача");
                    }
                    else
                    {
                        qw.prepare("SELECT * FROM data.doctor WHERE login = :1");
                        qw.bindValue(":1",login);
                        qw.exec();
                        if(qw.size()>0)
                        {
                            QMessageBox::warning(this,"Ошибка","Данный логин занят");
                        }
                        else
                        {
                            qw.prepare("INSERT INTO data.doctor (login,password,first_name,second_name,birthday,id_specialization,price) VALUES (:1,:2,:3,:4,:5,:6,:7)");
                            qw.bindValue(":1",login);
                            qw.bindValue(":2",hashPassword(password));
                            qw.bindValue(":3",firstName);
                            qw.bindValue(":4",secondName);
                            qw.bindValue(":5",bd);
                            qw.bindValue(":6",ui->comboBox_specialization->currentIndex());
                            qw.bindValue(":7",price);

                            qw.exec();
                            if(qw.lastError().type() == QSqlError::NoError)
                            {
                                QMessageBox::information(this,"Регистрация","Успешно");
                                reject();
                            }
                            else
                            {
                                QMessageBox::warning(this,"Ошибка",qw.lastError().text());
                            }
                        }
                    }
                }
            }
        }
    }
    else //если пациент
    {
        if(login == nullptr  || password == nullptr  || firstName == nullptr || secondName == nullptr || polis == nullptr)
        {
            QMessageBox::warning(this,"Ошибка","Заполните все поля");
        }
        else
        {
            if(login.length() < 4)
            {
                QMessageBox::warning(this,"Ошибка","Логин должен содержать как минимум 4 символа");
            }
            else
            {
                if(password.length() < 6 || password.length() > 20)
                {
                    QMessageBox::warning(this,"Ошибка","Пароль должен содержать от 6 до 20 символов");
                }
                else
                {
                    if(!polis.contains(QRegularExpression("[0-9]{16}")) || polis.length() != 16)
                    {
                        QMessageBox::warning(this,"Ошибка","Полис должен содержать 16 цифр без пробелов");
                    }
                    else
                    {
                        qw.prepare("SELECT * FROM data.user WHERE login = :1");
                        qw.bindValue(":1",login);
                        qw.exec();
                        if(qw.size()>0)
                        {
                            QMessageBox::warning(this,"Ошибка","Данный логин занят");
                        }
                        else
                        {
                            qw.prepare("SELECT * FROM data.user WHERE polis = :1");
                            qw.bindValue(":1",polis);
                            qw.exec();
                            if(qw.size()>0)
                            {
                                QMessageBox::warning(this,"Ошибка","Пользователь с данным полисом уже существует");
                            }
                            else
                            {
                                qw.prepare("INSERT INTO data.user (login,password,first_name,second_name,birthday,polis) VALUES (:1,:2,:3,:4,:5,:6)");
                                qw.bindValue(":1",login);
                                qw.bindValue(":2",hashPassword(password));
                                qw.bindValue(":3",firstName);
                                qw.bindValue(":4",secondName);
                                qw.bindValue(":5",bd);
                                qw.bindValue(":6",polis);

                                qw.exec();

                                if(qw.lastError().type() == QSqlError::NoError)
                                {
                                    QMessageBox::information(this,"Регистрация","Успешно");
                                    reject();
                                }
                                else
                                {
                                    QMessageBox::warning(this,"Ошибка",qw.lastError().text());
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}
