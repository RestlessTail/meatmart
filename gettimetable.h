#ifndef GETTIMETABLE_H
#define GETTIMETABLE_H

#include "cloud_service.h"

class QNetworkReply;

class getTimetable: public cloud_service{

    Q_OBJECT

public:



    getTimetable(MainWindow* parent);

    void exec_cloud_service() override;

    void get_timetables();

    void get_data(QString raw_data);

    void update_data();


public slots:

    void download_collection_data(QNetworkReply*);

    void download_schedule_data(QNetworkReply* reply);
};

#endif // GETTIMETABLE_H
