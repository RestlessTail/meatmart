#include "modification.h"
#include "ui_modification.h"

modification::modification(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modification)
{
    ui->setupUi(this);
}

modification::~modification()
{
    delete ui;
}

bool modification::getValue()
{
    if(ui->avaliable->isChecked()){
        return true;
    }
    else{
        return false;
    }
}
