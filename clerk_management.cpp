#include "clerk_management.h"
#include "ui_clerk_management.h"
#include "modify_info.h"
#include "ui_modify_info.h"
#include "updateserver.h"
#include "gettimetable.h"
#include "mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>

clerk_management::clerk_management(MainWindow *parent_window, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::clerk_management)
{
    ui->setupUi(this);
    parent_ptr = parent_window;

    updateList();
}

clerk_management::~clerk_management()
{
    delete ui;
}

void clerk_management::updateList()
{
    int currentNum = parent_ptr->photographer.size();
    ui->clerks->blockSignals(true);
    ui->clerks->clear();
    if(currentNum > 0){
        for(int i = 0; i < currentNum; i++){
            ui->clerks->addItem(parent_ptr->photographer[i].name);
        }
    }
    ui->clerks->blockSignals(false);
}

void clerk_management::on_clerks_currentRowChanged(int currentRow)
{
    ui->clerk_id->setText(QString::number(parent_ptr->photographer[currentRow].id));
    ui->clerk_tel->setText(parent_ptr->photographer[currentRow].tel);
    ui->clerk_name->setText(parent_ptr->photographer[currentRow].name);
}

/*void clerk_management::on_modify_id_clicked()
{
    auto is_id_valid = [](int id, QList<profile> list)->bool{
        for(QList<profile>::iterator i = list.begin(); i != list.end(); ++i){
            if(id == i->id){
                return false;
            }
        }
        return true;
    };

    QString title = "修改";
    QString prompt = "请输入新ID";
    QString defaultText = ui->clerk_id->text();

    bool isOK=false;
    QString modified_data = QInputDialog::getText(this, title, prompt, QLineEdit::Normal, defaultText, &isOK);
    if (isOK && !modified_data.isEmpty()){
        if(is_id_valid(modified_data.toInt(), temp_clerk_list)){
            ui->clerk_id->setText(modified_data);

            int currentRow = ui->clerks->currentRow();
            temp_clerk_list[currentRow].id = modified_data.toInt();
        }
        else{
            QString title="警告";
            QString info="请勿设置重复ID";
            QMessageBox::critical(this, title, info);
        }
    }
}

void clerk_management::on_modify_name_clicked()
{
    QString title = "修改";
    QString prompt = "请输入新姓名";
    QString defaultText = ui->clerk_name->text();

    bool isOK=false;
    QString modified_data = QInputDialog::getText(this, title, prompt, QLineEdit::Normal, defaultText, &isOK);
    if (isOK && !modified_data.isEmpty()){
        ui->clerk_name->setText(modified_data);

        int currentRow = ui->clerks->currentRow();
        temp_clerk_list[currentRow].name = modified_data;
    }
}

void clerk_management::on_modify_tel_clicked()
{
    QString title = "修改";
    QString prompt = "请输入新联系方式";
    QString defaultText = ui->clerk_tel->text();

    bool isOK=false;
    QString modified_data = QInputDialog::getText(this, title, prompt, QLineEdit::Normal, defaultText, &isOK);
    if (isOK && !modified_data.isEmpty()){
        ui->clerk_tel->setText(modified_data);

        int currentRow = ui->clerks->currentRow();
        temp_clerk_list[currentRow].tel = modified_data;
    }
}*/

void clerk_management::on_remove_clerk_clicked()
{
    int currentIndex = ui->clerks->currentRow();
    parent_ptr->service_category = new updateServer(parent_ptr, this, currentIndex, Server::REMOVE);
    parent_ptr->get_access_token();
}

void clerk_management::on_add_clerk_clicked()
{
    profile new_clerk;
    modify_info dialog(this);
    if(!dialog.exec()){
        return;
    }
    new_clerk.id = dialog.ui->id->text().toInt();
    new_clerk.name = dialog.ui->name->text();
    new_clerk.tel = dialog.ui->tel->text();
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 7; j++){
            new_clerk.schedule[i][j] = true;
        }
    }

    parent_ptr->photographer.append(new_clerk);

    parent_ptr->service_category = new updateServer(parent_ptr, this, parent_ptr->photographer.size() - 1, Server::ADD);
    parent_ptr->get_access_token();

    /*parent_ptr->service_category = new getTimetable(parent_ptr);
    parent_ptr->get_access_token();*/
}

void clerk_management::on_modify_clicked()
{
    int currentIndex = ui->clerks->currentRow();
    if(currentIndex == -1){
        return;
    }
    modify_info dialog(this, currentIndex);
    if(!dialog.exec()){
        return;
    }

    auto is_id_valid = [&](int id, QList<profile> list)->bool{
        int k = 0;
        for(QList<profile>::iterator i = list.begin(); i != list.end(); ++i){
            if(id == i->id && k != currentIndex){
                return false;
            }
            k++;
        }
        return true;
    };

    int id;
    QString name;
    QString tel;
    bool is_modified = false;

    id = dialog.ui->id->text().toInt();
    if(!is_id_valid(id, parent_ptr->photographer)){
        QString title="警告";
        QString info="请勿设置重复ID";
        QMessageBox::critical(this, title, info);
        return;
    }

    name = dialog.ui->name->text();
    tel = dialog.ui->tel->text();

    if(parent_ptr->photographer[currentIndex].id != id){
        is_modified = true;
        parent_ptr->photographer[currentIndex].id = id;
    }
    if(parent_ptr->photographer[currentIndex].name != name){
        is_modified = true;
        parent_ptr->photographer[currentIndex].name = name;
    }
    if(parent_ptr->photographer[currentIndex].tel != tel){
        is_modified = true;
        parent_ptr->photographer[currentIndex].tel = tel;
    }

    if(is_modified){
        parent_ptr->service_category = new updateServer(parent_ptr, this, currentIndex, Server::MODIFY);
        parent_ptr->get_access_token();
        updateList();
        parent_ptr->update_combobox();
        ui->clerk_id->clear();
        ui->clerk_name->clear();
        ui->clerk_tel->clear();
    }
}
