#ifndef TP02_MFSU_TYPE_H
#define TP02_MFSU_TYPE_H

#include <stdint.h>
#include <QMap>

#define TP02_EXPORT_MFSU_CHN_CFG         0xA9
#define TP02_IMPORT_MFSU_CHN_CFG         0xAA
#define TP02_CHN_AI_GET_DATA             0xAB
#define TP02_CHN_DI_GET_DATA             0xAC
#define TP02_CHN_DO_CONTROL              0xAD
#define TP02_UART_TEST                   0xAE
#define TP02_NBIOT_TEST                  0xAF

#define TP02_GET_DEV_INFO                0x05

#define TP02_GET_POWER_TYPE              0x06
#define TP02_SET_POWER_TYPE              0x07

#define TP02_GET_ELE_DATA                0x08
#define TP02_CLEAR_ELE_DATA              0x09

#define TP02_EXPORT_SERIAL_CFG           0x0A
#define TP02_IMPORT_SERIAL_CFG           0x0B

#define TP02_IMPORT_LIB_START            0x0C
#define TP02_IMPORT_LIB_DATA             0x0D
#define TP02_IMPORT_LIB_END              0x0E

#define TP02_EXPORT_METER_CAL_ARGS       0x0F
#define TP02_IMPORT_METER_CAL_ARGS       0x10

#define TP02_GET_BETTARY_INFO            0x11
#define TP02_GET_WORK_STATUS             0x12

#define TP02_SET_FSU_ID                  0x13
#define TP02_GET_FSU_ID                  0x14

#define TP02_GET_CONNECT_STATES          0x15

#pragma pack(1)
typedef struct Tp02BettaryInfo
{
    int16_t vol;
    int16_t temp;
    uint8_t heatStatus;
    uint8_t chargeStatus;
}Tp02BettaryInfo_t;

typedef struct TP02SingleAIData
{
    uint8_t id;
    int32_t data;
}TP02SingleAIData_t;

typedef struct TP02AIData
{
    uint8_t count;
    TP02SingleAIData_t data[1];//实际为柔性数组(data[])，解决编译器警告处理
}TP02AIData_t;

typedef struct TP02SingleDIData
{
    uint8_t id;
    uint8_t data;
}TP02SingleDIData_t;

typedef struct TP02DIData
{
    uint8_t count;
    TP02SingleDIData_t data[1];//实际为柔性数组(data[])，解决编译器警告处理
}TP02DIData_t;

typedef struct TP02EleData
{
    int32_t vol;
    int32_t cur;
    int32_t pow;
    uint32_t freq;
    int32_t eleToday;
    int32_t eleTotal;
}TP02EleData_t;
#pragma pack()

#endif // TP02_MFSU_TYPE_H
