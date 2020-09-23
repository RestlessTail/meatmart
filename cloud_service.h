#ifndef CLOUD_SERVICE_H
#define CLOUD_SERVICE_H

#include <QObject>

class MainWindow;

class cloud_service : public QObject{
public:

    MainWindow* parent_ptr;

    virtual void exec_cloud_service() = 0;//automatically called by return_access_token

};

#endif // CLOUD_SERVICE_H
