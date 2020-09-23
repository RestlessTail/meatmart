#include "deal.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

Deal::Deal(MainWindow *parent)
{
    parent_ptr = parent;
}

void Deal::exec_cloud_service()
{
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    QString query;
    QString id = parent_ptr->reservation_list[parent_ptr->ui->reservation_list->currentRow()].id;
    query = "db.collection(\"reservations\").where({_id:\"" + id + "\"}).remove()";
    form.insert("query", query);
    QJsonDocument document;
    document.setObject(form);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(parent_ptr);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(is_successful(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/tcb/databasedelete?access_token=" + parent_ptr->access_token));

    QString testData = (QJsonDocument(form).toJson(QJsonDocument::Compact));
    QNetworkReply* reply = naManager->post(request, data);
}

void Deal::is_successful(QNetworkReply* reply)
{
    QString data = reply->readAll();
    QJsonDocument doc;
    QJsonObject obj;
    int errcode;
    doc = QJsonDocument::fromJson(data.toLocal8Bit().data());
    obj = doc.object();
    errcode = obj.find("errcode")->toInt();
    if(errcode != 0){
        parent_ptr->log("接单失败");
        parent_ptr->log(data);
    }
    else{
        QString log = "已接单\n";
        int current = parent_ptr->ui->reservation_list->currentRow();
        log += "id: " + parent_ptr->reservation_list[current].id + "\n";
        log += "姓名: " + parent_ptr->reservation_list[current].name + "\n";
        log += "联系方式: " + parent_ptr->reservation_list[current].tel + "\n";
        log += "预约时段: " + parent_ptr->ui->time->text() + "\n";
        log += "摄影师: " + parent_ptr->reservation_list[current].photographer + "\n";
        log += "地点: " + parent_ptr->reservation_list[current].location + "\n";
        log += "时间戳: " + parent_ptr->reservation_list[current].timestamp + "\n";
        parent_ptr->log(log);
        parent_ptr->ui->reservation_list->blockSignals(true);
        QListWidgetItem* item;
        item = parent_ptr->ui->reservation_list->takeItem(current);
        delete item;
        parent_ptr->ui->reservation_list->blockSignals(false);

        int num = parent_ptr->ui->num_of_reservations->text().toInt();
        num--;
        parent_ptr->set_item_num(num);
    }
    delete this;
}
