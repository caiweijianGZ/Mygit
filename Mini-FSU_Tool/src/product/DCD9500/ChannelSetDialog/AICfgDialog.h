#ifndef AICFGDIALOG_H
#define AICFGDIALOG_H

#include <QDialog>
#include "commonWigets/absSetDialog.h"
#include "channel_type.h"


namespace Ui {
class AICfgDialog;
}

class AICfgDialog : public QDialog, public absSetDialog
{
    Q_OBJECT

public:
    explicit AICfgDialog(QWidget *parent = 0);
    ~AICfgDialog();

protected:
    int AICfgWidget2Item(const AICfgItemWidget_t &itemWidget, AICfgItem_t &item);
    int AICfgItem2Widget(const AICfgItem_t &item, AICfgItemWidget_t &itemWidget);

    int addItemWidget(AICfgItemWidget_t *itemWidget);


protected:
    QList<AICfgItemWidget*> m_AIItemWidgetList;
    QList<AICfgItem*> m_AIItemList;

private slots:


private:
    Ui::AICfgDialog *ui;
};

#endif // AICFGDIALOG_H
