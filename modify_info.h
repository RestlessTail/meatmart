#ifndef MODIFY_INFO_H
#define MODIFY_INFO_H

#include <QDialog>

class clerk_management;

namespace Ui {
class modify_info;
}

class modify_info : public QDialog
{
    Q_OBJECT

public:
    explicit modify_info(clerk_management* parent_window, int current, QWidget *parent = nullptr);

    explicit modify_info(clerk_management* parent_window, QWidget *parent = nullptr);

    ~modify_info();

    Ui::modify_info *ui;

    int currentIndex;

    void enter_default_info();

    clerk_management* parent_ptr;

private:

};

#endif // MODIFY_INFO_H
