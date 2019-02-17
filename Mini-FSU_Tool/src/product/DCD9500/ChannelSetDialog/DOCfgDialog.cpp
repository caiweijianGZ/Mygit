#include "DOCfgDialog.h"
#include "ui_DOCfgDialog.h"

DOCfgDialog::DOCfgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DOCfgDialog)
{
    ui->setupUi(this);

    QStringList headLabels;
    headLabels << "DO通道备注" << "启动" << "设备Code" << "信号ID";
    ui->tableWidget->setColumnCount(headLabels.count());
    ui->tableWidget->setHorizontalHeaderLabels(headLabels);

    /*
    this->addDOItemWidget(ui->tableWidget, new DOItemWidget());
    this->addDOItemWidget(ui->tableWidget, new DOItemWidget());
    */
}

DOCfgDialog::~DOCfgDialog()
{
    delete ui;
}

int DOCfgDialog::loadJsonData2Ui(const QByteArray &original)
{
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(original, &jerror);

    if ( jerror.error != QJsonParseError::NoError )
    {
        qDebug() << "数据格式错误1-1";
        return -1;
    }

    if ( !jdoc.isObject() )
    {
        qDebug() << "数据格式错误1-2";
        return -1;
    }

    QJsonObject jobj = jdoc.object();
    QJsonArray jarry = jobj.value(CHN_DO_JSON_VALUE_ARRAY).toArray();
    this->loadJsonData2Ui(jarry);

    return 0;
}

int DOCfgDialog::loadJsonData2Ui(const QJsonArray &jsonArry)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    while ( m_DOItemList.count() )
    {
        delete m_DOItemList.takeFirst();
    }

    for ( int i=0; i<jsonArry.count(); i++ )
    {
        QJsonObject DOObj = jsonArry.at(i).toObject();
        DOItemWidget * DOItem = new DOItemWidget(DOObj);

        this->addDOItemWidget(ui->tableWidget, DOItem);
        m_DOItemList.append(DOItem);
    }

    return 0;
}

int DOCfgDialog::getJsonData(QJsonArray &jsonArry)
{
    if ( !m_DOItemList.count() )
    {
        return -1;
    }

    QJsonArray DOArry;
    bool coverOk = false;

    for ( int i=0; i<m_DOItemList.count(); i++ )
    {
        DOArry.append(m_DOItemList.at(i)->toJsonObject(&coverOk));
        if ( false == coverOk )
        {
            return -1;
        }
    }

    jsonArry = DOArry;
    return 0;
}

int DOCfgDialog::addDOItemWidget(QTableWidget *tableWidget, DOItemWidget *itemWidget)
{
    int insertIndex = (0 == tableWidget->rowCount()) ? 0 :  tableWidget->rowCount();
    tableWidget->insertRow(insertIndex);

    int rowIndex = tableWidget->rowCount() - 1;
    tableWidget->setCellWidget(rowIndex, 0, &itemWidget->tag);
    tableWidget->setCellWidget(rowIndex, 1, &itemWidget->active);
    tableWidget->setCellWidget(rowIndex, 2, &itemWidget->devCode);
    tableWidget->setCellWidget(rowIndex, 3, &itemWidget->signalIdsBtn);

    return 0;
}
