#ifndef RESERVATION_H
#define RESERVATION_H

#include "cloud_service.h"

class QNetworkReply;

class Reservations : public cloud_service{

    Q_OBJECT

public:

    int num;

    Reservations(MainWindow* parent);

    void exec_cloud_service() override;

    void analyse_collection_data(QString raw_data);

    void get_reservations();

    void update_data(QString raw_data);

public slots:
    void download_collection_data(QNetworkReply* reply);

    void download_reservation_data(QNetworkReply* reply);

};

#endif // RESERVATION_H
