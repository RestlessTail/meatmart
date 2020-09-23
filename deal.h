#ifndef DEAL_H
#define DEAL_H

#include "cloud_service.h"

class QNetworkReply;

class Deal: public cloud_service{

    Q_OBJECT

public:

    Deal(MainWindow* parent);

    void exec_cloud_service() override;

public slots:

    void is_successful(QNetworkReply*);

};
#endif // DEAL_H
