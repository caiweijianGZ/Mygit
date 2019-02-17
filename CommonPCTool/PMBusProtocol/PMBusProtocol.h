#ifndef PMBUSPROTOCOL_H
#define PMBUSPROTOCOL_H

#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <QByteArray>
#include <QDebug>

#include "pmbusprotocol_type.h"

class PMBusProtocol
{
public:
    PMBusProtocol();
    virtual ~PMBusProtocol();
    /* 解析包 */
    int fromRawData(const char *data, uint16_t len);
    /* 模拟量 */
    int getBtyGroupCount(uint8_t &count);
    int getVolArray(int16_t *volArray, uint8_t &count, uint8_t group);
    int getTemArray(int16_t *temArray, uint8_t &count, uint8_t group);
    int getEleGroup(int16_t &eleVal, uint8_t group);//电流
    int getVolGroup(int16_t &volVal, uint8_t group);//电压
    int getPowGroup(int16_t &powVal, uint8_t group);//安时
    /* 告警状态 */
    int getVoAlmArray(uint8_t *volArray, uint8_t &count, uint8_t group);
    int getTemAlmArray(uint8_t *temArray, uint8_t &count, uint8_t group);
    int getEleAlmGroup(uint8_t &eleVal, uint8_t group);//电流
    int getVolAlmGroup(uint8_t &volVal, uint8_t group);//电压

    int getSofVersion(uint8_t &version);
    int getDevAddress(uint8_t &addr);
    int getFactoryInfo(factoryInfo_t *pFacInfo);
    int getRtn(uint8_t &rtn);
    /* 创建包 */    
    int genPMPackage(char *data, uint16_t &len, uint8_t cmd, uint8_t addr=PMBUS_ADDR, uint8_t devType=PMBUS_DEVICE_TYPE, uint8_t ver=PMBUS_VER);
    int genPMPackage(char *data, uint16_t &len, uint8_t cmd, uint8_t addr,
                     uint8_t devType, const char *info, uint16_t infoLen, uint8_t ver=PMBUS_VER);
    int genPkgGetAnalog(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t group);
    int genPkgGetAlm(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t group);
    int genPkgGetSysInfo(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t group);
    int genPkgGetFacInfo(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t ver);
    int genPkgGetVersion(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t ver);
    int genPkgGetAddress(char *data, uint16_t &len, uint8_t devType, uint8_t ver);
    int genPkgGetAddress(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t ver);
    int genPkgSetAddress(char *data, uint16_t &len, uint8_t addrOld, uint8_t addrNew, uint8_t devType, uint8_t ver);
    int genPkgSetSysArg(char *data, uint16_t &len, uint8_t addr, uint8_t devType,
                        uint8_t argType, uint8_t group, uint16_t value);

    virtual int getBtySysInfo(btySysInfo_t *pSysInfo);

protected:
    int dataClone(const char *data, uint16_t len);
    int ascii2Hex(unsigned char *hexData, const unsigned char *asciiData, uint16_t asciiLen);
    int PMascii2Hex();
    int hex2Ascii(unsigned char *asciiData, const unsigned char *hexData, uint16_t hexLen);
    uint16_t calCrc16(const unsigned char *data, uint16_t len);
    uint8_t calLenCrc(uint16_t data);
    bool isCrcPass();
    bool isLenCrcPass();
    bool isFormatPass();
    virtual char *getAnaValPtr(int type, int group);//获取模拟量值得指针
    virtual char *getAlmValPtr(int type, int group);


protected:
    char *m_data;
    char *m_hexData;
    uint16_t m_dataLen;
    uint16_t m_dataHexLen;

};

#endif // PMBUSPROTOCOL_H
