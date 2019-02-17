#include "ChnItemWidget.h"

ChnItemWidget::ChnItemWidget(QWidget *parent) : QWidget(parent)
{
    id = 0;
    tag.setPlaceholderText("不超过10个字符");
    active.addItems(QStringList()
                    << mapChannelActive.value(CHANNEL_ENABLE)
                    <<  mapChannelActive.value(CHANNEL_DISABLE));
    signalIdsBtn.setText("编辑");
    connect(&signalIdsBtn, SIGNAL(clicked(bool)), this, SLOT(signalIdsEditSlot()));
}

ChnItemWidget::~ChnItemWidget()
{

}

void ChnItemWidget::setEnabled(bool enable)
{
    QWidget::setEnabled(enable);

    tag.setEnabled(enable);
    active.setEnabled(enable);
    devCode.setEnabled(enable);
    signalIdsBtn.setEnabled(enable);
}

void ChnItemWidget::signalIdsEditSlot()
{
    QString strDictPath;
    QFile dictFile(SIGNAL_DICT_PATH);
    if ( !dictFile.exists() )
    {
        strDictPath = ":/dict/signal_dic.json";
    }
    else
    {
        strDictPath = SIGNAL_DICT_PATH;
    }

    SignalIdsEditDialog signalDialog(strDictPath);
    signalDialog.setSignalList(signalIdList);

    if ( SignalIdsEditDialog::Accepted == signalDialog.exec() )
    {
        signalDialog.getSignalList(signalIdList);
    }
}
