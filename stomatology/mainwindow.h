#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "database.h"
#include <QCryptographicHash>
#include "reg_window.h"
#include "reviews_window.h"
#include "record_window.h"
#include "dialogforreview.h"
#include <QInputDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow,private database
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void connectionWithDB();//функция подключения к БД
    QString hashPassword(QString str);//функция хеширования паролей
    void searchRecords();//функция для поиска записей
    void updateRecommendation();//функция обновления списка рекомендаций
    void updateHistory();//функция обновления списка записей
    void updateDoctors();//функция обновления списка врачей
    void updateDays();//функция для обновления списка рабочих дней
    void updateReception();//функция для обновления списка приемов

    ~MainWindow();

private slots:
    void on_pushButton_toReg_clicked();

    void hideButton();

    void on_radioButton_user_clicked();

    void on_pushButton_enter_clicked();

    void on_lineEdit_login_textChanged();

    void on_lineEdit_password_textChanged();

    void on_pushButton_toRecord_clicked();

    void on_pushButton_toRecommendation_clicked();

    void on_pushButton_toHistory_clicked();

    void backWindow();//слот для обработки перехода на начальное окно

    void readReview();//слот для обработки открытия окна с отзывами

    void setRecord();//слот для обработки открытия окна записи

    void readRecommendation();//слот для обработки открытия окна с рекомендацией

    void setReviews();//слот для обработки вызова окна отзывов

    void timer();//обработка таймера

    void changePrice();//слот для обработки вызова окна изменения

    void dismissDoctor();//слот для обработки увольнения врача

    void cancelRecord();//слот для обработки отмены записи

    void setRecommendation();//слот для обработки создания рекомендации

    void on_pushButton_search_clicked();

    void on_pushButton_toRegAdmin_clicked();

    void on_pushButton_back_4_clicked();

    void on_pushButton_toDoctors_clicked();

    void on_pushButton_Back_clicked();

    void on_pushButton_toSetDay_clicked();

    void on_pushButton_addDay_clicked();

    void on_pushButton_toReceptions_clicked();

    void on_pushButton_Back_2_clicked();

private:
    Ui::MainWindow *ui;
    int id;
    QString polis;
};

#endif // MAINWINDOW_H
