#-------------------------------------------------
#
# Project created by QtCreator 2018-11-06T09:14:13
#
#-------------------------------------------------

QT       += core gui    \
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mini-FSU_Tool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(config.h)
include(F:\qtproject\CommonPCTool\CPT.pri)

INCLUDEPATH += ./

SOURCES += \
        main.cpp \
        MiniFSUTool.cpp \
    SerialOpenDialog.cpp \
    productCreator.cpp \
    product/DCD9500/DCD9500_MiniFSUTool.cpp \
    product/DCD9500/ManageDialog.cpp \
    product/DCD9500/DeviceDetailDialog.cpp \
    product/DCD9500/DeviceTestDialog/AITestDialog.cpp \
    product/DCD9500/DeviceTestDialog/DITestDialog.cpp \
    product/DCD9500/DeviceTestDialog/DOTestDialog.cpp \
    product/DCD9500/DeviceTestDialog/NBIotTestDialog.cpp \
    product/DCD9500/DeviceTestDialog/TransObject.cpp \
    product/DCD9500/DeviceTestDialog/UartTestDialog.cpp \
    cJSON/cJSON.c \
    product/DCD9500/OtherSetDialog.cpp \
    product/DCD9500/EleDetailDialog.cpp \
    product/DCD9500/SerialItemWidget/SerialItemWidget.cpp \
    product/DCD9500/SerialSetDialog.cpp \
    product/DCD9500/ChannelSetDialog/AI_DICfgDialog.cpp \
    product/DCD9500/ChannelSetDialog/ChannelSetDialog.cpp \
    product/DCD9500/ChannelSetDialog/DOCfgDialog.cpp \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/AIItemWidget.cpp \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/ChnItemWidget.cpp \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/DIItemWidget.cpp \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/DOItemWidget.cpp \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/SignalIdsEditDialog.cpp \    
    product/DCD9500/SerialItemWidget/LibImportDialog/LibImportDialog.cpp \
    product/DCD9500/CalArgsSetDialog.cpp \
    TP02_MFSU/TP02_MFSU.cpp \
    product/DCD9500/BettaryInfoDialog.cpp \
    product/DCD9500/WorkStatusDialog.cpp \
    product/DCD9500/TimeSetDialog.cpp \
    product/DCD9500/DeviceResetDialog.cpp \
    product/DCD9500/ConnectStatesDialog.cpp

HEADERS += \
        MiniFSUTool.h \
    SerialOpenDialog.h \
    productCreator.h \
    product/DCD9500/DCD9500_MiniFSUTool.h \
    product/DCD9500/ManageDialog.h \
    product/DCD9500/DeviceDetailDialog.h \
    product/DCD9500/DeviceTestDialog/AITestDialog.h \
    product/DCD9500/DeviceTestDialog/DITestDialog.h \
    product/DCD9500/DeviceTestDialog/DOTestDialog.h \
    product/DCD9500/DeviceTestDialog/NBIotTestDialog.h \
    product/DCD9500/DeviceTestDialog/TransObject.h \
    product/DCD9500/DeviceTestDialog/UartTestDialog.h \
    cJSON/cJSON.h \
    product/DCD9500/OtherSetDialog.h \
    product/DCD9500/TransObject.h \
    product/DCD9500/EleDetailDialog.h \
    product/DCD9500/SerialItemWidget/SerialItemWidget.h \
    product/DCD9500/SerialSetDialog.h \
    product/DCD9500/SerialItemWidget/serialitem_type.h \
    product/DCD9500/ChannelSetDialog/AI_DICfgDialog.h \
    product/DCD9500/ChannelSetDialog/channel_type.h \
    product/DCD9500/ChannelSetDialog/ChannelSetDialog.h \
    product/DCD9500/ChannelSetDialog/DOCfgDialog.h \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/AIItemWidget.h \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/ChnItemWidget.h \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/DIItemWidget.h \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/DOItemWidget.h \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/SignalIdsEditDialog.h \    
    product/DCD9500/SerialItemWidget/LibImportDialog/LibImportDialog.h \
    product/DCD9500/SerialItemWidget/LibImportDialog/LibImportDialog_type.h \
    product/DCD9500/CalArgsSetDialog.h \    
    TP02_MFSU/TP02_MFSU.h \
    TP02_MFSU/tp02_mfsu_type.h \
    product/DCD9500/BettaryInfoDialog.h \
    product/DCD9500/WorkStatusDialog.h \
    product/DCD9500/TimeSetDialog.h \
    product/DCD9500/DeviceResetDialog.h \
    product/DCD9500/ConnectStatesDialog.h

FORMS += \
        MiniFSUTool.ui \
    SerialOpenDialog.ui \
    product/DCD9500/ManageDialog.ui \
    product/DCD9500/DeviceDetailDialog.ui \
    product/DCD9500/DeviceTestDialog/AITestDialog.ui \
    product/DCD9500/DeviceTestDialog/DITestDialog.ui \
    product/DCD9500/DeviceTestDialog/DOTestDialog.ui \
    product/DCD9500/DeviceTestDialog/NBIotTestDialog.ui \
    product/DCD9500/DeviceTestDialog/UartTestDialog.ui \
    product/DCD9500/OtherSetDialog.ui \
    product/DCD9500/EleDetailDialog.ui \
    product/DCD9500/SerialSetDialog.ui \
    product/DCD9500/ChannelSetDialog/AI_DICfgDialog.ui \
    product/DCD9500/ChannelSetDialog/ChannelSetDialog.ui \
    product/DCD9500/ChannelSetDialog/DOCfgDialog.ui \
    product/DCD9500/ChannelSetDialog/ChannelItemWidget/SignalIdsEditDialog.ui \
    product/DCD9500/SerialItemWidget/LibImportDialog/LibImportDialog.ui \
    product/DCD9500/CalArgsSetDialog.ui \
    product/DCD9500/BettaryInfoDialog.ui \
    product/DCD9500/WorkStatusDialog.ui \
    product/DCD9500/TimeSetDialog.ui \
    product/DCD9500/DeviceResetDialog.ui \
    product/DCD9500/ConnectStatesDialog.ui

LIBS += -lws2_32

DESTDIR = ../release

RESOURCES += \
    img.qrc

