#include "mcp_can_dfs.h"

// CAN config
#define CAN0Speed         CAN_250KBPS
#define CAN0IntPin        49
#define CAN0CS            53

#define CAN1Speed         CAN_500KBPS
#define CAN1IntPin        46
#define CAN1CS            48

// Setup parameters
#define chargerID         0x1806E7F4
#define shuntVoltagemV    0

// Data structures
struct BMSData
{
    int cellVoltagemV[12]        = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cellVoltagemVUpdated[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int temperatures[2]          = { 0, 0 };
    int temperaturesUpdated[2]   = { 0, 0 };
};

struct SEVCONData
{
    unsigned short int TPDO1_1        = 0;
    int                TPDO1_1Updated = 0;
    unsigned short int TPDO1_2        = 0;
    int                TPDO1_2Updated = 0;
    unsigned short int TPDO1_3        = 0;
    int                TPDO1_3Updated = 0;
    unsigned short int TPDO1_4        = 0;
    int                TPDO1_4Updated = 0;

    unsigned short int TPDO2_1        = 0;
    int                TPDO2_1Updated = 0;
    unsigned short int TPDO2_2        = 0;
    int                TPDO2_2Updated = 0;
    unsigned short int TPDO2_3        = 0;
    int                TPDO2_3Updated = 0;

    unsigned short int TPDO3_1        = 0;
    int                TPDO3_1Updated = 0;
    unsigned short int TPDO3_2        = 0;
    int                TPDO3_2Updated = 0;
    unsigned short int TPDO3_3        = 0;
    int                TPDO3_3Updated = 0;
    unsigned short int TPDO3_4        = 0;
    int                TPDO3_4Updated = 0;

    unsigned short int TPDO4_1        = 0;
    int                TPDO4_1Updated = 0;
    unsigned short int TPDO4_2        = 0;
    int                TPDO4_2Updated = 0;
    unsigned short int TPDO4_3        = 0;
    int                TPDO4_3Updated = 0;
    unsigned short int TPDO4_4        = 0;
    int                TPDO4_4Updated = 0;

    INT32U             TPDO5_1        = 0;
    int                TPDO5_1Updated = 0;
    INT32U             TPDO5_2        = 0;
    int                TPDO5_2Updated = 0;
};

struct CHARGERData
{
    int Vtotal          = 0;
    int VtotalUpdated   = 0;
    int Icharge         = 0;
    int IchargeUpdated  = 0;
    int flags[5]        = { 0, 0, 0, 0, 0 };
    int flagsUpdated[5] = { 0, 0, 0, 0, 0 };
};

struct Data
{
    int                allOK;
    struct BMSData     BMS[3];
    struct SEVCONData  SEVCON;
    struct CHARGERData CHARGER;
};
