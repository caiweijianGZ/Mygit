#ifndef TP02PROTOCOL_TYPE_H
#define TP02PROTOCOL_TYPE_H

#include <stdint.h>
#include <string.h>
#include <QMap>

#define TP02_HEAD                    0x4253
#define TP02_VER                     0x2

/*************************命令字**********************/
//升级
#define TP02_UPGRADE_TYPE_START          0xB1
#define TP02_UPGRADE_TYPE_ERASE          0xB2
#define TP02_UPGRADE_TYPE_DATA           0xB3
#define TP02_UPGRAD_TYPE_END             0xB4
#define TP02_UPGRAD_TYPE_GET_VER         0xB5
//TP02通用部分
#define TP02_GET_SERVER_INFO             0xA5
#define TP02_SET_SERVER_INFO             0xA6
#define TP02_GET_DEVICE_INFO             0xC0
#define TP02_SET_SERIAL_INFO             0xC1
#define TP02_SET_TIME                    0xC2
#define TP02_GET_TIME                    0xC3
#define TP02_DEVICE_RESET                0xC4

//设备类型
#define TP02_DEVICE_TYPE_UNKNOW          0x00
#define TP02_DEVICE_TYPE_BDM             0x01
#define TP02_DEVICE_TYPE_ACD             0x02
#define TP02_DEVICE_TYPE_AC_METER        0x03
#define TP02_DEVICE_TYPE_DC_METER        0x04
#define TP02_DEVICE_TYPE_WIRE_BDM        0x05
#define TP02_DEVICE_TYPE_ACD_8210        0x06
#define TP02_DEVICE_TYPE_DCD9500         0x07
#define TP02_DEVICE_TYPE_YIKE_POWER      0x08
//空调控制器导出指令
#define TP02_EXPORT_TYPE_ASK             0x04
#define TP02_EXPORT_TYPE_DATA            0x02
#define TP02_INPORT_TYPE_DATA            0x03
#define TP02_EXPORT_ROM_TYPE_ASK         0xA1
#define TP02_EXPORT_ROM_TYPE_DATA        0xA2

#define TP02_EXPORT_DC_CAL_ARGS          0xA7
#define TP02_IMPORT_DC_CAL_ARGS          0xA8

//错误码
#define TP02_RTN_NO_ERR                  0x00
#define TP02_RTN_ERR_VER                 0x01
#define TP02_RTN_ERR_CHECKSUM            0x02
#define TP02_RTN_ERR_CMD                 0x03
#define TP02_RTN_ERR_FORMAT              0x04
#define TP02_RTN_ERR_DATA                0x05

#pragma pack(1)

typedef struct TP02Package
{
    uint16_t head;
    uint16_t crc;
    uint8_t ver;
    uint8_t addr;
    uint8_t cmd;
    uint16_t len;//data长度
    uint8_t data[];
}__attribute__ ((gcc_struct, packed)) TP02Package_t;

typedef struct TP02M3SoftVer
{
    uint8_t mainVer;
    uint8_t secVer;
    uint8_t relVer;
}__attribute__ ((gcc_struct, packed)) M3SoftVer_t;

typedef struct ServerInfo
{
    uint8_t ip1;
    uint8_t ip2;
    uint8_t ip3;
    uint8_t ip4;
    uint16_t com;
}ServerInfo_t;

typedef struct deviceInfo
{
    char deviceName[30];
    char softwareVersion[20];
    char factoryName[20];
    char extra[];

    deviceInfo()
    {
        memset(deviceName, 0, sizeof(deviceName));
        memset(softwareVersion, 0, sizeof(softwareVersion));
        memset(factoryName, 0, sizeof(factoryName));
    }
} deviceInfo_t;

typedef struct SerialInfo
{
    uint8_t serialNo;
    uint8_t baudIndex;
    uint8_t dataBits;
    uint8_t parity;
    uint8_t stopBits;
}SerialInfo_t;

#pragma pack()

#endif // TP02PROTOCOL_TYPE_H
