#ifndef DEVSETDIALOG_H
#define DEVSETDIALOG_H

#include <QDialog>

namespace Ui {
class listStackedDialog;
}

class listStackedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit listStackedDialog(QWidget *parent = 0);
    ~listStackedDialog();
    void addWidget(QString name, QWidget *widget);

private:
    Ui::listStackedDialog *ui;
};

#endif // DEVSETDIALOG_H
