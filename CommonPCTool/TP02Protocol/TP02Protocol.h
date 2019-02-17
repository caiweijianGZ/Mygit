#ifndef TP02PROTOCOL_H
#define TP02PROTOCOL_H

#include <stdlib.h>
#include "tp02protocol_type.h"
#include "CRC/Service_Crc.h"
#include <QDebug>

class TP02Protocol
{
public:
    TP02Protocol();
    ~TP02Protocol();
    int fromRawData(const char *data, uint16_t len);
    //解析
    int getRtn(uint8_t &r);
    int getExportData(char *data, uint16_t &len);
    int getExportRomData(char *data, uint16_t &len);
    int getExportInfredData(char *data, uint16_t &len, uint8_t &type, uint8_t &pkgIndex);
    int getExportInfredNum(uint8_t &type, uint8_t &pkgTotal);
    int getSoftVer(M3SoftVer_t &softVer);
    int getServerInfo(ServerInfo_t &info);
    int getDeviceInfo(deviceInfo_t &info);
    int getTime(char *time);
    int getExportDCArgs(char *data, uint16_t &len, uint8_t &type);
    //组包    
    int genPkgExportRomAsk(char *pkg, uint16_t &len, uint8_t addr, uint32_t offset, uint32_t dataLen);
    int genPkgExportRomData(char *pkg, uint16_t &len, uint8_t addr, uint32_t offset, uint16_t dataLen);
    int genPkgExportInfredAsk(char *pkg, uint16_t &len, uint8_t addr, uint8_t type);
    int genPkgExportInfredData(char *pkg, uint16_t &len, uint8_t addr, uint8_t type, uint8_t selNo);
    int genPkgImportInfredData(char *pkg, uint16_t &len, uint8_t addr, uint8_t selNo, const char *data, uint16_t dataLen);
    int genPkgUpgradeStart(char *pkg, uint16_t &len, uint8_t addr, uint16_t devId);
    int genPkgUpgradeErase(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgUpgradeData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen, uint16_t offset);//这里的offset代表第几包如：1，2，3...
    int genPkgUpgradeEnd(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetSoftVersion(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetServerInfo(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetDeviceInfo(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgSetServerInfo(char *pkg, uint16_t &len, uint8_t addr, const ServerInfo_t &info);
    int genPkgSetSerialArgs(char *pkg, uint16_t &len, uint8_t addr, const SerialInfo_t &info);
    int genPkgSetTime(char *pkg, uint16_t &len, uint8_t addr, const char *time, uint16_t strlen = 20);
    int genPkgGetTime(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgDeviceReset(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgExportDCArgs(char *pkg, uint16_t &len, uint8_t addr, uint8_t argType = 0);
    int genPkgImportDCArgs(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen, uint8_t argType = 0);

protected:
    int genTP02Package(char *pkg, uint16_t &pkgLen, uint8_t cmd, uint8_t addr, const char *data, uint16_t dataLen);
    int dataClone(const char *data, uint16_t len);
    int isCrcPass();
    int isFormatPass();

protected:
    char *m_data;
    uint16_t m_dataLen;

};

#endif // TP02PROTOCOL_H
