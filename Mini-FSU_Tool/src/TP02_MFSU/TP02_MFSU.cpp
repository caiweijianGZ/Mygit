#include "TP02_MFSU.h"

TP02_MFSU::TP02_MFSU()
{

}

int TP02_MFSU::getChnAIData(TP02AIData_t *AIData)
{
    uint16_t len = 0;
    return this->getExportData((char*)AIData, len);
}

int TP02_MFSU::getChnDIData(TP02DIData_t *DIData)
{
    uint16_t len = 0;
    return this->getExportData((char*)DIData, len);
}

int TP02_MFSU::getPowerType(uint8_t &type)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    type = pPack->data[1];

    return TP02_RTN_NO_ERR;
}

int TP02_MFSU::getEleData(TP02EleData_t *data)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;

    memcpy(data, pPack->data+1, sizeof(TP02EleData_t));

    return TP02_RTN_NO_ERR;
}

int TP02_MFSU::getChnCfgData(char *data, uint16_t &len)
{
    return this->getExportData(data, len);
}

int TP02_MFSU::getSerialCfgData(char *data, uint16_t &len)
{
    return this->getExportData(data, len);
}

int TP02_MFSU::getExportDCArgs(char *data, uint16_t &len)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    uint8_t startIndex = sizeof(rtn);
    memcpy(data, pPack->data+startIndex, pPack->len-startIndex);
    len = pPack->len-startIndex;

    return TP02_RTN_NO_ERR;
}

int TP02_MFSU::getBettaryInfo(Tp02BettaryInfo_t *info)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    uint8_t startIndex = sizeof(rtn);
    memcpy(info, pPack->data+startIndex, pPack->len-startIndex);

    return TP02_RTN_NO_ERR;
}

int TP02_MFSU::getWorkStatus(char *data, uint16_t len)
{
    return this->getExportData(data, len);
}

int TP02_MFSU::getFSUID(uint64_t &id)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    uint8_t startIndex = sizeof(rtn);
    memcpy(&id, pPack->data+startIndex, pPack->len-startIndex);

    return TP02_RTN_NO_ERR;
}

int TP02_MFSU::getConnectStates(char *states)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    uint8_t startIndex = sizeof(rtn);
    memcpy(states, pPack->data+startIndex, pPack->len-startIndex);

    return TP02_RTN_NO_ERR;
}

int TP02_MFSU::genPkgExportChnCfgData(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_EXPORT_MFSU_CHN_CFG, addr, NULL, 0);
}

int TP02_MFSU::genPkgImportChnCfgData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen)

{
    return genTP02Package(pkg, len, TP02_IMPORT_MFSU_CHN_CFG, addr, data, dataLen);
}

int TP02_MFSU::genPkgExportSerialCfgData(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_EXPORT_SERIAL_CFG, addr, NULL, 0);
}

int TP02_MFSU::genPkgImportSerialCfgData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen)
{
    return genTP02Package(pkg, len, TP02_IMPORT_SERIAL_CFG, addr, data, dataLen);
}

int TP02_MFSU::genPkgGetChnAIData(char *pkg, uint16_t &len, uint8_t addr, uint8_t chnNo)
{
    return genTP02Package(pkg, len, TP02_CHN_AI_GET_DATA, addr, (char*)&chnNo, sizeof(chnNo));
}

int TP02_MFSU::genPkgGetChnDIData(char *pkg, uint16_t &len, uint8_t addr, uint8_t chnNo)
{
    return genTP02Package(pkg, len, TP02_CHN_DI_GET_DATA, addr, (char*)&chnNo, sizeof(chnNo));
}

int TP02_MFSU::genPkgControlChnDO(char *pkg, uint16_t &len, uint8_t addr, uint8_t chnNo, uint8_t IOValue)
{
    char extraData[sizeof(chnNo) + sizeof(IOValue)];
    memcpy(extraData, &chnNo, sizeof(chnNo));
    memcpy(extraData+1, &IOValue, sizeof(IOValue));

    return genTP02Package(pkg, len, TP02_CHN_DO_CONTROL, addr, extraData, sizeof(extraData));
}

int TP02_MFSU::genPkgTestUart(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_UART_TEST, addr, NULL, 0);
}

int TP02_MFSU::genPkgTestNBIot(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_NBIOT_TEST, addr, NULL, 0);
}

int TP02_MFSU::genPkgGetDevInfo(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_DEV_INFO, addr, NULL, 0);
}

int TP02_MFSU::genPkgGetPowType(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_POWER_TYPE, addr, NULL, 0);
}

int TP02_MFSU::genPkgSetPowType(char *pkg, uint16_t &len, uint8_t addr, uint8_t type)
{
    return genTP02Package(pkg, len, TP02_SET_POWER_TYPE, addr, (char*)&type, sizeof(type));
}

int TP02_MFSU::genPkgGetEleData(char *pkg, uint16_t &len, uint8_t addr)
{
     return genTP02Package(pkg, len, TP02_GET_ELE_DATA, addr, NULL, 0);
}

int TP02_MFSU::genPkgClearEleData(char *pkg, uint16_t &len, uint8_t addr)
{
     return genTP02Package(pkg, len, TP02_CLEAR_ELE_DATA, addr, NULL, 0);
}

int TP02_MFSU::genPkgImportLibStart(char *pkg, uint16_t &len, uint8_t addr, uint8_t comIndex)
{
    return genTP02Package(pkg, len, TP02_IMPORT_LIB_START, addr, (const char *)&comIndex, sizeof(comIndex));
}

int TP02_MFSU::genPkgImportLibData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen, uint16_t offset)
{
    char extraData[dataLen+sizeof(offset)];
    memcpy(extraData, &offset, sizeof(offset));
    memcpy(extraData+sizeof(offset), data, dataLen);

    return genTP02Package(pkg, len, TP02_IMPORT_LIB_DATA, addr, extraData, sizeof(extraData));
}

int TP02_MFSU::genPkgImportLibEnd(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_IMPORT_LIB_END, addr, NULL, 0);
}

int TP02_MFSU::genPkgExportMeterCalArgs(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_EXPORT_METER_CAL_ARGS, addr, NULL, 0);
}

int TP02_MFSU::genPkgImportMeterCalArgs(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen)
{
    char extraData[dataLen];
    memcpy(extraData, data, dataLen);

    return genTP02Package(pkg, len, TP02_IMPORT_METER_CAL_ARGS, addr, extraData, sizeof(extraData));
}

int TP02_MFSU::genPkgGetBettaryInfo(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_BETTARY_INFO, addr, NULL, 0);
}

int TP02_MFSU::genPkgGetWorkStatus(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_WORK_STATUS, addr, NULL, 0);
}

int TP02_MFSU::genPkgSetFSUID(char *pkg, uint16_t &len, uint8_t addr, uint64_t id)
{
    return genTP02Package(pkg, len, TP02_SET_FSU_ID, addr, (const char *)&id, sizeof(id));
}

int TP02_MFSU::genPkgGetFSUID(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_FSU_ID, addr, NULL, 0);
}

int TP02_MFSU::genPkgGetConnectStates(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_CONNECT_STATES, addr, NULL, 0);
}

