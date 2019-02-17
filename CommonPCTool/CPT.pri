
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/common/deviceTransceiver.h \
    $$PWD/common/Logger.h \
    $$PWD/common/RingBuffer.h \
    $$PWD/commonWigets/absSetDialog.h \
    $$PWD/commonWigets/addrSetDialog.h \
    $$PWD/commonWigets/listStackedDialog.h \
    $$PWD/commonWigets/romExportDialog.h \
    $$PWD/commonWigets/waittingDialog.h \
    $$PWD/interface/AbsRWInterface.h \
    $$PWD/interface/SerialPortInterface.h \
    $$PWD/interface/tcpInterface.h \
    $$PWD/interface/udpInterface.h \
    $$PWD/interface/winSerialPortInterface.h \
    $$PWD/serial/winSerialPort.h \
    $$PWD/TP02Protocol/TP02Protocol.h \
    $$PWD/TP02Protocol/tp02protocol_type.h \
    $$PWD/upgrade/upgradeDialog.h \
    $$PWD/upgrade/upgradedialog_type.h \
    $$PWD/PMBusProtocol/PMBusProtocol.h \
    $$PWD/PMBusProtocol/pmbusprotocol_type.h \
    $$PWD/tcp/tcpClient.h \
    $$PWD/tcp/tcpServer.h \
    $$PWD/udp/UdpClient.h \
    $$PWD/CRC/Service_Crc.h

SOURCES += \
    $$PWD/common/deviceTransceiver.cpp \
    $$PWD/common/Logger.cpp \
    $$PWD/common/RingBuffer.cpp \
    $$PWD/commonWigets/absSetDialog.cpp \
    $$PWD/commonWigets/addrSetDialog.cpp \
    $$PWD/commonWigets/listStackedDialog.cpp \
    $$PWD/commonWigets/romExportDialog.cpp \
    $$PWD/commonWigets/waittingDialog.cpp \
    $$PWD/interface/AbsRWInterface.cpp \
    $$PWD/interface/SerialPortInterface.cpp \
    $$PWD/interface/tcpInterface.cpp \
    $$PWD/interface/udpInterface.cpp \
    $$PWD/interface/winSerialPortInterface.cpp \
    $$PWD/serial/winSerialPort.cpp \
    $$PWD/TP02Protocol/TP02Protocol.cpp \
    $$PWD/upgrade/upgradeDialog.cpp \
    $$PWD/PMBusProtocol/PMBusProtocol.cpp \
    $$PWD/tcp/tcpClient.cpp \
    $$PWD/tcp/tcpServer.cpp \
    $$PWD/udp/UdpClient.cpp \
    $$PWD/CRC/Service_Crc.c

FORMS += \
    $$PWD/commonWigets/addrSetDialog.ui \
    $$PWD/commonWigets/listStackedDialog.ui \
    $$PWD/commonWigets/romExportDialog.ui \
    $$PWD/commonWigets/waittingDialog.ui \
    $$PWD/upgrade/upgradeDialog.ui


