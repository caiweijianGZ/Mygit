#ifndef TP02_MFSU_H
#define TP02_MFSU_H

#include "TP02Protocol/TP02Protocol.h"
#include "tp02_mfsu_type.h"

class TP02_MFSU : public TP02Protocol
{
public:
    TP02_MFSU();

    int getChnAIData(TP02AIData_t *AIData);
    int getChnDIData(TP02DIData_t *DIData);
    int getPowerType(uint8_t &type);
    int getEleData(TP02EleData_t *data);
    int getChnCfgData(char *data, uint16_t &len);
    int getSerialCfgData(char *data, uint16_t &len);
    int getExportDCArgs(char *data, uint16_t &len);
    int getBettaryInfo(Tp02BettaryInfo_t *info);
    int getWorkStatus(char *data, uint16_t len = 200);
    int getFSUID(uint64_t &id);
    int getConnectStates(char *states);

    int genPkgExportChnCfgData(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgImportChnCfgData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen);
    int genPkgExportSerialCfgData(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgImportSerialCfgData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen);
    int genPkgGetChnAIData(char *pkg, uint16_t &len, uint8_t addr, uint8_t chnNo);
    int genPkgGetChnDIData(char *pkg, uint16_t &len, uint8_t addr, uint8_t chnNo);
    int genPkgControlChnDO(char *pkg, uint16_t &len, uint8_t addr, uint8_t chnNo, uint8_t IOValue);
    int genPkgTestUart(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgTestNBIot(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetDevInfo(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetPowType(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgSetPowType(char *pkg, uint16_t &len, uint8_t addr, uint8_t type);
    int genPkgGetEleData(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgClearEleData(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgImportLibStart(char *pkg, uint16_t &len, uint8_t addr, uint8_t comIndex);
    int genPkgImportLibData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen, uint16_t offset);//这里的offset代表第几包如：1，2，3...
    int genPkgImportLibEnd(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgExportMeterCalArgs(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgImportMeterCalArgs(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen);
    int genPkgGetBettaryInfo(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetWorkStatus(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgSetFSUID(char *pkg, uint16_t &len, uint8_t addr, uint64_t id);
    int genPkgGetFSUID(char *pkg, uint16_t &len, uint8_t addr);
    int genPkgGetConnectStates(char *pkg, uint16_t &len, uint8_t addr);
};

#endif // TP02_MFSU_H
