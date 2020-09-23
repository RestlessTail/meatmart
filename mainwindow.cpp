#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modification.h"
#include "reservation.h"
#include "deal.h"
#include "gettimetable.h"
#include "updateserver.h"
#include "clerk_management.h"
#include "database_option.h"
#include "ui_database_option.h"


#include <QTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList header_h, header_v;
    header_h << "周一";
    header_h << "周二";
    header_h << "周三";
    header_h << "周四";
    header_h << "周五";
    header_h << "周六";
    header_h << "周日";
    header_v << "上午";
    header_v << "下午";
    header_v << "晚上";
    ui->schedule->setColumnCount(7);
    ui->schedule->setRowCount(3);
    ui->schedule->setHorizontalHeaderLabels(header_h);
    ui->schedule->setVerticalHeaderLabels(header_v);
    ui->schedule->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->schedule->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    log("应用程序启动");
    service_category = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(QString msg)
{
    QTime current_time =QTime::currentTime();
    QString time = current_time.toString();
    ui->log_output->append("> " + time + " " + msg);
}

void MainWindow::set_item_num(int n)
{
    QString s = QString::number(n);
    ui->num_of_reservations->setText(s);
}

void MainWindow::add_reservation_item(QString info)
{
    ui->reservation_list->addItem(info);
}

void MainWindow::get_access_token()
{
    QNetworkRequest request;//先获取access token
    QNetworkAccessManager* naManager = new QNetworkAccessManager(this);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(return_access_token(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid="+ appid + "&secret=" + appsecret));
    QNetworkReply* reply = naManager->get(request);
}

void MainWindow::update_combobox()
{
    ui->photographers->blockSignals(true);
    ui->photographers->clear();
    ui->photographers->blockSignals(false);
    for(QList<profile>::iterator i = photographer.begin(); i != photographer.end(); ++i){
        ui->photographers->addItem(i->name);
    }
}

void MainWindow::return_access_token(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(!statusCode.isValid()){
        log("连接失败，status code=" + statusCode.toString());
        return;
    }

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        log("无法获取token，status code=" + reply->errorString());
        return;
    }
    else {
        QString data = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data.toLocal8Bit().data());
        if(document.isNull()){
            log("access token格式错误");
        }
        QJsonObject object = document.object();
        service_category->parent_ptr->access_token = object.find("access_token").value().toString();
    }

    service_category->exec_cloud_service();
}

void MainWindow::on_refresh1_clicked()
{
    service_category = new Reservations(this);
    get_access_token();
}

void MainWindow::on_reservation_list_currentRowChanged(int currentRow)
{
    QString time;
    QStringList dayName;
    dayName << "星期一";
    dayName << "星期二";
    dayName << "星期三";
    dayName << "星期四";
    dayName << "星期五";
    dayName << "星期六";
    dayName << "星期日";
    time += dayName[reservation_list[currentRow].begin.day];
    time += reservation_list[currentRow].begin.time;
    time += "-";
    time += dayName[reservation_list[currentRow].end.day];
    time += reservation_list[currentRow].end.time;
    ui->name->setText(reservation_list[currentRow].name);
    ui->tel->setText(reservation_list[currentRow].tel);
    ui->time->setText(time);
    ui->photographer->setText(reservation_list[currentRow].photographer);
    ui->location->setText(reservation_list[currentRow].location);
    ui->timestamp->setText(reservation_list[currentRow].timestamp);
    ui->id->setText(reservation_list[currentRow].id);
}

void MainWindow::on_deal_clicked()
{
    if(ui->reservation_list->currentRow() == -1){
        return;
    }
    if(!ui->no_warning->isChecked()){
        if(QMessageBox::information(this, "提示", "此操作不可撤销，请确认订单信息并及时联系客户。", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel){
            return;
        }
    }
    service_category = new Deal(this);
    get_access_token();
}

void MainWindow::on_refresh2_clicked()
{
	service_category = new getTimetable(this);
	get_access_token();
}



void MainWindow::update_chart(int n)
{
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 7; j++){
            QString available_t = "空闲";
            QString busy_t = "忙碌";
            QColor available_c = QColor(0, 255, 0);
            QColor busy_c = QColor(255, 0, 0);
            bool isAvailable = photographer.at(n).schedule[i][j];
            QTableWidgetItem* item = new QTableWidgetItem(isAvailable ? available_t : busy_t);
            ui->schedule->setItem(i, j, item);
            ui->schedule->item(i, j)->setBackgroundColor(isAvailable ? available_c : busy_c);
        }
    }
}

void MainWindow::on_photographers_currentIndexChanged(int index)
{
    update_chart(index);
}

void MainWindow::on_modify_status_clicked()
{
    int photographerNum = ui->photographers->currentIndex();
    if(photographerNum == -1){
        return;
    }

    modification dialog;
    if(!dialog.exec()){
        return;
    }
    bool status = dialog.getValue();

    QList<QTableWidgetItem*> selected = ui->schedule->selectedItems();
    int selectedNum = selected.size();
    for (int i = 0; i < selectedNum; i++) {
        int row = selected.at(i)->row();
        int column = selected.at(i)->column();
        photographer[photographerNum].schedule[row][column] = status;
    }
    update_chart(photographerNum);
}

void MainWindow::on_quick_modification_clicked()
{
    int photographerNum = ui->photographers->currentIndex();
    if(photographerNum == -1){
        return;
    }

    QList<QTableWidgetItem*> selected = ui->schedule->selectedItems();
    int selectedNum = selected.size();
    for (int i = 0; i < selectedNum; i++) {
        int row = selected.at(i)->row();
        int column = selected.at(i)->column();
        photographer[photographerNum].schedule[row][column] = !photographer[photographerNum].schedule[row][column];
    }
    update_chart(photographerNum);
}



void MainWindow::on_update_server_clicked()
{
    int index = ui->photographers->currentIndex();
    service_category = new updateServer(this, index, Server::MODIFY);
    get_access_token();
}

void MainWindow::on_export_log_clicked()
{
    QString title="导出日志";
    QString currentPath=QCoreApplication::applicationDirPath();
    QString filter="日志文件(*.log)";
    QString filename=QFileDialog::getSaveFileName(this,title,currentPath,filter);
    if (!filename.isEmpty()){
        QFile file(filename);
        /*file.open(QIODevice::WriteOnly);
        file.close();*/
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
           QTextStream stream(&file);
           stream.seek(file.size());

           stream << ui->log_output->toPlainText() << "\n";
           file.close();
        }
    }
}

void MainWindow::on_clerk_management_clicked()
{
    clerk_management dialog(this);
    dialog.exec();
}

void MainWindow::on_to_clerk_management_triggered()
{
    on_clerk_management_clicked();
}

void MainWindow::on_to_database_option_triggered()
{
    database_option dialog;
    dialog.exec();
    appid = dialog.ui->appid->text();
    appsecret = dialog.ui->appsecret->text();
    env = dialog.ui->env->text();

}
