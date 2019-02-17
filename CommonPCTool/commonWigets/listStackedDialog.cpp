#include "listStackedDialog.h"
#include "ui_listStackedDialog.h"

listStackedDialog::listStackedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::listStackedDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)),
            ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ui->listWidget, SIGNAL(currentTextChanged(QString)),
            ui->label, SLOT(setText(QString)));
}

listStackedDialog::~listStackedDialog()
{
    delete ui;
}

void listStackedDialog::addWidget(QString name, QWidget *widget)
{
    ui->listWidget->addItem(name);
    ui->stackedWidget->addWidget(widget);
}
