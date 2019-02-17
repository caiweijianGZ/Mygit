#ifndef SIGNALIDSEDITDIALOG_H
#define SIGNALIDSEDITDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QTableWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QScrollBar>
#include <QDebug>

#include "../channel_type.h"

namespace Ui {
class SignalIdsEditDialog;
}

class SignalIdsEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignalIdsEditDialog(QWidget *parent = 0);
    SignalIdsEditDialog(const QByteArray &src, QWidget *parent = 0);
    SignalIdsEditDialog(const QString &ditPath, QWidget *parent = 0);
    SignalIdsEditDialog(const char *ditPath, QWidget *parent = 0);
    ~SignalIdsEditDialog();

    int setSignalList(const QList<uint64_t> &list);
    int getSignalList(QList<uint64_t> &list);


protected:
    int addDevType2ListWidget(QListWidget *widget);
    int addSigInfo2TableWidget(int devType, QTableWidget *widget);
    int appendSigInfo2Table(const QJsonObject &obj, QTableWidget *tableWidget);
    int sigSelectedListDisaplay(const QList<uint64_t> &list);
    int addSigInfo2SelectedList(int row);
    int removeRowAtSelectedList(int row);


protected:
    QByteArray m_dictCache;
    QMap<int, int> m_mapRow2DevType;
    QList<uint64_t> m_sigSelectedList;

private slots:
    void devTypeChangeSlot(int rowIndex);
    void sigSelectSlot(QModelIndex index);
    void sigRemoveSlot(QModelIndex index);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::SignalIdsEditDialog *ui;
};

#endif // SIGNALIDSEDITDIALOG_H
