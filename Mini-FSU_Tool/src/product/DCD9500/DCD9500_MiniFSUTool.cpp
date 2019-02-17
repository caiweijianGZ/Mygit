#include "DCD9500_MiniFSUTool.h"

DCD9500_MiniFSUTool::DCD9500_MiniFSUTool()
{
    this->setWindowTitle(tr("%1").arg(DCD9500_APP_NAME));
}

void DCD9500_MiniFSUTool::actionAbout()
{
    QMessageBox::information(this, tr("关于软件"),
                                   tr("用于获取和配置Mini-FSU的相关属性，固件升级等功能。\n"
                                      "版本号：%1").arg(DCD9500_APP_VERSON),
                                   QMessageBox::Ok);
}

int DCD9500_MiniFSUTool::funcInit()
{
    QTabWidget *pDevTestDialog = new QTabWidget;
    pDevTestDialog->addTab(new AITestDialog(m_pDevTranrecv), "AI通道测试");
    pDevTestDialog->addTab(new DITestDialog(m_pDevTranrecv), "DI通道测试");
    pDevTestDialog->addTab(new UartTestDialog(m_pDevTranrecv), "串口测试");
    pDevTestDialog->addTab(new NBIotTestDialog(m_pDevTranrecv), "NB模块测试");

    QTabWidget *pDevSetDialog = new QTabWidget;
    pDevSetDialog->addTab(new OtherSetDialog(m_pDevTranrecv), "系统配置");
    pDevSetDialog->addTab(new ChannelSetDialog(m_pDevTranrecv), "通道配置");
    pDevSetDialog->addTab(new SerialSetDialog(m_pDevTranrecv), "串口配置");
    pDevSetDialog->addTab(new CalArgsSetDialog(m_pDevTranrecv), "电表参数配置");

    QTabWidget *pDevManage = new QTabWidget;
    //pDevManage->addTab(new WorkStatusDialog(m_pDevTranrecv), "设备工作状态");
    pDevManage->addTab(new ConnectStatesDialog(m_pDevTranrecv), "设备连接检测");
    pDevManage->addTab(new TimeSetDialog(m_pDevTranrecv), "时钟同步");
    pDevManage->addTab(new DeviceResetDialog(m_pDevTranrecv), "设备重启");

    QTabWidget *pDevDetail = new QTabWidget;
    pDevDetail->addTab(new DeviceDetailDialog(m_pDevTranrecv), "设备信息");
    pDevDetail->addTab(new BettaryInfoDialog(m_pDevTranrecv), "电池信息");
    pDevDetail->addTab(new EleDetailDialog(m_pDevTranrecv), "电量信息");

    this->addWidget("设备配置", pDevSetDialog);
    this->addWidget("设备测试", pDevTestDialog);
    this->addWidget("设备管理", pDevManage);
    this->addWidget("设备信息", pDevDetail);

    return 0;
}

int DCD9500_MiniFSUTool::menuInit()
{
    QMenu *menus;
    /******************设置**********************/
    menus = ui->menuBar->addMenu(tr("连接设备(&C)"));
    QAction *pAction = menus->addAction("打开串口", this, SLOT(actionSerialOpen()));
    pAction->setStatusTip("打开串口设备");

    /******************更新固件***********************/
    menus = ui->menuBar->addMenu(tr("更新(&U)"));
    pAction = menus->addAction("更新固件", this, SLOT(actionUpgrade()));
    pAction->setStatusTip("更新蓄电池固件");
    m_actionList.append(pAction);

    menus = ui->menuBar->addMenu(tr("帮助(&H)"));
    pAction = menus->addAction("关于软件", this, SLOT(actionAbout()));
    pAction->setStatusTip("关于本软件的信息");

    return 0;
}
