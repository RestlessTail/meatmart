#include "database_option.h"
#include "ui_database_option.h"

database_option::database_option(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::database_option)
{
    ui->setupUi(this);
}

database_option::~database_option()
{
    delete ui;
}
