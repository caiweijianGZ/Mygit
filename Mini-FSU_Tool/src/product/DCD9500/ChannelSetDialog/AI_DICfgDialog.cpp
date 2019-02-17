#include "AI_DICfgDialog.h"
#include "ui_AI_DICfgDialog.h"

AI_DICfgDialog::AI_DICfgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AI_DICfgDialog)
{
    ui->setupUi(this);

    widgetInit();


}

AI_DICfgDialog::~AI_DICfgDialog()
{
    delete ui;
}

int AI_DICfgDialog::loadJsonData2Ui(const QByteArray &original)
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
    QJsonArray jarry = jobj.value(CHN_AI_DI_JSON_VALUE_ARRAY).toArray();
    this->loadJsonData2Ui(jarry);

    return 0;
}

int AI_DICfgDialog::loadJsonData2Ui(const QJsonArray &jsonArry)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    while ( m_AIItemList.count() )
    {
        delete m_AIItemList.takeFirst();
    }

    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);
    while ( m_DIItemList.count() )
    {
        delete m_DIItemList.takeFirst();
    }

    for ( int i=0; i<jsonArry.count(); i++ )
    {
        QJsonObject AI_DIObj = jsonArry.at(i).toObject();
        DIItemWidget * DIItem = new DIItemWidget(AI_DIObj);
        AIItemWidget * AIItem = new AIItemWidget(AI_DIObj);

        this->addDIItemWidget(ui->tableWidget_2, DIItem);
        m_DIItemList.append(DIItem);

        this->addAIItemWidget(ui->tableWidget, AIItem);
        m_AIItemList.append(AIItem);

        if ( SENSOR_TYPE_OPEN == AI_DIObj.value(CHN_JSON_VALUE_SENSOR_TYPE).toInt()
             || SENSOR_TYPE_CLOSE == AI_DIObj.value(CHN_JSON_VALUE_SENSOR_TYPE).toInt() )//常开，常闭
        {
            DIItem->setEnabled(true);
            AIItem->setEnabled(false);

            if ( 0 == i )
            {
                ui->comboBox->setCurrentIndex(1);
            }else{
                ui->comboBox_2->setCurrentIndex(1);
            }

        }
        else
        {
            DIItem->setEnabled(false);
            AIItem->setEnabled(true);

            if ( 0 == i )
            {
                ui->comboBox->setCurrentIndex(0);
            }else{
                ui->comboBox_2->setCurrentIndex(0);
            }
        }
    }

    return 0;
}

int AI_DICfgDialog::AI_DICfgDialog::getJsonData(QJsonArray &jsonArry)
{        
    if ( !m_AIItemList.count() )
    {
        return -1;
    }

    if ( !m_DIItemList.count() )
    {
        return -1;
    }

    QJsonArray AIDIArry;
    bool coverOk = false;
    if ( 0 == ui->comboBox->currentIndex() )
    {
        AIDIArry.append(m_AIItemList.at(0)->toJsonObject(&coverOk));
    }
    else
    {
        AIDIArry.append(m_DIItemList.at(0)->toJsonObject(&coverOk));
    }

    if ( false == coverOk )
    {
        return -1;
    }

    if ( 0 == ui->comboBox_2->currentIndex() )
    {
        AIDIArry.append(m_AIItemList.at(1)->toJsonObject(&coverOk));
    }
    else
    {
        AIDIArry.append(m_DIItemList.at(1)->toJsonObject(&coverOk));
    }

    if ( false == coverOk )
    {
        return -1;
    }

    jsonArry = AIDIArry;
    return 0;
}

int AI_DICfgDialog::widgetInit()
{
    QStringList headerLabels;
    headerLabels << "AI通道备注" << "启用" << "传感器类型" << "最大值" << "最小值" << "设备Code" << "信号ID";
    ui->tableWidget->setColumnCount(headerLabels.count());
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->tableWidget->setColumnWidth(1, 80);//缩小启动列宽

    headerLabels.clear();
    headerLabels << "DI通道备注" << "启用" << "传感器类型" << "设备Code" << "信号ID";
    ui->tableWidget_2->setColumnCount(headerLabels.count());
    ui->tableWidget_2->setHorizontalHeaderLabels(headerLabels);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ch1FuncChangeslot(int)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(ch2FuncChangeslot(int)));


    return 0;
}

int AI_DICfgDialog::addAIItemWidget(QTableWidget *tableWidget, AIItemWidget *itemWidget)
{
    int insertIndex = (0 == tableWidget->rowCount()) ? 0 :  tableWidget->rowCount();
    ui->tableWidget->insertRow(insertIndex);

    int rowIndex = tableWidget->rowCount() - 1;
    tableWidget->setCellWidget(rowIndex, 0, &itemWidget->tag);
    tableWidget->setCellWidget(rowIndex, 1, &itemWidget->active);
    tableWidget->setCellWidget(rowIndex, 2, &itemWidget->sensorType);
    tableWidget->setCellWidget(rowIndex, 3, &itemWidget->maxValue);
    tableWidget->setCellWidget(rowIndex, 4, &itemWidget->minValue);
    tableWidget->setCellWidget(rowIndex, 5, &itemWidget->devCode);
    tableWidget->setCellWidget(rowIndex, 6, &itemWidget->signalIdsBtn);

    return 0;
}

int AI_DICfgDialog::addDIItemWidget(QTableWidget *tableWidget, DIItemWidget *itemWidget)
{

    int insertIndex = (0 == tableWidget->rowCount()) ? 0 :  tableWidget->rowCount();
    tableWidget->insertRow(insertIndex);

    int rowIndex = tableWidget->rowCount() - 1;
    tableWidget->setCellWidget(rowIndex, 0, &itemWidget->tag);
    tableWidget->setCellWidget(rowIndex, 1, &itemWidget->active);
    tableWidget->setCellWidget(rowIndex, 2, &itemWidget->sensorType);
    tableWidget->setCellWidget(rowIndex, 3, &itemWidget->devCode);
    tableWidget->setCellWidget(rowIndex, 4, &itemWidget->signalIdsBtn);

    return 0;
}

void AI_DICfgDialog::ch1FuncChangeslot(int index)
{
    if ( !m_AIItemList.count() )
    {
        return;
    }

    if ( !m_DIItemList.count() )
    {
        return;
    }

    if ( 0 == index )//AI
    {
        m_AIItemList.at(0)->setEnabled(true);
        m_DIItemList.at(0)->setEnabled(false);
    }
    else
    {
        m_AIItemList.at(0)->setEnabled(false);
        m_DIItemList.at(0)->setEnabled(true);
    }
}

void AI_DICfgDialog::ch2FuncChangeslot(int index)
{
    if ( !m_AIItemList.count() )
    {
        return;
    }

    if ( !m_DIItemList.count() )
    {
        return;
    }

    if ( 0 == index )//AI
    {
        m_AIItemList.at(1)->setEnabled(true);
        m_DIItemList.at(1)->setEnabled(false);
    }
    else
    {
        m_AIItemList.at(1)->setEnabled(false);
        m_DIItemList.at(1)->setEnabled(true);
    }
}
