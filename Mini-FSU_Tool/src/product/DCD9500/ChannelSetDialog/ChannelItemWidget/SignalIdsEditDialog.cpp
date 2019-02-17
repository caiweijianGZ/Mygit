#include "SignalIdsEditDialog.h"
#include "ui_SignalIdsEditDialog.h"

SignalIdsEditDialog::SignalIdsEditDialog(QWidget *parent) :
    SignalIdsEditDialog(QByteArray() ,parent)
{

}

SignalIdsEditDialog::SignalIdsEditDialog(const QByteArray &src, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalIdsEditDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("信号选择");
    QStringList headLabels;
    //headLabels << "信号标准名" << "信号类型" << "告警时" << "正常时" << "信号说明" << "信号注释" << "信号ID";
    headLabels << "信号标准名" << "信号类型" << "告警时" << "正常时" << "信号ID";
    ui->tableWidget->setColumnCount(headLabels.count());
    ui->tableWidget->setHorizontalHeaderLabels(headLabels);
    ui->tableWidget->setColumnHidden(headLabels.count()-1, true);//隐藏信号ID

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选行模式
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//复选模式

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(devTypeChangeSlot(int)));
    connect(ui->listWidget_2, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sigRemoveSlot(QModelIndex)));
    connect(ui->tableWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sigSelectSlot(QModelIndex)));


    if ( !src.isEmpty() )
    {
        m_dictCache = src;
        this->addDevType2ListWidget(ui->listWidget);
    }
}

SignalIdsEditDialog::SignalIdsEditDialog(const QString &ditPath, QWidget *parent)
    : SignalIdsEditDialog(QByteArray(), parent)
{
    QFile file(ditPath);
    if ( file.open(QIODevice::ReadOnly) )
    {
        m_dictCache = file.readAll();
        file.close();
        this->addDevType2ListWidget(ui->listWidget);
    }
}

SignalIdsEditDialog::SignalIdsEditDialog(const char *ditPath, QWidget *parent)
    : SignalIdsEditDialog(QString(ditPath), parent)
{

}

SignalIdsEditDialog::~SignalIdsEditDialog()
{
    delete ui;
}

int SignalIdsEditDialog::setSignalList(const QList<uint64_t> &list)
{
    this->sigSelectedListDisaplay(list);
    m_sigSelectedList = list;

    return 0;
}

int SignalIdsEditDialog::getSignalList(QList<uint64_t> &list)
{
    list = m_sigSelectedList;

    return 0;
}

int SignalIdsEditDialog::addDevType2ListWidget(QListWidget *widget)
{
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(m_dictCache, &jerror);

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
    QJsonArray jarry = jobj.value(DICT_JSON_VALUE_DEVICES).toArray();

    for ( int i=0; i<jarry.count(); i++ )
    {
        jobj = jarry.at(i).toObject();
        QString itemName = jobj.value(DICT_JSON_VALUE_NAME).toString();
        widget->addItem(itemName);
        m_mapRow2DevType.insert(i, jobj.value(DICT_JSON_VALUE_TYPE).toInt());
    }

    return 0;
}

int SignalIdsEditDialog::addSigInfo2TableWidget(int devType, QTableWidget *widget)
{
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(m_dictCache, &jerror);

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
    QJsonArray devArry = jobj.value(DICT_JSON_VALUE_DEVICES).toArray();

    for ( int i=0; i<devArry.count(); i++ )
    {
        QJsonObject devObj = devArry.at(i).toObject();

        if ( devObj.value(DICT_JSON_VALUE_TYPE).toInt() == devType )
        {
            QJsonArray sigArry = devObj.value(DICT_JSON_VALUE_SIGNALS).toArray();
            for ( int j=0; j<sigArry.count(); j++ )
            {
                this->appendSigInfo2Table(sigArry.at(j).toObject(), widget);
            }

        }
    }

    return 0;
}

int SignalIdsEditDialog::appendSigInfo2Table(const QJsonObject &obj, QTableWidget *tableWidget)
{
    int insertIndex = (0 == tableWidget->rowCount()) ? 0 :  tableWidget->rowCount();
    tableWidget->insertRow(insertIndex);

    int rowIndex = tableWidget->rowCount() - 1;


    for ( int i=0; i<tableWidget->columnCount(); i++ )
    {
        tableWidget->setItem(rowIndex, i, new QTableWidgetItem());
    }

    ui->tableWidget->item(rowIndex, 0)->setText(obj.value(DICT_JSON_VALUE_SIGNAME).toString());
    ui->tableWidget->item(rowIndex, 1)->setText(mapChnSigTypes.value(obj.value(DICT_JSON_VALUE_TYPE).toInt()));
    ui->tableWidget->item(rowIndex, 2)->setText(obj.value(DICT_JSON_VALUE_ALMMSG).toString());
    ui->tableWidget->item(rowIndex, 3)->setText(obj.value(DICT_JSON_VALUE_NORMSG).toString());
    //ui->tableWidget->item(rowIndex, 4)->setText("");
    //ui->tableWidget->item(rowIndex, 5)->setText("");
    ui->tableWidget->item(rowIndex, 4)->setText(QString::number(obj.value(DICT_JSON_VALUE_ID).toInt()));

    return 0;
}

int SignalIdsEditDialog::sigSelectedListDisaplay(const QList<uint64_t> &list)
{
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(m_dictCache, &jerror);

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
    QJsonArray devArry = jobj.value(DICT_JSON_VALUE_DEVICES).toArray();

    for ( int i=0; i<devArry.count(); i++ )
    {
        QJsonObject devObj = devArry.at(i).toObject();
        QJsonArray sigArry = devObj.value(DICT_JSON_VALUE_SIGNALS).toArray();

        for ( int j=0; j<sigArry.count(); j++ )
        {
            QJsonObject sigObj = sigArry.at(j).toObject();
            uint64_t sigId = sigObj.value(DICT_JSON_VALUE_ID).toInt();
            QString sigName = sigObj.value(DICT_JSON_VALUE_SIGNAME).toString();

            if ( list.contains(sigId) )
            {
                ui->listWidget_2->addItem(sigName);
            }
        }
    }

    return 0;
}

int SignalIdsEditDialog::addSigInfo2SelectedList(int row)
{
    QString sigName = ui->tableWidget->item(row, 0)->text();
    uint64_t sigId = ui->tableWidget->item(row, ui->tableWidget->columnCount()-1)->text().toULongLong();

    if ( m_sigSelectedList.contains(sigId) )
    {
        return -1;
    }

    int listRow = ui->listWidget_2->count();
    ui->listWidget_2->insertItem(listRow, sigName);
    m_sigSelectedList.append(sigId);

    return 0;
}

int SignalIdsEditDialog::removeRowAtSelectedList(int row)
{
    m_sigSelectedList.removeAt(row);
    QListWidgetItem *item = ui->listWidget_2->takeItem(row);
    delete item;

    return 0;
}

void SignalIdsEditDialog::devTypeChangeSlot(int rowIndex)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    this->addSigInfo2TableWidget(m_mapRow2DevType.value(rowIndex), ui->tableWidget);
    ui->label_2->setText(ui->listWidget->currentItem()->text());
}

void SignalIdsEditDialog::sigSelectSlot(QModelIndex index)
{
    this->addSigInfo2SelectedList(index.row());
}

void SignalIdsEditDialog::sigRemoveSlot(QModelIndex index)
{
    this->removeRowAtSelectedList(index.row());
}

void SignalIdsEditDialog::on_pushButton_2_clicked()
{
    this->close();
}

void SignalIdsEditDialog::on_pushButton_clicked()
{
    this->accept();
}

void SignalIdsEditDialog::on_pushButton_3_clicked()
{
    static int s_lastSearchLine = 0;
    static QString s_lastSearchText;
    QString searchText = ui->lineEdit->text();

    if ( searchText.isEmpty() )
    {
        return;
    }

    int rowCount = ui->tableWidget->rowCount();
    int rowStartIndex = 0;
    //从上一次查找行的下一行开始找
    if ( s_lastSearchText == searchText )
    {
        rowStartIndex = (s_lastSearchLine == ui->tableWidget->currentRow()) ? s_lastSearchLine + 1 : 0;
    }

    for ( int i=0,j=rowStartIndex; i<rowCount; i++,j++ )
    {        
        j = ( j < rowCount ) ? j : (rowCount - j);

        if ( ui->tableWidget->item(j, 0)->text().contains(searchText) )
        {
            ui->tableWidget->item(j, 0)->setSelected(true);
            ui->tableWidget->selectRow(j);
            s_lastSearchLine = j;//保存上次搜索结果
            s_lastSearchText = searchText;

            break;
        }
    }
}

void SignalIdsEditDialog::on_pushButton_4_clicked()
{
    if ( ui->tableWidget->selectedItems().count() <= 0 )
    {
        return;
    }

    this->addSigInfo2SelectedList(ui->tableWidget->currentRow());
}

void SignalIdsEditDialog::on_pushButton_5_clicked()
{
    if ( ui->listWidget_2->selectedItems().count() <= 0 )
    {
        return;
    }

    this->removeRowAtSelectedList(ui->listWidget_2->currentRow());
}
