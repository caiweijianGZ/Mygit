#ifndef PMBUSPROTOCOL_TYPE_H
#define PMBUSPROTOCOL_TYPE_H

#include <stdint.h>
#include <string.h>

#define PMBUS_HEAD                      0x7E
#define PMBUS_END                       0x0D
#define PMBUS_ADDR                      0x01
#define PMBUS_DEVICE_TYPE               0x46
#define PMBUS_VER                       0x21

#define PMBUS_RTN_NO_ERR                0x00
#define PMBUS_RTN_ERR_VER               0x01
#define PMBUS_RTN_ERR_CHECKSUM          0x02
#define PMBUS_RTN_ERR_LCHECKSUM         0x03
#define PMBUS_RTN_ERR_CID2              0x04
#define PMBUS_RTN_ERR_FORMAT            0x05
#define PMBUS_RTN_ERR_DATA              0x06

#define PMBUS_CID2_GET_ALG              0x42 //获取模拟量
#define PMBUS_CID2_GET_ALM              0x44
#define PMBUS_CID2_GET_SYSARG           0x47
#define PMBUS_CID2_SET_SYSARG           0x49
#define PMBUS_CID2_SET_TIME             0x4E
#define PMBUS_CID2_GET_VER              0x4F
#define PMBUS_CID2_GET_ADDR             0x50
#define PMBUS_CID2_GET_FACINFO          0x51 //获取厂家信息
#define PMBUS_CID2_SET_ADDR             0x93

#define PMBUS_SYSARG_MAX_VOL            0x80
#define PMBUS_SYSARG_MIN_VOL            0x81
#define PMBUS_SYSARG_MAX_TEM            0x82
#define PMBUS_SYSARG_MIN_TEM            0x83
#define PMBUS_SYSARG_MAX_ELE            0x84
#define PMBUS_SYSARG_MAX_TOTALVOL       0x85
#define PMBUS_SYSARG_MIN_TOTALVOL       0x86

#define PMBUS_ALARM_STATUS_NORMAL       0
#define PMBUS_ALARM_STATUS_LESS         1
#define PMBUS_ALARM_STATUS_MORE         2

#define POINTER_TYPE_BTY_COUNT          0
#define POINTER_TYPE_VOL                1
#define POINTER_TYPE_TEM                2
#define POINTER_TYPE_GROUP_ELE          3
#define POINTER_TYPE_GROUP_VOL          4
#define POINTER_TYPE_GROUP_POW          5
#define POINTER_TYPE_USR                6



typedef struct PMBusPackage
{
    uint8_t head;
    uint8_t ver;
    uint8_t addr;
    uint8_t cid1;
    uint8_t cid2;
    uint16_t len;
    char info[];
    uint16_t crc;
    uint8_t end;
    PMBusPackage() {
        head    = PMBUS_HEAD;
        ver     = PMBUS_VER;
        addr    = PMBUS_ADDR;//设备地址
        cid1    = PMBUS_DEVICE_TYPE;//设备类型
        cid2    = 0x01;//设备控制码，也就是命令
        len     = 0x00;
        crc     = 0x00;
        end     = PMBUS_END;
    }
}__attribute__ ((gcc_struct, packed)) PMBusPackage_t;

typedef struct baseBettaryInfo
{
    int16_t signalVolMax;
    int16_t signalVolMin;
    int16_t temMax;
    int16_t temMin;
    int16_t chargeEleMax;
    int16_t totalVolMax;
    int16_t totalVolMin;
    uint8_t dataLen;
    int16_t data[6];//自定义数据
}__attribute__ ((gcc_struct, packed)) baseBettaryInfo_t;

typedef struct bettarySysInfo
{
    uint8_t arryCount;
    baseBettaryInfo_t baseInfo[];
}__attribute__ ((gcc_struct, packed)) btySysInfo_t;

typedef struct factoryInfo
{
    char deviceName[30];
    char softwareVersion[20];
    char factoryName[30];

    factoryInfo()
    {
        memset(deviceName, 0, sizeof(deviceName));
        memset(softwareVersion, 0, sizeof(softwareVersion));
        memset(factoryName, 0, sizeof(factoryName));
    }
}__attribute__ ((gcc_struct, packed)) factoryInfo_t;



#endif // PMBUSPROTOCOL_TYPE_H
