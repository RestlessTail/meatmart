#ifndef MODIFICATION_H
#define MODIFICATION_H

#include <QDialog>

namespace Ui {
class modification;
}

class modification : public QDialog
{
    Q_OBJECT

public:
    explicit modification(QWidget *parent = nullptr);
    ~modification();

    bool getValue();

private:
    Ui::modification *ui;
};

#endif // MODIFICATION_H
