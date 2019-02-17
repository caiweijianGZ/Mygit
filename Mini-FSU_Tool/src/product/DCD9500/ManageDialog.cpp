#include "ManageDialog.h"
#include "ui_ManageDialog.h"

ManageDialog::ManageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageDialog)
{
    ui->setupUi(this);
}

ManageDialog::~ManageDialog()
{
    delete ui;
}
