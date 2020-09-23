#ifndef CLERK_MANAGEMENT_H
#define CLERK_MANAGEMENT_H

#include <QDialog>
#include <mainwindow.h>

class MainWindow;

namespace Ui {
class clerk_management;
}

class clerk_management : public QDialog
{
    Q_OBJECT

public:
    explicit clerk_management(MainWindow* parent_window, QWidget *parent = nullptr);
    ~clerk_management();

    MainWindow* parent_ptr;

    void updateList();

private slots:


    void on_clerks_currentRowChanged(int currentRow);

    void on_remove_clerk_clicked();

    void on_add_clerk_clicked();

    void on_modify_clicked();

public:
    Ui::clerk_management *ui;
};

#endif // CLERK_MANAGEMENT_H
