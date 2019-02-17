#include "AICfgDialog.h"
#include "ui_AICfgDialog.h"
#include <QHBoxLayout>

AICfgDialog::AICfgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AICfgDialog)
{
    ui->setupUi(this);  

    QStringList headerLabels;
    headerLabels << "AI通道" << "激活" << "类型" << "最大值" << "最小值" << "设备ID" << "信号ID";
    ui->tableWidget->setColumnCount(headerLabels.count());
    //ui->tableWidget->setRowCount(1);
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    AICfgItemWidget_t *itemWidget = new AICfgItemWidget_t;
    this->addItemWidget(itemWidget);
    this->addItemWidget(new AICfgItemWidget_t);
}

AICfgDialog::~AICfgDialog()
{
    delete ui;
}

int AICfgDialog::AICfgWidget2Item(const AICfgItemWidget_t &itemWidget, AICfgItem_t &item)
{
    return 0;
}

int AICfgDialog::AICfgItem2Widget(const AICfgItem_t &item, AICfgItemWidget_t &itemWidget)
{
    return 0;
}

int AICfgDialog::addItemWidget(AICfgItemWidget_t *itemWidget)
{
    int insertIndex = (0 == ui->tableWidget->rowCount()) ? 0 :  ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(insertIndex);

    int rowIndex = ui->tableWidget->rowCount() - 1;
    ui->tableWidget->setCellWidget(rowIndex, 0, &itemWidget->tag);
    ui->tableWidget->setCellWidget(rowIndex, 1, &itemWidget->active);
    ui->tableWidget->setCellWidget(rowIndex, 2, &itemWidget->sensorType);
    ui->tableWidget->setCellWidget(rowIndex, 3, &itemWidget->maxValue);
    ui->tableWidget->setCellWidget(rowIndex, 4, &itemWidget->minValue);
    ui->tableWidget->setCellWidget(rowIndex, 5, &itemWidget->devCode);
    ui->tableWidget->setCellWidget(rowIndex, 6, &itemWidget->signalIdsBtn);

    return 0;
}


