#ifndef UPDATESERVER_H
#define UPDATESERVER_H

#include "cloud_service.h"

class QNetworkReply;
class clerk_management;

namespace Server {
    enum ServerAction{
        MODIFY,
        ADD,
        REMOVE
    };
}


class updateServer: public cloud_service{

    Q_OBJECT

public:

    int updateIndex;

    clerk_management* parent_to_modify;

    Server::ServerAction action;

    updateServer(MainWindow* parent, int photographerNum, Server::ServerAction serverAction);

    updateServer(MainWindow* parent_1, clerk_management* parent_2, int photographerNum, Server::ServerAction serverAction);

    void exec_cloud_service() override;

    void modify();

    void add();

    void remove();

private slots:

    void is_successful(QNetworkReply* reply);
};

#endif // UPDATESERVER_H
