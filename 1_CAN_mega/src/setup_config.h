// CAN config
#define CAN0Speed     CAN_250KBPS
#define CAN0IntPin    49
#define CAN0CS        53

#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    46
#define CAN1CS        48

// Setup parameters
#define chargerID     0x1806E7F4

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
    int TPDO1_1        = 0;
    int TPDO1_1Updated = 0;
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
