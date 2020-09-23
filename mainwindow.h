#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
/*#include <QTime>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QMessageBox>
#include <QFileDialog>
#include "modification.h"*/



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct profile;
class cloud_service;
class QNetworkReply;

enum Day{
    Monday = 1,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

struct specificTime{
    Day day;
    QString time;
};

struct reservation_item{
    QString name;
    QString tel;
    specificTime begin;
    specificTime end;
    QString photographer;
    QString location;
    QString timestamp;
    QString id;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void log(QString msg);

    void set_item_num(int n);

    void add_reservation_item(QString info);

    void get_access_token();

    void update_combobox();

    QString access_token;

    cloud_service* service_category;

    QList<reservation_item> reservation_list;

    QList<profile> photographer;

    bool isServerUpdateFinished;

    void update_chart(int n);

    //virtual void exec_cloud_service() = 0;//automatically called by return_access_token

public slots:
    void return_access_token(QNetworkReply* reply);

private slots:
    void on_refresh1_clicked();

    void on_reservation_list_currentRowChanged(int currentRow);

    void on_deal_clicked();

    void on_refresh2_clicked();

    void on_photographers_currentIndexChanged(int index);

    void on_modify_status_clicked();

    void on_quick_modification_clicked();

    void on_update_server_clicked();

    void on_export_log_clicked();

    void on_clerk_management_clicked();

    void on_to_clerk_management_triggered();

    void on_to_database_option_triggered();

public:
    Ui::MainWindow *ui;

public:

    QString appid;

    QString appsecret;

    QString env;
};

struct profile{
    bool schedule[3][7];
    QString name;
    int id;
    QString tel;
    QString _id;
};

#endif // MAINWINDOW_H
