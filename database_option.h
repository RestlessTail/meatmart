#ifndef DATABASE_OPTION_H
#define DATABASE_OPTION_H

#include <QDialog>

namespace Ui {
class database_option;
}

class database_option : public QDialog
{
    Q_OBJECT

public:
    explicit database_option(QWidget *parent = nullptr);
    ~database_option();
    Ui::database_option *ui;

private:
};

#endif // DATABASE_OPTION_H
