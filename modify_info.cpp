#include "modify_info.h"
#include "ui_modify_info.h"
#include "clerk_management.h"
#include "ui_clerk_management.h"

modify_info::modify_info(clerk_management *parent_window, int current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modify_info)
{
    ui->setupUi(this);
    parent_ptr = parent_window;
    currentIndex = current;
    enter_default_info();
}

modify_info::modify_info(clerk_management *parent_window, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modify_info)
{
    ui->setupUi(this);
    parent_ptr = parent_window;
}

modify_info::~modify_info()
{
    delete ui;
}

void modify_info::enter_default_info()
{
    ui->id->setText(QString::number(parent_ptr->parent_ptr->photographer[currentIndex].id));
    ui->name->setText(parent_ptr->parent_ptr->photographer[currentIndex].name);
    ui->tel->setText(parent_ptr->parent_ptr->photographer[currentIndex].tel);
}
