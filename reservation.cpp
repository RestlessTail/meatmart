#include "reservation.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

Reservations::Reservations(MainWindow *parent)
{
    parent_ptr = parent;
}

void Reservations::exec_cloud_service()
{
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    QJsonDocument document;
    document.setObject(form);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(parent_ptr);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(download_collection_data(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/tcb/databasecollectionget?access_token=" + parent_ptr->access_token));

    QString testData = (QJsonDocument(form).toJson(QJsonDocument::Compact));
    QNetworkReply* reply = naManager->post(request, data);
}

void Reservations::analyse_collection_data(QString raw_data)
{
    QJsonDocument doc;
    QJsonObject obj;
    QJsonArray collections;
    doc = doc.fromJson(raw_data.toLocal8Bit().data());
    if(doc.isNull()){
        goto err;
    }

    obj = doc.object();
    collections = obj.find("collections")->toArray();

    for(QJsonArray::iterator i = collections.begin(); i != collections.end(); ++i){
        if(i->toObject().find("name")->toString() == "reservations"){
            num = i->toObject().find("count")->toInt();
            parent_ptr->set_item_num(num);
            break;
        }
    }
    return;
    err:
        parent_ptr->log("JSON解析错误");
        return;
}

void Reservations::get_reservations()
{
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    form.insert("query", "db.collection(\"reservations\").get()");
    QJsonDocument document;
    document.setObject(form);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QNetworkAccessManager* nManager = new QNetworkAccessManager(parent_ptr);
    QMetaObject::Connection connRet = QObject::connect(nManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(download_reservation_data(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/tcb/databasequery?access_token=" + parent_ptr->access_token));

    QString testData = (QJsonDocument(form).toJson(QJsonDocument::Compact));
    QNetworkReply* reply = nManager->post(request, data);
}

void Reservations::update_data(QString raw_data)
{
    raw_data = QUrl::toPercentEncoding(raw_data, "{}\"\\,.[]:");
    QJsonDocument doc;
    QJsonArray array;
    QJsonObject obj;
    doc = QJsonDocument::fromJson(raw_data.toLocal8Bit().data());
    if(doc.isEmpty()){
        parent_ptr->log("JSON解析失败");
        parent_ptr->log(raw_data);
        return;
    }
    obj = doc.object();
    array = obj.find("data")->toArray();

    auto getObj = [](QJsonArray::iterator i)->QJsonObject {
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(i->toString().toLocal8Bit());
        QJsonObject obj = doc.object();
        return obj;
    };

    parent_ptr->ui->reservation_list->blockSignals(true);
    parent_ptr->ui->reservation_list->clear();
    parent_ptr->ui->reservation_list->blockSignals(false);
    parent_ptr->reservation_list.clear();
    for(QJsonArray::iterator i = array.begin(); i != array.end(); ++i){
        QJsonObject obj = getObj(i);
        reservation_item item;
        item.name = QUrl::fromPercentEncoding(obj.find("username")->toString().toLocal8Bit());
        item.tel = QUrl::fromPercentEncoding(obj.find("userTel")->toString().toLocal8Bit());
        item.begin.day = (Day)obj.find("dateSelectedBegin")->toInt();
        item.begin.time = obj.find("timeSelectedBegin")->toString();
        item.end.day = (Day)obj.find("dateSelectedEnd")->toInt();
        item.end.time = obj.find("timeSelectedEnd")->toString();
        item.photographer = QUrl::fromPercentEncoding(obj.find("userPhotographer")->toString().toLocal8Bit());
        item.location = QUrl::fromPercentEncoding(obj.find("userLocation")->toString().toLocal8Bit());
        item.timestamp = obj.find("timestamp")->toString();
        item.id = QUrl::fromPercentEncoding(obj.find("_id")->toString().toLocal8Bit());
        parent_ptr->reservation_list.append(item);
        parent_ptr->add_reservation_item(item.name);
    }
    parent_ptr->log("订单列表已更新");
    delete this;
}

void Reservations::download_collection_data(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(!statusCode.isValid()){
        parent_ptr->log("连接失败，status code=" + statusCode.toString());
        return;
    }

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        parent_ptr->log("无法获取token，status code=" + reply->errorString());
        return;
    }
    else {
        analyse_collection_data(reply->readAll());
    }
    get_reservations();

}

void Reservations::download_reservation_data(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(!statusCode.isValid()){
        parent_ptr->log("连接失败，status code=" + statusCode.toString());
        return;
    }

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        parent_ptr->log("无法获取token，status code=" + reply->errorString());
        return;
    }
    else {
        update_data(reply->readAll());
    }
}
