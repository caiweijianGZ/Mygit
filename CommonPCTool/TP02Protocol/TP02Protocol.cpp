#include "TP02Protocol.h"

TP02Protocol::TP02Protocol()
{
    m_data = NULL;
    m_dataLen = 0;
}

TP02Protocol::~TP02Protocol()
{
    if ( NULL != m_data )
    {
        delete[] m_data;
    }
}

int TP02Protocol::fromRawData(const char *data, uint16_t len)
{
    int ret = this->dataClone(data, len);
    if ( ret )
    {
        return TP02_RTN_ERR_DATA;
    }

    ret = this->isFormatPass();
    if ( ret )
    {
        return TP02_RTN_ERR_FORMAT;
    }

    ret = this->isCrcPass();
    if ( ret )
    {
        return TP02_RTN_ERR_CHECKSUM;
    }

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getRtn(uint8_t &r)
{
    if ( m_dataLen < sizeof(TP02Package_t) )
    {
        return -1;
    }
    //回复的第一个字节是错误码
    TP02Package_t *pPack = (TP02Package_t*)m_data;
    r = pPack->data[0];

    return 0;
}

int TP02Protocol::getExportData(char *data, uint16_t &len)
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

int TP02Protocol::getExportRomData(char *data, uint16_t &len)
{
    return this->getExportData(data, len);
}

int TP02Protocol::getExportInfredData(char *data, uint16_t &len, uint8_t &type, uint8_t &pkgIndex)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    //uint8_t startIndex = sizeof(type) + sizeof(pkgIndex) + sizeof(rtn);
    uint8_t startIndex = sizeof(rtn);
    memcpy(data, pPack->data+startIndex, pPack->len-startIndex);
    len = pPack->len-startIndex;
    type = data[1];
    pkgIndex = data[2];

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getExportInfredNum(uint8_t &type, uint8_t &pkgTotal)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    type = pPack->data[1];
    pkgTotal = pPack->data[2];

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getSoftVer(M3SoftVer_t &softVer)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    softVer.mainVer = pPack->data[1];
    softVer.secVer = pPack->data[2];
    softVer.relVer = pPack->data[3];

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getServerInfo(ServerInfo_t &info)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    memcpy(&info, pPack->data+1, sizeof(info));

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getDeviceInfo(deviceInfo_t &info)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;

    uint8_t startIndex = sizeof(rtn);
    memcpy(&info, pPack->data+startIndex, pPack->len-startIndex);

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getTime(char *time)
{
    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;

    uint8_t startIndex = sizeof(rtn);
    memcpy(time, pPack->data+startIndex, pPack->len-startIndex);

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::getExportDCArgs(char *data, uint16_t &len, uint8_t &type)
{
    if ( m_dataLen <= sizeof(TP02Package_t) + sizeof(uint8_t) + sizeof(uint8_t) )//以后是否也要加上去？
    {
        return -1;
    }

    uint8_t rtn = 0;
    this->getRtn(rtn);
    if ( rtn )
    {
        return rtn;
    }

    TP02Package_t *pPack = (TP02Package_t*)m_data;
    type = *(pPack->data+sizeof(rtn));
    uint8_t startIndex = sizeof(rtn) + sizeof(type);
    memcpy(data, pPack->data+startIndex, pPack->len-startIndex);
    len = pPack->len-startIndex;

    return TP02_RTN_NO_ERR;
}

int TP02Protocol::genPkgExportRomAsk(char *pkg, uint16_t &len, uint8_t addr, uint32_t offset, uint32_t dataLen)
{
    char extraData[sizeof(uint32_t)*2];
    memcpy(extraData, &offset, sizeof(offset));
    memcpy(extraData+sizeof(offset), &dataLen, sizeof(dataLen));

    return genTP02Package(pkg, len, TP02_EXPORT_ROM_TYPE_ASK, addr, extraData, sizeof(extraData));
}

int TP02Protocol::genPkgExportRomData(char *pkg, uint16_t &len, uint8_t addr, uint32_t offset, uint16_t dataLen)
{
    char extraData[sizeof(offset)+sizeof(dataLen)];
    memcpy(extraData, &offset, sizeof(offset));
    memcpy(extraData+sizeof(offset), &dataLen, sizeof(dataLen));

    return genTP02Package(pkg, len, TP02_EXPORT_ROM_TYPE_DATA, addr, extraData, sizeof(extraData));
}

int TP02Protocol::genPkgExportInfredAsk(char *pkg, uint16_t &len, uint8_t addr, uint8_t type)
{
    return genTP02Package(pkg, len, TP02_EXPORT_TYPE_ASK, addr, (const char*)&type, sizeof(type));
}

int TP02Protocol::genPkgExportInfredData(char *pkg, uint16_t &len, uint8_t addr, uint8_t type, uint8_t selNo)
{
    char extraData[sizeof(type)+sizeof(selNo)];
    memcpy(extraData, &type, sizeof(type));
    memcpy(extraData+sizeof(type), &selNo, sizeof(selNo));

    return genTP02Package(pkg, len, TP02_EXPORT_TYPE_DATA, addr, extraData, sizeof(extraData));
}

int TP02Protocol::genPkgImportInfredData(char *pkg, uint16_t &len, uint8_t addr, uint8_t selNo, const char *data, uint16_t dataLen)
{
    char extraData[dataLen+sizeof(selNo)];
    memcpy(extraData, &selNo, sizeof(selNo));
    memcpy(extraData+sizeof(selNo), data, dataLen);

    return genTP02Package(pkg, len, TP02_INPORT_TYPE_DATA, addr, extraData, sizeof(extraData));
}

int TP02Protocol::genPkgUpgradeStart(char *pkg, uint16_t &len, uint8_t addr, uint16_t devId)
{
    return genTP02Package(pkg, len, TP02_UPGRADE_TYPE_START, addr, (const char *)&devId, sizeof(devId));
}

int TP02Protocol::genPkgUpgradeErase(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_UPGRADE_TYPE_ERASE, addr, NULL, 0);
}

int TP02Protocol::genPkgUpgradeData(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen, uint16_t offset)
{
    char extraData[dataLen+sizeof(offset)];
    memcpy(extraData, &offset, sizeof(offset));
    memcpy(extraData+sizeof(offset), data, dataLen);

    return genTP02Package(pkg, len, TP02_UPGRADE_TYPE_DATA, addr, extraData, sizeof(extraData));
}

int TP02Protocol::genPkgUpgradeEnd(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_UPGRAD_TYPE_END, addr, NULL, 0);
}

int TP02Protocol::genPkgGetSoftVersion(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_UPGRAD_TYPE_GET_VER, addr, NULL, 0);
}

int TP02Protocol::genPkgGetServerInfo(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_SERVER_INFO, addr, NULL, 0);
}

int TP02Protocol::genPkgGetDeviceInfo(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_DEVICE_INFO, addr, NULL, 0);
}

int TP02Protocol::genPkgSetServerInfo(char *pkg, uint16_t &len, uint8_t addr, const ServerInfo_t &info)
{
    return genTP02Package(pkg, len, TP02_SET_SERVER_INFO, addr, (char *)&info, sizeof(info));
}

int TP02Protocol::genPkgSetSerialArgs(char *pkg, uint16_t &len, uint8_t addr, const SerialInfo_t &info)
{
    return genTP02Package(pkg, len, TP02_SET_SERIAL_INFO, addr, (char *)&info, sizeof(info));
}

int TP02Protocol::genPkgSetTime(char *pkg, uint16_t &len, uint8_t addr, const char *time, uint16_t strlen)
{
    return genTP02Package(pkg, len, TP02_SET_TIME, addr, time, strlen);
}

int TP02Protocol::genPkgGetTime(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_GET_TIME, addr, NULL, 0);
}

int TP02Protocol::genPkgDeviceReset(char *pkg, uint16_t &len, uint8_t addr)
{
    return genTP02Package(pkg, len, TP02_DEVICE_RESET, addr, NULL, 0);
}

int TP02Protocol::genPkgExportDCArgs(char *pkg, uint16_t &len, uint8_t addr, uint8_t argType)
{
    return genTP02Package(pkg, len, TP02_EXPORT_DC_CAL_ARGS, addr, (char*)&argType, sizeof(argType));
}

int TP02Protocol::genPkgImportDCArgs(char *pkg, uint16_t &len, uint8_t addr, const char *data, uint16_t dataLen, uint8_t argType)
{
    char extraData[dataLen+sizeof(argType)];
    memcpy(extraData, &argType, sizeof(argType));
    memcpy(extraData+sizeof(argType), data, dataLen);

    return genTP02Package(pkg, len, TP02_IMPORT_DC_CAL_ARGS, addr, extraData, sizeof(extraData));
}

int TP02Protocol::genTP02Package(char *pkg, uint16_t &pkgLen, uint8_t cmd, uint8_t addr, const char *data, uint16_t dataLen)
{
    uint16_t packLen = sizeof(TP02Package_t) + dataLen;
    TP02Package_t *pPack = (TP02Package_t*)malloc(packLen);
    memset(pPack, 0, packLen);
    unsigned char *pPMPackData = (unsigned char*)pPack;

    pPack->head = TP02_HEAD;
    pPack->ver = TP02_VER;
    pPack->addr = addr;
    pPack->cmd = cmd;
    pPack->len = dataLen;
    if (dataLen)
    {
        memcpy(pPack->data, data, dataLen);
    }
    uint16_t crcStart = sizeof(pPack->head) + sizeof(pPack->crc);
    uint16_t crcDataLen = packLen - crcStart;
    uint16_t calCrc = CalcCrcAll(0, (unsigned char*)pPMPackData+crcStart, crcDataLen);
    pPack->crc = calCrc;

    memcpy(pkg, pPack, packLen);
    pkgLen = packLen;

    return 0;
}

int TP02Protocol::dataClone(const char *data, uint16_t len)
{
    if ( len < sizeof(TP02Package_t) )
    {
        return -1;
    }

    if ( NULL != m_data )
    {
        delete[] m_data;
    }

    m_dataLen = len;
    m_data = new char[len];
    memcpy(m_data, data, len);

    return 0;
}

int TP02Protocol::isCrcPass()
{
    TP02Package_t *pPack = (TP02Package_t *)m_data;
    uint16_t crcStart = sizeof(pPack->head) + sizeof(pPack->crc);
    uint16_t crcDataLen = pPack->len + 5;
    uint16_t calCrc = CalcCrcAll(0, (unsigned char*)m_data+crcStart, crcDataLen);
    if ( pPack->crc != calCrc )
    {
        qDebug() << "crc1:" << pPack->crc << "crc2:" << calCrc;
        return -1;
    }

    return 0;
}

int TP02Protocol::isFormatPass()
{
    char *pStart = m_data;
    char *pEnd = m_data + m_dataLen - 1;
    while ( (unsigned int)(pEnd-pStart+1) >= sizeof(TP02Package_t) )
    {
        TP02Package_t *pPack = (TP02Package_t *)pStart;
        if ( pPack->head == TP02_HEAD )
        {
            char tmpBuff[pEnd-pStart+1];
            memcpy(tmpBuff, pStart, sizeof(tmpBuff));
            return this->dataClone(tmpBuff, sizeof(tmpBuff));
        }
        pStart++;
    }

    return -1;
}
