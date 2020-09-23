#include "gettimetable.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

void getTimetable::exec_cloud_service(){
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

void getTimetable::download_collection_data(QNetworkReply* reply){
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
    get_timetables();
}

void getTimetable::get_timetables(){
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    form.insert("query", "db.collection(\"photographerSchedule\").get()");
    QJsonDocument document;
    document.setObject(form);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QNetworkAccessManager* nManager = new QNetworkAccessManager(parent_ptr);
    QMetaObject::Connection connRet = QObject::connect(nManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(download_schedule_data(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/tcb/databasequery?access_token=" + parent_ptr->access_token));

    QString testData = (QJsonDocument(form).toJson(QJsonDocument::Compact));
    QNetworkReply* reply = nManager->post(request, data);
}

void getTimetable::download_schedule_data(QNetworkReply* reply){
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
        get_data(reply->readAll());
    }
}

getTimetable::getTimetable(MainWindow* parent){
    parent_ptr = parent;
}

void getTimetable::get_data(QString raw_data){
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
    if(array.isEmpty()){
        return;
    }

    auto getPhotographerInfo = [](QJsonObject obj, profile* p, QJsonArray* timetable){
        p->name = QUrl::fromPercentEncoding(obj.find("name")->toString().toLocal8Bit());
        p->id = obj.find("id")->toInt();
        p->tel = obj.find("tel")->toString();
        p->_id = obj.find("_id")->toString();
        *timetable = obj.find("timetable")->toArray();
    };

    auto getobj = [](QJsonArray::iterator i)->QJsonObject {
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(i->toString().toLocal8Bit());
        QJsonObject obj = doc.object();
        return obj;
    };

    parent_ptr->photographer.clear();
    for(QJsonArray::iterator i = array.begin(); i != array.end(); ++i){
        profile* p = new profile;
        QJsonArray table;
        QJsonObject obj = getobj(i);
        getPhotographerInfo(obj, p, &table);

        for(int k = 0; k < 3; k++){
            for(int l = 0; l < 7; l++){
                bool temp = table.at(k).toArray().at(l).toBool();
                p->schedule[k][l] = temp;
            }
        }
        parent_ptr->photographer.append(*p);
    }
    update_data();
    parent_ptr->log("排班表已更新");
    delete this;
}

void getTimetable::update_data()
{
    //bool foo = disconnect(parent_ptr->ui->photographers, 0, 0, 0);
    parent_ptr->update_combobox();
    parent_ptr->update_chart(0);
}
