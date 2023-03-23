#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    database(parent),
    ui(new Ui::MainWindow)//конструктор
{
    ui->setupUi(this);
    connectionWithDB();
    this->setWindowTitle("Стоматология");
    //Соединяем сигналы со слотом
    connect(ui->radioButton_doc,SIGNAL(clicked()),this,SLOT(hideButton()));
    connect(ui->radioButton_admin,SIGNAL(clicked()),this,SLOT(hideButton()));
    connect(ui->pushButton_back,SIGNAL(clicked()),this,SLOT(backWindow()));
    connect(ui->pushButton_back_2,SIGNAL(clicked()),this,SLOT(backWindow()));
    connect(ui->pushButton_back_3,SIGNAL(clicked()),this,SLOT(backWindow()));
    ui->tabWidget->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(2,false);
    ui->tabWidget->setTabEnabled(3,false);
    QSqlQuery qw(*db);
    qw.prepare("SELECT * FROM data.specialization");//Запрос для вывода списка специальностей
    qw.exec();
    while(qw.next())
    {
        ui->comboBox_specialization->addItem(qw.value(1).toString());
    }

    //    record_window * dialog = new record_window(db,this,1,2,id);//создание окна
    //    dialog->setModal(true);
    //    dialog->show();//показ окна
}

MainWindow::~MainWindow()//деструктор
{
    delete db;
    delete ui;
}

void MainWindow::hideButton()//обработка сигнала для скрытия кнопки
{
    ui->pushButton_toReg->hide();
}

QString MainWindow::hashPassword(QString str)//функция хеширования вводимого пароля
{
    // Создания объекта хэша и запись данных в него
    QCryptographicHash crypto(QCryptographicHash::Md5);
    crypto.addData(str.toUtf8());
    // Собственно хэширование и превращение результата в строку
    return QString::fromUtf8(crypto.result().toHex());
}


void MainWindow::connectionWithDB()//функция подключения к БД
{
    //=====================Подключение к бд============================
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL"));
    QString dbName("stomatology"); // имя БД
    QString host("127.0.0.1");//адрес сервера
    QString usr("postgres"); // логин
    QString pwd("1234");//пароль

    db->setDatabaseName(dbName);
    db->setHostName(host);
    db->setUserName(usr);
    db->setPassword(pwd);
    db->setPort(5432);

    db->open();
}

void MainWindow::on_pushButton_toReg_clicked()//обработка вызова окна регистрации
{
    reg_window * dialog = new reg_window(db,this,false);//создание окна
    dialog->setModal(true);
    dialog->show();//показ окна
}


void MainWindow::on_radioButton_user_clicked()//обработка выбора пользователя
{
    ui->pushButton_toReg->show();
}

void MainWindow::on_pushButton_enter_clicked()//обработка нажатия на кнопку входа
{
    QSqlQuery qw(*db);
    auto login = ui->lineEdit_login->text().trimmed();
    auto password = hashPassword(ui->lineEdit_password->text().trimmed());

    if(ui->lineEdit_login->text() != nullptr && ui->lineEdit_password->text() != nullptr)
    {
        if(ui->radioButton_user->isChecked())
        {
            qw.prepare("SELECT * FROM data.user WHERE login = :1 AND password = :2");
        }
        else if (ui->radioButton_doc->isChecked())
        {
            qw.prepare("SELECT * FROM data.doctor WHERE login = :1 AND password = :2");
        }
        else if(ui->radioButton_admin->isChecked())
        {
            qw.prepare("SELECT * FROM data.admin WHERE login = :1 AND password = :2");
        }

        qw.bindValue(":1",login);
        qw.bindValue(":2",password);
        qw.exec();

        if(qw.size()>0)
        {
            qw.next();
            id = qw.value(0).toInt();
            if(ui->radioButton_user->isChecked())
            {
                ui->tabWidget->setTabEnabled(1,true);
                ui->tabWidget->setTabEnabled(2,false);
                ui->tabWidget->setTabEnabled(3,false);
                ui->tabWidget->setCurrentIndex(1);
                ui->pushButton_enter->setEnabled(false);
                polis = qw.value(6).toString();
                ui->lineEdit_polis->setText(polis);
                ui->stackedWidget_user->setCurrentIndex(0);
            }
            else if (ui->radioButton_doc->isChecked())
            {
                ui->tabWidget->setTabEnabled(1,false);
                ui->tabWidget->setTabEnabled(2,true);
                ui->tabWidget->setTabEnabled(3,false);
                ui->tabWidget->setCurrentIndex(2);
                ui->stackedWidget_doctor->setCurrentIndex(0);
                ui->pushButton_enter->setEnabled(false);
            }
            else if(ui->radioButton_admin->isChecked())
            {
                ui->tabWidget->setTabEnabled(1,false);
                ui->tabWidget->setTabEnabled(2,false);
                ui->tabWidget->setTabEnabled(3,true);
                ui->tabWidget->setCurrentIndex(3);
                ui->stackedWidget_admin->setCurrentIndex(0);
                ui->pushButton_enter->setEnabled(false);
            }
        }
        else
        {
            QMessageBox::warning(this,"Ошибка","Пользователь с такими данными не был найден");
        }

    }
    else
    {
        QMessageBox::warning(this,"Ошибка","Введите данные для входа");
    }

}

void MainWindow::on_lineEdit_login_textChanged()//обработка изменения вводимых значений
{
    ui->pushButton_enter->setEnabled(true);
}

void MainWindow::on_lineEdit_password_textChanged()//обработка изменения вводимых значений
{
    ui->pushButton_enter->setEnabled(true);
}

void MainWindow::on_pushButton_toRecord_clicked()//обработка переход на окно записи ко врачу
{
    ui->stackedWidget_user->setCurrentIndex(1);
    ui->dateEdit->setMinimumDate(QDate::currentDate());
    ui->dateEdit->setDate(QDate::currentDate());
    ui->label_date->setText("Дата - "+QDate::currentDate().toString("dd.MM.yyyy"));
    searchRecords();
}

void MainWindow::on_pushButton_toRecommendation_clicked()//обработка перехода на окно списка рекомендаций от врачей
{
    ui->stackedWidget_user->setCurrentIndex(2);
    updateRecommendation();
}

void MainWindow::on_pushButton_toHistory_clicked()//обработка перехода на окно истории приемов
{
    ui->stackedWidget_user->setCurrentIndex(3);
    updateHistory();
}

void MainWindow::backWindow()//обработка перехода на начальное окно
{
    ui->stackedWidget_user->setCurrentIndex(0);
}

void MainWindow::searchRecords()
{
    QSqlQuery query(*db);
    int currentRow = 0;
    auto doc = ui->lineEdit_search->text().trimmed();
    auto date = QDate::fromString(ui->dateEdit->date().toString("dd.MM.yyyy"),"dd.MM.yyyy").toString("yyyy-MM-dd");
    auto id_specialisation = ui->comboBox_specialization->currentIndex();
    ui->label_date->setText("Дата - "+ ui->dateEdit->date().toString("dd.MM.yyyy"));

    if(doc == nullptr && id_specialisation == 0)//если только дата
    {
        query.prepare("SELECT * FROM data.timetable WHERE date_timetable = :1");
        query.bindValue(":1",date);
    }
    else if(doc == nullptr) //если только дата и специальность
    {
        query.prepare("SELECT * FROM data.timetable WHERE date_timetable = :1 AND id_doctor IN(SELECT id_doctor FROM data.doctor WHERE id_specialization = :2)");
        query.bindValue(":1",date);
        query.bindValue(":2",id_specialisation);
    }
    else if (id_specialisation == 0)//если только дата и фамилия
    {
        query.prepare("SELECT * FROM data.timetable WHERE date_timetable = :1 AND id_doctor IN(SELECT id_doctor FROM data.doctor WHERE second_name ALIKE :2)");
        query.bindValue(":1",date);
        query.bindValue(":2",doc);
    }
    else // если дата,специальность и фамилия
    {
        query.prepare("SELECT * FROM data.timetable WHERE date_timetable = :1 AND id_doctor IN(SELECT id_doctor FROM data.doctor WHERE id_specialization = :2 AND second_name ALIKE :3)");
        query.bindValue(":1",date);
        query.bindValue(":2",id_specialisation);
        query.bindValue(":3",doc);
    }

    query.exec();
    ui->tableWidget->setRowCount(query.size());

    if(query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",query.lastError().text());
    }
    else
    {
        while (query.next())
        {
            for (int i=0;i<4;i++)
            {
                if(ui->tableWidget->item(currentRow,i) == nullptr)
                {
                    QTableWidgetItem *ti;
                    ti = new QTableWidgetItem;
                    ti->setFlags(ti->flags() & 0x3d);
                    ui->tableWidget->setItem(currentRow,i,ti);
                }
            }

            QSqlQuery qw(*db);
            qw.prepare("SELECT * FROM data.doctor WHERE id_doctor = :1");
            qw.bindValue(":1",query.value(1).toInt());
            qw.exec();
            qw.next();
            int id_doctor = qw.value(0).toInt();
            id_specialisation = qw.value(7).toInt();
            ui->tableWidget->item(currentRow,0)->setText(qw.value(3).toString()  + " " + qw.value(4).toString());

            qw.prepare("SELECT * FROM data.specialization WHERE id_specialization = :1");
            qw.bindValue(":1",id_specialisation);
            qw.exec();
            qw.next();

            ui->tableWidget->item(currentRow,1)->setText(qw.value(1).toString());

            QPushButton * pb = new QPushButton;
            pb->setText("Отзывы");
            pb->setProperty("id",id_doctor);
            connect(pb, SIGNAL(clicked()), this, SLOT(readReview()));
            ui->tableWidget->setCellWidget(currentRow, 2, pb);

            qw.prepare("SELECT * FROM data.record WHERE id_user = :1 AND id_doctor = :2 AND record_date = :3");
            qw.bindValue(":1",id);
            qw.bindValue(":2",id_doctor);
            qw.bindValue(":3",date);
            qw.exec();


            QPushButton * pb_record = new QPushButton;
            if(qw.size()>0)
            {
                pb_record->setText("Уже записаны");
                pb_record->setEnabled(false);
            }
            else
            {
                pb_record->setText("Записаться");
                pb_record->setProperty("id",query.value(0).toInt());
                pb_record->setProperty("id_doc",id_doctor);
                connect(pb_record, SIGNAL(clicked()), this, SLOT(setRecord()));
            }
            ui->tableWidget->setCellWidget(currentRow, 3, pb_record);

            currentRow++;
        }
    }
}


void MainWindow::readReview()//обработка открытия окна с отзывами на врача
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int id_doctor = cbox->property("id").toInt();
    reviews_window * dialog = new reviews_window(db,this,id_doctor);//создание окна
    dialog->setModal(true);
    dialog->show();//показ окна
}

void MainWindow::setRecord()//обработка открытия окна для записи на прием
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int id_timetable = cbox->property("id").toInt();
    int id_doctor = cbox->property("id_doc").toInt();
    record_window * dialog = new record_window(db,this,id_timetable,id_doctor,id);//создание окна
    dialog->setModal(true);
    auto x = dialog->exec();//показ окна
    if(x == QDialog::Rejected)
    {
        QTimer::singleShot(100,this,SLOT(timer()));
    }
    searchRecords();
}

void MainWindow::on_pushButton_search_clicked()//обработка поиска записей
{
    searchRecords();
}

void MainWindow::updateRecommendation()//обработка обновления списка рекомендаций
{
    QSqlQuery query(*db);
    int currentRow = 0;

    query.prepare("SELECT * FROM data.recommendation WHERE id_record IN (SELECT id_record FROM data.record WHERE id_user = :1)");
    query.bindValue(":1",id);

    query.exec();
    ui->tableWidget_recommendation->setRowCount(query.size());

    if(query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",query.lastError().text());
    }
    else
    {
        while (query.next())
        {
            for (int i=0;i<4;i++)
            {
                if(ui->tableWidget_recommendation->item(currentRow,i) == nullptr)
                {
                    QTableWidgetItem *ti;
                    ti = new QTableWidgetItem;
                    ti->setFlags(ti->flags() & 0x3d);
                    ui->tableWidget_recommendation->setItem(currentRow,i,ti);
                }
            }

            QSqlQuery qw(*db);
            int id_record = query.value(1).toInt();
            QString str = query.value(2).toString();

            qw.prepare("SELECT * FROM data.record WHERE id_record = :1");
            qw.bindValue(":1",id_record);
            qw.exec();
            qw.next();

            int id_doctor = qw.value(2).toInt();

            auto date = QDate::fromString(qw.value(3).toString(),"yyyy-MM-dd").toString("dd.MM.yyyy");
            ui->tableWidget_recommendation->item(currentRow,0)->setText(date);//дата


            qw.prepare("SELECT * FROM data.doctor WHERE id_doctor = :1");
            qw.bindValue(":1",id_doctor);
            qw.exec();
            qw.next();

            int id_specialisation = qw.value(7).toInt();
            QString name = qw.value(3).toString()  + " " + qw.value(4).toString();
            ui->tableWidget_recommendation->item(currentRow,1)->setText(name);//врач

            qw.prepare("SELECT * FROM data.specialization WHERE id_specialization = :1");
            qw.bindValue(":1",id_specialisation);
            qw.exec();
            qw.next();

            ui->tableWidget_recommendation->item(currentRow,2)->setText(qw.value(1).toString());//специальность

            QPushButton * pb = new QPushButton;
            pb->setText("Прочитать");
            pb->setProperty("str",str);
            pb->setProperty("doctor",name);
            pb->setProperty("date",date);
            connect(pb, SIGNAL(clicked()), this, SLOT(readRecommendation()));
            ui->tableWidget_recommendation->setCellWidget(currentRow, 3, pb);


            currentRow++;
        }
    }
}

void MainWindow::readRecommendation()//обработка открытия окна с рекомендацией
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    QString str = cbox->property("str").toString();
    QString name = cbox->property("doctor").toString();
    QString date = cbox->property("date").toString();

    QMessageBox msgBox;
    msgBox.setText("Рекомендация за "+date);
    msgBox.setInformativeText(str);
    msgBox.setWindowTitle(name);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setModal(true);
    msgBox.exec();
}

void MainWindow::updateHistory()//обработка обновления списка приемов
{
    QSqlQuery query(*db);
    int currentRow = 0;

    query.prepare("SELECT * FROM data.record WHERE id_user = :1 ORDER BY record_date DESC");
    query.bindValue(":1",id);

    query.exec();
    ui->tableWidget_records->setRowCount(query.size());

    if(query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",query.lastError().text());
    }
    else
    {
        while (query.next())
        {
            for (int i=0;i<5;i++)
            {
                if(ui->tableWidget_records->item(currentRow,i) == nullptr)
                {
                    QTableWidgetItem *ti;
                    ti = new QTableWidgetItem;
                    ti->setFlags(ti->flags() & 0x3d);
                    ui->tableWidget_records->setItem(currentRow,i,ti);
                }
            }

            QSqlQuery qw(*db);
            int id_doctor = query.value(2).toInt();

            auto date = QDate::fromString(query.value(3).toString(),"yyyy-MM-dd").toString("dd.MM.yyyy");
            auto time = QTime::fromString(query.value(4).toString()).toString("hh:mm");
            ui->tableWidget_records->item(currentRow,0)->setText(date);//дата
            ui->tableWidget_records->item(currentRow,1)->setText(time);//время


            qw.prepare("SELECT * FROM data.doctor WHERE id_doctor = :1");
            qw.bindValue(":1",id_doctor);
            qw.exec();
            qw.next();

            int id_specialisation = qw.value(7).toInt();
            QString name = qw.value(3).toString()  + " " + qw.value(4).toString();
            ui->tableWidget_records->item(currentRow,2)->setText(name);//врач

            qw.prepare("SELECT * FROM data.specialization WHERE id_specialization = :1");
            qw.bindValue(":1",id_specialisation);
            qw.exec();
            qw.next();

            ui->tableWidget_records->item(currentRow,3)->setText(qw.value(1).toString());//специальность

            qDebug()<<QDate::fromString(date,"dd.MM.yyyy")<< QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy");

            QPushButton * pb = new QPushButton;
            if(QDate::fromString(date,"dd.MM.yyyy") > QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
            {
                pb->setText("Отменить");
                pb->setProperty("id_record",query.value(0).toInt());
                connect(pb, SIGNAL(clicked()), this, SLOT(cancelRecord()));
            }
            else if (QDate::fromString(date,"dd.MM.yyyy") == QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
            {
                if(QTime::fromString(time) > QTime::currentTime())
                {
                    pb->setText("Отменить");
                    pb->setProperty("id_record",query.value(0).toInt());
                    connect(pb, SIGNAL(clicked()), this, SLOT(cancelRecord()));
                }
                else
                {

                    qw.prepare("SELECT * FROM data.reviews WHERE id_doctor = :1 AND id_user = :2");
                    qw.bindValue(":1",id_doctor);
                    qw.bindValue(":2",id);
                    qw.exec();
                    if(qw.size()>0)
                    {
                        pb->setText("Изменить отзыв");
                        pb->setProperty("id_user",id);
                        pb->setProperty("id_doctor",id_doctor);
                        pb->setProperty("bool",true);
                    }
                    else
                    {
                        pb->setText("Создать отзыв");
                        pb->setProperty("id_user",id);
                        pb->setProperty("id_doctor",id_doctor);
                        pb->setProperty("bool",false);
                    }

                    connect(pb, SIGNAL(clicked()), this, SLOT(setReviews()));
                }

            }
            else if(QDate::fromString(date,"dd.MM.yyyy") < QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
            {
                qw.prepare("SELECT * FROM data.reviews WHERE id_doctor = :1 AND id_user = :2");
                qw.bindValue(":1",id_doctor);
                qw.bindValue(":2",id);
                qw.exec();
                if(qw.size()>0)
                {
                    pb->setText("Изменить отзыв");
                    pb->setProperty("id_user",id);
                    pb->setProperty("id_doctor",id_doctor);
                    pb->setProperty("bool",true);
                }
                else
                {
                    pb->setText("Создать отзыв");
                    pb->setProperty("id_user",id);
                    pb->setProperty("id_doctor",id_doctor);
                    pb->setProperty("bool",false);
                }

                connect(pb, SIGNAL(clicked()), this, SLOT(setReviews()));
            }

            ui->tableWidget_records->setCellWidget(currentRow, 4, pb);

            currentRow++;
        }
    }
}


void MainWindow::setReviews()//функция для отправки отзыва
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int doctor = cbox->property("id_doctor").toInt();
    int user = cbox->property("id_user").toInt();
    bool fl = cbox->property("bool").toBool();
    DialogForReview * dialog = new DialogForReview(db,this,doctor,user,fl);//создаем диалоговое окно для ввода отзыва
    dialog->setModal(true);//делаем окно модальным
    auto x = dialog->exec();//вызываем окно
    if(x == QDialog::Rejected)
    {
        QTimer::singleShot(100,this,SLOT(timer()));
    }
}

void MainWindow::cancelRecord()//обработка отмены записи
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int id_record = cbox->property("id_record").toInt();
    QSqlQuery qw(*db);
    qw.prepare("DELETE FROM data.record WHERE id_record = :1");
    qw.bindValue(":1",id_record);
    qw.exec();
    updateHistory();
}

void MainWindow::timer()//функция обработки таймера
{
    updateHistory();
    searchRecords();
}

void MainWindow::on_pushButton_toRegAdmin_clicked()//вызов окна регистрации врача
{
    reg_window * dialog = new reg_window(db,this,true);//создание окна
    dialog->setModal(true);
    dialog->show();//показ окна
}



void MainWindow::on_pushButton_back_4_clicked()//обработка перехода на начальное окно админа
{
    ui->stackedWidget_admin->setCurrentIndex(0);
}

void MainWindow::on_pushButton_toDoctors_clicked()//обработка перехода на список врачей
{
    ui->stackedWidget_admin->setCurrentIndex(1);
    updateDoctors();
}

void MainWindow::updateDoctors()//обработка обновления списка врачей
{
    QSqlQuery query(*db);
    int currentRow = 0;

    query.prepare("SELECT * FROM data.doctor");
    query.exec();
    ui->tableWidget_doctors->setRowCount(query.size());

    if(query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",query.lastError().text());
    }
    else
    {
        while (query.next())
        {
            for (int i=0;i<4;i++)
            {
                if(ui->tableWidget_doctors->item(currentRow,i) == nullptr)
                {
                    QTableWidgetItem *ti;
                    ti = new QTableWidgetItem;
                    ti->setFlags(ti->flags() & 0x3d);
                    ui->tableWidget_doctors->setItem(currentRow,i,ti);
                }
            }

            QSqlQuery qw(*db);

            int id_specialisation = query.value(7).toInt();
            int price = query.value(6).toInt();
            QString name = query.value(3).toString()  + " " + query.value(4).toString();
            ui->tableWidget_doctors->item(currentRow,0)->setText(name);//врач

            qw.prepare("SELECT * FROM data.specialization WHERE id_specialization = :1");
            qw.bindValue(":1",id_specialisation);
            qw.exec();
            qw.next();

            ui->tableWidget_doctors->item(currentRow,1)->setText(qw.value(1).toString());//специальность
            ui->tableWidget_doctors->item(currentRow,2)->setText(QString::number(price));//стоимость услуг


            QPushButton * pb = new QPushButton;
            pb->setText("Изменить стоимость");
            pb->setProperty("id_doctor",query.value(0).toInt());
            pb->setProperty("price",price);
            connect(pb, SIGNAL(clicked()), this, SLOT(changePrice()));
            ui->tableWidget_doctors->setCellWidget(currentRow, 3, pb);

            QPushButton * pb_dismiss = new QPushButton;
            pb_dismiss->setText("Уволить");
            pb_dismiss->setProperty("id_doctor",query.value(0).toInt());
            connect(pb_dismiss, SIGNAL(clicked()), this, SLOT(dismissDoctor()));
            ui->tableWidget_doctors->setCellWidget(currentRow, 4, pb_dismiss);


            currentRow++;
        }
    }
}

void MainWindow::changePrice()//обработка изменения стоимости услуг врача
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int id_doctor = cbox->property("id_doctor").toInt();
    int price = cbox->property("price").toInt();

    QSqlQuery qw(*db);

    bool ok;
    int newPrice = QInputDialog::getInt(this,"Стоимость","Введите новое значение (старое значение = "+QString::number(price)+")",1,1,100000,1,&ok);
    if(ok)
    {
        qw.prepare("UPDATE data.doctor SET price = :1 WHERE id_doctor = :2");
        qw.bindValue(":1",newPrice);
        qw.bindValue(":2",id_doctor);
        qw.exec();

        updateDoctors();
    }

}

void MainWindow::dismissDoctor()//обработка увольнения врачей
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int id_doctor = cbox->property("id_doctor").toInt();

    QSqlQuery qw(*db);

    qw.prepare("DELETE FROM data.doctor WHERE id_doctor = :1");
    qw.bindValue(":1",id_doctor);
    qw.exec();

    if(qw.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",qw.lastError().text());
    }
    updateDoctors();
}

void MainWindow::on_pushButton_Back_clicked()//обработка возврата на начальное окно
{
    ui->stackedWidget_doctor->setCurrentIndex(0);
}

void MainWindow::updateDays()//обновление списка рабочих дней
{
    QSqlQuery query(*db);
    int currentRow = 0;

    query.prepare("SELECT * FROM data.timetable WHERE id_doctor = :1 AND date_timetable >= :2");
    query.bindValue(":1",id);
    query.bindValue(":2",QDate::currentDate().toString("yyyy-MM-dd"));
    query.exec();
    ui->tableWidget_days->setRowCount(query.size());

    if(query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",query.lastError().text());
    }
    else
    {
        while (query.next())
        {
            for (int i=0;i<1;i++)
            {
                if(ui->tableWidget_days->item(currentRow,i) == nullptr)
                {
                    QTableWidgetItem *ti;
                    ti = new QTableWidgetItem;
                    ti->setFlags(ti->flags() & 0x3d);
                    ui->tableWidget_days->setItem(currentRow,i,ti);
                }
            }
            ui->tableWidget_days->item(currentRow,0)->setText(QDate::fromString(query.value(2).toString(),"yyyy-MM-dd").toString("dd.MM.yyyy"));
            currentRow++;
        }
    }
}

void MainWindow::on_pushButton_toSetDay_clicked()//обработка перехода на окно с рабочими днями
{
    ui->stackedWidget_doctor->setCurrentIndex(1);
    updateDays();
    ui->dateEdit_work->setMinimumDate(QDate::currentDate());
}

void MainWindow::on_pushButton_addDay_clicked()//обработка добавления рабочего дня
{
    QSqlQuery qw(*db);
    auto date = ui->dateEdit_work->date().toString("yyyy-MM-dd");

    qw.prepare("SELECT * FROM data.timetable WHERE id_doctor = :1 AND date_timetable = :2");
    qw.bindValue(":1",id);
    qw.bindValue(":2",date);
    qw.exec();

    if(qw.size()>0)
    {
        QMessageBox::warning(this,"Ошибка","В этот день вы уже работаете");
    }
    else
    {
        qw.prepare("INSERT INTO data.timetable (id_doctor,date_timetable) VALUES (:1,:2)");
        qw.bindValue(":1",id);
        qw.bindValue(":2",date);
        qw.exec();
        updateDays();
    }
}

void MainWindow::on_pushButton_toReceptions_clicked()//обработка перехода на окно списка приемов
{
    ui->stackedWidget_doctor->setCurrentIndex(2);
    updateReception();
}

void MainWindow::on_pushButton_Back_2_clicked()//обработка перехода на начальное окно
{
    ui->stackedWidget_doctor->setCurrentIndex(0);
}

void MainWindow::updateReception()//обработка обновления списка приемов
{
    QSqlQuery query(*db);
    int currentRow = 0;

    query.prepare("SELECT * FROM data.record WHERE id_doctor = :1 ORDER BY record_date");
    query.bindValue(":1",id);
    query.exec();
    ui->tableWidget_reception->setRowCount(query.size());

    if(query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this,"Ошибка",query.lastError().text());
    }
    else
    {
        while (query.next())
        {
            for (int i=0;i<5;i++)
            {
                if(ui->tableWidget_reception->item(currentRow,i) == nullptr)
                {
                    QTableWidgetItem *ti;
                    ti = new QTableWidgetItem;
                    ti->setFlags(ti->flags() & 0x3d);
                    ui->tableWidget_reception->setItem(currentRow,i,ti);
                }
            }

            QSqlQuery qw(*db);
            int id_user = query.value(1).toInt();

            qw.prepare("SELECT * FROM data.user WHERE id_user = :1");
            qw.bindValue(":1",id_user);
            qw.exec();
            qw.next();

            auto name = qw.value(3).toString() + " "+qw.value(4).toString();
            ui->tableWidget_reception->item(currentRow,0)->setText(name);//пациент

            QStringList str = qw.value(5).toString().split("-");
            int age=QDate::currentDate().year()-str[0].toInt();
            if(QDate(0,str[1].toInt(),str[2].toInt())>QDate(0,QDate::currentDate().month (),QDate::currentDate().day()))
                --age;

            ui->tableWidget_reception->item(currentRow,1)->setText(QString::number(age));//возраст


            auto date = QDate::fromString(query.value(3).toString(),"yyyy-MM-dd").toString("dd.MM.yyyy");
            auto time = QTime::fromString(query.value(4).toString()).toString("hh:mm");

            ui->tableWidget_reception->item(currentRow,2)->setText(date);//дата
            ui->tableWidget_reception->item(currentRow,3)->setText(time);//время



            QPushButton * pb = new QPushButton;

            if(QDate::fromString(date,"dd.MM.yyyy") > QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
            {
                pb->setText("Ожидание");
                pb->setEnabled(false);
            }
            else if (QDate::fromString(date,"dd.MM.yyyy") == QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
            {
                if(QTime::fromString(time) > QTime::currentTime())
                {
                    pb->setText("Ожидание");
                    pb->setEnabled(false);
                }
                else
                {

                    qw.prepare("SELECT * FROM data.recommendation WHERE id_record = :1");
                    qw.bindValue(":1",query.value(0).toInt());
                    qw.exec();
                    if(qw.size()>0)
                    {
                        pb->setText("Рекомендация отправлена");
                        pb->setEnabled(false);
                    }
                    else
                    {
                        pb->setText("Написать рекомендацию");
                        pb->setProperty("id_record",query.value(0).toInt());
                        connect(pb, SIGNAL(clicked()), this, SLOT(setRecommendation()));
                    }

                }

            }
            else if(QDate::fromString(date,"dd.MM.yyyy") < QDate::fromString(QDate::currentDate().toString("dd.MM.yyyy"),"dd.MM.yyyy"))
            {
                qw.prepare("SELECT * FROM data.recommendation WHERE id_record = :1");
                qw.bindValue(":1",query.value(0).toInt());
                qw.exec();
                if(qw.size()>0)
                {
                    pb->setText("Рекомендация отправлена");
                    pb->setEnabled(false);
                }
                else
                {
                    pb->setText("Написать рекомендацию");
                    pb->setProperty("id_record",query.value(0).toInt());
                    connect(pb, SIGNAL(clicked()), this, SLOT(setRecommendation()));
                }


            }
            ui->tableWidget_reception->setCellWidget(currentRow, 4, pb);


            currentRow++;
        }
    }
}

void MainWindow::setRecommendation()//обработка отправки рекомендации
{
    QPushButton * cbox = qobject_cast<QPushButton *>(sender());
    int id_record = cbox->property("id_record").toInt();

    QSqlQuery qw(*db);

    bool ok;
    QString rec = QInputDialog::getText(this,"Рекомендация","Напишите рекомендацию пациенту",QLineEdit::Normal,"",&ok);
    if(ok)
    {
        qw.prepare("INSERT INTO data.recommendation (id_record,description) VALUES (:1,:2)");
        qw.bindValue(":1",id_record);
        qw.bindValue(":2",rec);
        qw.exec();

        if(qw.lastError().type() != QSqlError::NoError) QMessageBox::warning(this,"Ошибка",qw.lastError().text());
        updateReception();
    }
}
