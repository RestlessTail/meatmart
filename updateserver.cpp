#include "updateserver.h"
#include "mainwindow.h"
#include "gettimetable.h"
#include "clerk_management.h"
#include "ui_clerk_management.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

updateServer::updateServer(MainWindow *parent, int photographerNum, Server::ServerAction serverAction)
{
    parent_ptr = parent;
    updateIndex = photographerNum;
    action = serverAction;
}

updateServer::updateServer(MainWindow *parent_1, clerk_management *parent_2, int photographerNum, Server::ServerAction serverAction)
{
    parent_ptr = parent_1;
    parent_to_modify = parent_2;
    updateIndex = photographerNum;
    action = serverAction;
}

void updateServer::exec_cloud_service()
{
    switch (action) {
        case Server::MODIFY: modify();break;
        case Server::ADD: add();break;
        case Server::REMOVE: remove();break;
    }
}

void updateServer::modify()
{
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    QString query;

    QString name = parent_ptr->photographer[updateIndex].name;
    QString _id = parent_ptr->photographer[updateIndex]._id;
    QString id = QString::number(parent_ptr->photographer[updateIndex].id);
    QString tel = parent_ptr->photographer[updateIndex].tel;
    QString timetable;
    QJsonArray Array;
    for(int i = 0; i < 3; i++){
        QJsonArray array;
        for(int j = 0; j < 7; j++){

            array.append(QJsonValue(parent_ptr->photographer[updateIndex].schedule[i][j]));
        }
        Array.append(QJsonValue(array));
    }
    QJsonDocument doc = QJsonDocument(Array);
    timetable = QString(doc.toJson());
    query = "db.collection(\"photographerSchedule\").doc(\"" + _id + "\").update({data:{" +
            "timetable:" + timetable + "," +
            "_id:\"" + _id + "\"," +
            "id:" + id + "," +
            "tel:\"" + tel + "\"," +
            "name:\"" + name + "\"," +
            "}})";

    form.insert("query", query);
    QJsonDocument document;
    document.setObject(form);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(parent_ptr);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(is_successful(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/tcb/databaseupdate?access_token=" + parent_ptr->access_token));

    QString testData = (QJsonDocument(form).toJson(QJsonDocument::Compact));
    QNetworkReply* reply = naManager->post(request, data);
}

void updateServer::add()
{
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    QString query;

    QString id = QString::number(parent_ptr->photographer[updateIndex].id);
    QString name = parent_ptr->photographer[updateIndex].name;
    QString tel = parent_ptr->photographer[updateIndex].tel;
    QString timetable;
    QJsonArray Array;
    for(int i = 0; i < 3; i++){
        QJsonArray array;
        for(int j = 0; j < 7; j++){

            array.append(QJsonValue(parent_ptr->photographer[updateIndex].schedule[i][j]));
        }
        Array.append(QJsonValue(array));
    }
    QJsonDocument doc = QJsonDocument(Array);
    timetable = QString(doc.toJson());

    query = "db.collection(\"photographerSchedule\").add({"
            "   data:["
            "       {"
            "           id: " + id + ","
            "           name: \"" + name + "\","
            "           tel: \"" + tel + "\","
            "           timetable: " + timetable +
            "       }"
            "   ]"
            "})";

    form.insert("query", query);
    QJsonDocument document;
    document.setObject(form);
    QByteArray data = document.toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(parent_ptr);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(is_successful(QNetworkReply*)));
    Q_ASSERT(connRet);

    request.setUrl(QUrl("https://api.weixin.qq.com/tcb/databaseadd?access_token=" + parent_ptr->access_token));

    QString testData = (QJsonDocument(form).toJson(QJsonDocument::Compact));
    QNetworkReply* reply = naManager->post(request, data);
}

void updateServer::remove()
{
    QJsonObject form;
    //form.insert("access_token", access_token);
    form.insert("env", parent_ptr->env);
    QString query;

    QString _id = parent_ptr->photographer[updateIndex]._id;
    query = "db.collection(\"photographerSchedule\").doc(\"" + _id + "\").remove()";

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

void updateServer::is_successful(QNetworkReply *reply)
{
    QString data = reply->readAll();
    QJsonDocument doc;
    QJsonObject obj;
    int errcode;
    doc = QJsonDocument::fromJson(data.toLocal8Bit().data());
    obj = doc.object();
    errcode = obj.find("errcode")->toInt();
    if(errcode != 0){
        parent_ptr->log("同步失败");
        parent_ptr->log(data);
    }
    else{
        parent_ptr->log("同步成功");
    }

    if(action == Server::REMOVE){
        parent_to_modify->blockSignals(true);
        delete parent_to_modify->ui->clerks->takeItem(updateIndex);
        parent_to_modify->blockSignals(false);
        parent_ptr->service_category = new getTimetable(parent_ptr);
        parent_ptr->get_access_token();
    }
    else if(action == Server::ADD){
        parent_ptr->update_combobox();
        parent_to_modify->updateList();//FIXME: 这里不知道怎么搞的一直崩溃，要开调试看一下
    }
    else if(action == Server::MODIFY){

    }

    delete this;
}
