#ifndef MANAGEDIALOG_H
#define MANAGEDIALOG_H

#include <QDialog>

namespace Ui {
class ManageDialog;
}

class ManageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManageDialog(QWidget *parent = 0);
    ~ManageDialog();

private:
    Ui::ManageDialog *ui;
};

#endif // MANAGEDIALOG_H
