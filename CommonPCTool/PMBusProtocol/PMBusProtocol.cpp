#include "PMBusProtocol.h"

PMBusProtocol::PMBusProtocol()
{
    m_data = NULL;
    m_hexData = NULL;
    m_dataLen = 0;
}

PMBusProtocol::~PMBusProtocol()
{
    if ( NULL != m_data )
    {
        delete[] m_data;
    }

    if ( NULL != m_hexData )
    {
        delete[] m_hexData;
    }
}

int PMBusProtocol::fromRawData(const char *data, uint16_t len)
{
    int ret = this->dataClone(data, len);
    if ( ret )
    {
        return PMBUS_RTN_ERR_DATA;
    }

    if ( !this->isFormatPass() )
    {
        qDebug() << "format error!";
        return PMBUS_RTN_ERR_FORMAT;
    }

    this->PMascii2Hex();

    if ( !this->isCrcPass() )
    {
        qDebug() << "crc error!";
        return PMBUS_RTN_ERR_CHECKSUM;
    }

    if ( !this->isLenCrcPass() )
    {
        qDebug() << "len crc error!";
        return PMBUS_RTN_ERR_LCHECKSUM;
    }

    return PMBUS_RTN_NO_ERR;
}


int PMBusProtocol::getBtyGroupCount(uint8_t &count)
{
    char *pStart = getAnaValPtr(POINTER_TYPE_BTY_COUNT, 0);
    if ( NULL == pStart )
    {
        return -1;
    }
    count = *(uint8_t*)pStart;

    return 0;
}

int PMBusProtocol::getVolArray(int16_t *volArray, uint8_t &count, uint8_t group)
{
    char *pStart = getAnaValPtr(POINTER_TYPE_VOL, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    uint8_t *volCount = (uint8_t*)pStart;
    count = *volCount;
    pStart += sizeof(uint8_t);
    for ( int i=0; i<count; i++ )
    {
        int16_t *pVol = (int16_t*)pStart;
        volArray[i] = ntohs(*pVol);
        pStart += sizeof(int16_t);
    }

    return 0;
}

int PMBusProtocol::getTemArray(int16_t *temArray, uint8_t &count, uint8_t group)
{
    char *pStart = getAnaValPtr(POINTER_TYPE_TEM, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    uint8_t *temCount = (uint8_t*)pStart;
    count = *temCount;
    pStart += sizeof(uint8_t);
    for ( int i=0; i<count; i++ )
    {
        int16_t *pTem = (int16_t*)pStart;
        temArray[i] = ntohs(*pTem);
        pStart += sizeof(int16_t);
    }

    return 0;
}

int PMBusProtocol::getEleGroup(int16_t &eleVal, uint8_t group)
{
    char *pStart = getAnaValPtr(POINTER_TYPE_GROUP_ELE, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    int16_t *pEleVal = (int16_t*)pStart;
    eleVal = ntohs(*pEleVal);

    return 0;
}

int PMBusProtocol::getVolGroup(int16_t &volVal, uint8_t group)
{
    char *pStart = getAnaValPtr(POINTER_TYPE_GROUP_VOL, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    int16_t *pvolVal = (int16_t*)pStart;
    volVal = ntohs(*pvolVal);

    return 0;
}

int PMBusProtocol::getPowGroup(int16_t &powVal, uint8_t group)
{
    char *pStart = getAnaValPtr(POINTER_TYPE_GROUP_POW, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    int16_t *pPowVal = (int16_t*)pStart;
    powVal = ntohs(*pPowVal);

    return 0;
}

int PMBusProtocol::getVoAlmArray(uint8_t *volArray, uint8_t &count, uint8_t group)
{
    char *pStart = getAlmValPtr(POINTER_TYPE_VOL, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    uint8_t *volCount = (uint8_t*)pStart;
    count = *volCount;
    pStart += sizeof(uint8_t);
    for ( int i=0; i<count; i++ )
    {
        uint8_t *pVol = (uint8_t*)pStart;
        volArray[i] = *pVol;
        pStart += sizeof(uint8_t);
    }

    return 0;
}

int PMBusProtocol::getTemAlmArray(uint8_t *temArray, uint8_t &count, uint8_t group)
{
    char *pStart = getAlmValPtr(POINTER_TYPE_TEM, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    uint8_t *temCount = (uint8_t*)pStart;
    count = *temCount;
    pStart += sizeof(uint8_t);
    for ( int i=0; i<count; i++ )
    {
        uint8_t *pTem = (uint8_t*)pStart;
        temArray[i] = *pTem;
        pStart += sizeof(uint8_t);
    }

    return 0;
}

int PMBusProtocol::getEleAlmGroup(uint8_t &eleVal, uint8_t group)
{
    char *pStart = getAlmValPtr(POINTER_TYPE_GROUP_ELE, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    uint8_t *pEleVal = (uint8_t*)pStart;
    eleVal = *pEleVal;

    return 0;
}

int PMBusProtocol::getVolAlmGroup(uint8_t &volVal, uint8_t group)
{
    char *pStart = getAlmValPtr(POINTER_TYPE_GROUP_VOL, group);
    if ( NULL == pStart )
    {
        return -1;
    }

    uint8_t *pEVolVal = (uint8_t*)pStart;
    volVal = *pEVolVal;

    return 0;
}

int PMBusProtocol::getSofVersion(uint8_t &version)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    version = pPMBusPack->ver;

    return 0;
}

int PMBusProtocol::getDevAddress(uint8_t &addr)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    addr = pPMBusPack->addr;

    return 0;
}

int PMBusProtocol::getFactoryInfo(factoryInfo_t *pFacInfo)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    uint16_t infoLen = (ntohs(pPMBusPack->len) & 0xFFF)/2;
    if ( infoLen <=0 )
    {
        return -1;
    }

    char *pInfo = (char*)pPMBusPack->info;
    memcpy(pFacInfo, pInfo, infoLen);

    return 0;
}

int PMBusProtocol::getRtn(uint8_t &rtn)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    rtn = pPMBusPack->cid2;

    return 0;
}

int PMBusProtocol::genPMPackage(char *data, uint16_t &len, uint8_t cmd, uint8_t addr, uint8_t devType, uint8_t ver)
{
    return this->genPMPackage(data, len, cmd, addr, devType, NULL, 0, ver);
}

int PMBusProtocol::genPMPackage(char *data, uint16_t &len, uint8_t cmd, uint8_t addr,
                 uint8_t devType, const char *info, uint16_t infoLen, uint8_t ver)
{
    uint16_t packLen = sizeof(PMBusPackage_t) + infoLen;
    PMBusPackage_t *pPack = (PMBusPackage_t*)malloc(packLen);
    memset(pPack, 0, packLen);
    unsigned char *pPMPackData = (unsigned char*)pPack;

    pPack->head = PMBUS_HEAD;
    pPack->ver = ver;
    pPack->addr = addr;
    pPack->cid1 = devType;
    pPack->cid2 = cmd;

    if (infoLen)
    {
        pPack->len += infoLen*2;
        pPack->len += this->calLenCrc(infoLen*2) * (0xfff+1);//长度校验码
        pPack->len = htons(pPack->len);//大端发送
        memcpy(pPack->info, info, infoLen);
    }

    this->hex2Ascii((unsigned char*)data+1, pPMPackData+1, packLen-2);

    int asciiLen = (packLen-2)*2 + 2;
    uint16_t crc16 = this->calCrc16((unsigned char*)data+1, asciiLen-6);//ascii的校验和
    ((PMBusPackage_t *)(pPMPackData+infoLen))->crc = htons(crc16);
    this->hex2Ascii((unsigned char*)data+1, pPMPackData+1, packLen-2);

    len = asciiLen;
    data[0] = PMBUS_HEAD;
    data[asciiLen-1] = PMBUS_END;

    free(pPack);
    return 0;
}

int PMBusProtocol::genPkgGetAnalog(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t group)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_ALG, addr,
                            devType, (char*)&group, sizeof(group));
}

int PMBusProtocol::genPkgGetAlm(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t group)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_ALM, addr,
                            devType, (char*)&group, sizeof(group));
}

int PMBusProtocol::genPkgGetSysInfo(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t group)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_SYSARG, addr,
                            devType, (char*)&group, sizeof(group));
}

int PMBusProtocol::genPkgGetFacInfo(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t ver)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_FACINFO, addr, devType, ver);
}

int PMBusProtocol::genPkgGetVersion(char *data, uint16_t &len, uint8_t addr, uint8_t devType, uint8_t ver)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_VER, addr, devType, ver);
}

int PMBusProtocol::genPkgGetAddress(char *data, uint16_t &len, uint8_t devType, uint8_t ver)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_ADDR, PMBUS_ADDR, devType, ver);
}

int PMBusProtocol::genPkgGetAddress(char *data, uint16_t &len, uint8_t addr,uint8_t devType, uint8_t ver)
{
    return genPMPackage(data, len, PMBUS_CID2_GET_ADDR, addr, devType, ver);
}

int PMBusProtocol::genPkgSetAddress(char *data, uint16_t &len, uint8_t addrOld, uint8_t addrNew, uint8_t devType, uint8_t ver)
{
    return genPMPackage(data, len, PMBUS_CID2_SET_ADDR, addrOld,
                            devType, (const char*)&addrNew, sizeof(addrNew), ver);
}

int PMBusProtocol::genPkgSetSysArg(char *data, uint16_t &len, uint8_t addr, uint8_t devType,
                   uint8_t argType, uint8_t group, uint16_t value)
{
    char info[4] = {0};
    memcpy(info, &group, sizeof(group));
    memcpy(info+1, &argType, sizeof(argType));
    uint16_t tmp = htons(value);
    memcpy(info+2, &tmp, sizeof(value));
    return genPMPackage(data, len, PMBUS_CID2_SET_SYSARG, addr,
                            devType, info, sizeof(info));
}

int PMBusProtocol::getBtySysInfo(btySysInfo_t *pSysInfo)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    uint16_t infoLen = (ntohs(pPMBusPack->len) & 0xFFF)/2;
    if ( infoLen <=0 )
    {
        return -1;
    }

    char *pInfo = (char*)pPMBusPack->info;
    memcpy(pSysInfo, pInfo, infoLen);
    for ( int i=0; i<pSysInfo->arryCount; i++ )
    {
        pSysInfo->baseInfo[i].signalVolMax = ntohs(pSysInfo->baseInfo[i].signalVolMax);
        pSysInfo->baseInfo[i].signalVolMin = ntohs(pSysInfo->baseInfo[i].signalVolMin);
        pSysInfo->baseInfo[i].temMax = ntohs(pSysInfo->baseInfo[i].temMax);
        pSysInfo->baseInfo[i].temMin = ntohs(pSysInfo->baseInfo[i].temMin);
        pSysInfo->baseInfo[i].chargeEleMax = ntohs(pSysInfo->baseInfo[i].chargeEleMax);
        pSysInfo->baseInfo[i].totalVolMax = ntohs(pSysInfo->baseInfo[i].totalVolMax);
        pSysInfo->baseInfo[i].totalVolMin = ntohs(pSysInfo->baseInfo[i].totalVolMin);
    }

    return 0;
}

int PMBusProtocol::dataClone(const char *data, uint16_t len)
{    
    if ( len < (sizeof(PMBusPackage_t)-1)*2 )
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

int PMBusProtocol::ascii2Hex(unsigned char *hexData, const unsigned char *asciiData, uint16_t asciiLen)
{
    for ( uint16_t i=0,j=0; i<asciiLen/2; i++,j+=2 )
    {
        uint8_t hig4 = (asciiData[j]>='A') ? (asciiData[j]-'A'+10) : (asciiData[j]-'0');
        uint8_t low4 = (asciiData[j+1]>='A') ? (asciiData[j+1]-'A'+10) : (asciiData[j+1]-'0');
        hexData[i] = (hig4 << 4) + low4;
    }

    return 0;
}

int PMBusProtocol::PMascii2Hex()
{
    if ( NULL != m_hexData )
    {
        delete[] m_hexData;
    }

    m_dataHexLen = m_dataLen/2 + 1;
    m_hexData = new char[m_dataHexLen];
    this->ascii2Hex((uint8_t*)m_hexData+1, (uint8_t*)m_data+1, m_dataLen-2);
    m_hexData[0] = m_data[0];
    m_hexData[m_dataHexLen-1] = m_data[m_dataLen-1];

    return 0;
}

int PMBusProtocol::hex2Ascii(unsigned char *asciiData, const unsigned char *hexData, uint16_t hexLen)
{
    for ( uint16_t i=0,j=0; i<hexLen; i++,j+=2 )
    {
        uint8_t hig4   = hexData[i] >> 4;
        uint8_t low4   = hexData[i] & 0x0f;
        asciiData[j]   = (hig4 >= 0x0A) ? (hig4-0x0A+'A') : (hig4+'0');
        asciiData[j+1] = (low4 >= 0x0A) ? (low4-0x0A+'A') : (low4+'0');
    }

    return 0;
}

uint16_t PMBusProtocol::calCrc16(const unsigned char *data, uint16_t len)
{
    uint16_t asciiSum = 0;
    for ( int i=0; i<len; i++ )
    {
        asciiSum += data[i];
    }

    asciiSum %= 0xFFFF;  // 取65535的模
    asciiSum ^= 0xFFFF;  // 取反
    asciiSum += 1;       // 加1

    return asciiSum;
}

uint8_t PMBusProtocol::calLenCrc(uint16_t data)
{
    uint16_t infoLen = data;
    uint8_t lenCheckSum = 0;
    for ( int i=0; i<3; i++ )
    {

        lenCheckSum += (infoLen >> (i*4)) & 0xF;
    }

    lenCheckSum &= 0xF;
    lenCheckSum ^= 0xF;
    lenCheckSum += 1;

    lenCheckSum &= 0xF;

    return lenCheckSum;
}

bool PMBusProtocol::isCrcPass()
{
    uint16_t *pCheckSum = (uint16_t *)&m_hexData[m_dataHexLen-3];
    uint16_t asciiSum = 0;

    asciiSum = this->calCrc16((unsigned char*)m_data+1, m_dataLen-6);
    if ( asciiSum != ntohs(*pCheckSum) )
    {
        qDebug() << "sum1:" << asciiSum << "sum2:" << *pCheckSum << "sum3:" << ntohs(*pCheckSum);
        return false;
    }

    return true;
}

bool PMBusProtocol::isLenCrcPass()
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    uint16_t infoLen = ntohs(pPMBusPack->len);
    uint8_t lenCheckSum = this->calLenCrc(infoLen);
    if (lenCheckSum != ((ntohs(pPMBusPack->len) >> (3*4)) & 0xF) )
    {
        qDebug() << lenCheckSum << ((ntohs(pPMBusPack->len) >> (3*4)) & 0xF);
        return false;
    }

    return true;
}

bool PMBusProtocol::isFormatPass()
{
    char *pStart = m_data;
    char *pEnd = m_data + m_dataLen - 1;
    while ( (unsigned int)(pEnd-pStart+1) >= (sizeof(PMBusPackage_t)-1)*2 )
    {
        if ( *pStart == PMBUS_HEAD && *pEnd == PMBUS_END )
        {
            char tmpBuff[pEnd-pStart+1];
            memcpy(tmpBuff, pStart, sizeof(tmpBuff));
            this->dataClone(tmpBuff, sizeof(tmpBuff));
            return true;
        }
        pStart++;
    }

    return false;
/*


    if ( m_data[0] != PMBUS_HEAD || m_data[m_dataLen-1] != PMBUS_END )
    {
        return false;
    }

    return true;
*/
}

char *PMBusProtocol::getAnaValPtr(int type, int group)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    uint16_t infoLen = (ntohs(pPMBusPack->len) & 0xFFF)/2;
    if ( infoLen <=0 )
    {
        return NULL;
    }

    char *pInfo = (char*)pPMBusPack->info;
    char *pStart = pInfo;
    char *typePointer = NULL;

    typePointer = pStart;
    uint8_t *btyCount = (uint8_t*)pStart;
    pStart += sizeof(uint8_t);
    if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_BTY_COUNT == type )
    {
        return typePointer;
    }

    for ( uint8_t i=1; i<=*btyCount; i++ )
    {
        typePointer = pStart;
        uint8_t *volCount = (uint8_t*)pStart;
        pStart += sizeof(uint8_t);
        if ( (pStart-pInfo) >infoLen )
        {
            return NULL;
        }

        pStart += (*volCount) * sizeof(uint16_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_VOL == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        uint8_t *temCount = (uint8_t*)pStart;
        pStart += sizeof(uint8_t);
        if ( (pStart-pInfo) >infoLen )
        {
            return NULL;
        }

        pStart += (*temCount) * sizeof(uint16_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_TEM == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        pStart += sizeof(uint16_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_GROUP_ELE == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        pStart += sizeof(uint16_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_GROUP_VOL == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        pStart += sizeof(uint16_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_GROUP_POW == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        pStart += sizeof(uint16_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_USR == type && i == group )
        {
            return (char*)typePointer;
        }
    }

    return NULL;
}

char *PMBusProtocol::getAlmValPtr(int type, int group)
{
    PMBusPackage_t *pPMBusPack = (PMBusPackage_t *)m_hexData;
    uint16_t infoLen = (ntohs(pPMBusPack->len) & 0xFFF)/2;
    if ( infoLen <=0 )
    {
        return NULL;
    }

    char *pInfo = (char*)pPMBusPack->info;
    char *pStart = pInfo;
    char *typePointer = NULL;

    typePointer = pStart;
    uint8_t *btyCount = (uint8_t*)pStart;
    pStart += sizeof(uint8_t);
    if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_BTY_COUNT == type )
    {
        return typePointer;
    }

    for ( uint8_t i=1; i<=*btyCount; i++ )
    {
        typePointer = pStart;
        uint8_t *volCount = (uint8_t*)pStart;
        pStart += sizeof(uint8_t);
        if ( (pStart-pInfo) >infoLen )
        {
            return NULL;
        }

        pStart += (*volCount) * sizeof(uint8_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_VOL == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        uint8_t *temCount = (uint8_t*)pStart;
        pStart += sizeof(uint8_t);
        if ( (pStart-pInfo) >infoLen )
        {
            return NULL;
        }

        pStart += (*temCount) * sizeof(uint8_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_TEM == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        pStart += sizeof(uint8_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_GROUP_ELE == type && i == group )
        {
            return (char*)typePointer;
        }

        typePointer = pStart;
        pStart += sizeof(uint8_t);
        if ( (pStart-pInfo) <= infoLen && POINTER_TYPE_GROUP_VOL == type && i == group )
        {
            return (char*)typePointer;
        }

        pStart += sizeof(uint8_t);//自定义字节数

    }

    return NULL;
}
