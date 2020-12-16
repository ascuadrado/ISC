#include <time.h>
#include <stdio.h>
#include "mcp_can_dfs_rpi.h"

// CAN config
#define CAN0Speed     CAN_250KBPS
#define CAN0IntPin    25
#define CAN0CS        8

#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    24
#define CAN1CS        7

// Setup parameters
#define chargerID     0x1806E7F4

// File saving
char fileName[128] = "datos.json";

/*
 * Data structures
 * ----------------------------
 */

struct CANMsg
{
    INT32U id;
    INT8U  len;
    INT8U  buf;
    int    bus;
};

struct BMSData
{
    int cellVoltagemV[12]        = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int cellVoltagemVUpdated[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int temperatures[2]          = { 0, 0 };
    int temperaturesUpdated[2]   = { 0, 0 };
};

struct SEVCONData
{
    // TPDO 1
    unsigned short int TPDO1_1        = 0;
    unsigned short int TPDO1_1Updated = 0;
    unsigned short int TPDO1_2        = 0;
    unsigned short int TPDO1_2Updated = 0;
    unsigned short int TPDO1_3        = 0;
    unsigned short int TPDO1_3Updated = 0;
    unsigned short int TPDO1_4        = 0;
    unsigned short int TPDO1_4Updated = 0;

    // TPDO 2
    unsigned short int TPDO2_1        = 0;
    unsigned short int TPDO2_1Updated = 0;
    unsigned short int TPDO2_2        = 0;
    unsigned short int TPDO2_2Updated = 0;
    unsigned short int TPDO2_3        = 0;
    unsigned short int TPDO2_3Updated = 0;

    // TPDO 3
    unsigned short int TPDO3_1        = 0;
    unsigned short int TPDO3_1Updated = 0;
    unsigned short int TPDO3_2        = 0;
    unsigned short int TPDO3_2Updated = 0;
    unsigned short int TPDO3_3        = 0;
    unsigned short int TPDO3_3Updated = 0;
    unsigned short int TPDO3_4        = 0;
    unsigned short int TPDO3_4Updated = 0;

    // TPDO 4
    unsigned short int TPDO4_1        = 0;
    unsigned short int TPDO4_1Updated = 0;
    unsigned short int TPDO4_2        = 0;
    unsigned short int TPDO4_2Updated = 0;
    unsigned short int TPDO4_3        = 0;
    unsigned short int TPDO4_3Updated = 0;
    unsigned short int TPDO4_4        = 0;
    unsigned short int TPDO4_4Updated = 0;

    // TPDO 5
    INT32U             TPDO5_1        = 0;
    unsigned short int TPDO5_1Updated = 0;
    INT32U             TPDO5_2        = 0;
    unsigned short int TPDO5_2Updated = 0;
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


void writeData(struct Data data)
{
    printf("Writing to file!\n");

    FILE   *filePointer;
    time_t timestamp;

    time(&timestamp);

    filePointer = fopen(fileName, "w");

    fprintf(filePointer, "{\n");
    fprintf(filePointer, "\"data\": {\n");
    fprintf(filePointer, "\"timeStamp\": %ld,\n", timestamp);
    fprintf(filePointer, "\"allOK\": %d,\n", data.allOK);
    fprintf(filePointer, "\"BMS\": [{\n");
    fprintf(filePointer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[0].cellVoltagemV[0], data.BMS[0].cellVoltagemV[1], data.BMS[0].cellVoltagemV[2], data.BMS[0].cellVoltagemV[3], data.BMS[0].cellVoltagemV[4], data.BMS[0].cellVoltagemV[5], data.BMS[0].cellVoltagemV[6], data.BMS[0].cellVoltagemV[7], data.BMS[0].cellVoltagemV[8], data.BMS[0].cellVoltagemV[9], data.BMS[0].cellVoltagemV[10], data.BMS[0].cellVoltagemV[11]);
    fprintf(filePointer, "\"temperatures\": [%d,%d]\n", data.BMS[0].temperatures[0], data.BMS[0].temperatures[1]);
    fprintf(filePointer, "},{\n");
    fprintf(filePointer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[1].cellVoltagemV[0], data.BMS[1].cellVoltagemV[1], data.BMS[1].cellVoltagemV[2], data.BMS[1].cellVoltagemV[3], data.BMS[1].cellVoltagemV[4], data.BMS[1].cellVoltagemV[5], data.BMS[1].cellVoltagemV[6], data.BMS[1].cellVoltagemV[7], data.BMS[1].cellVoltagemV[8], data.BMS[1].cellVoltagemV[9], data.BMS[1].cellVoltagemV[10], data.BMS[1].cellVoltagemV[11]);
    fprintf(filePointer, "\"temperatures\": [%d,%d]\n", data.BMS[1].temperatures[0], data.BMS[1].temperatures[1]);
    fprintf(filePointer, "},{\n");
    fprintf(filePointer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[2].cellVoltagemV[0], data.BMS[2].cellVoltagemV[1], data.BMS[2].cellVoltagemV[2], data.BMS[2].cellVoltagemV[3], data.BMS[2].cellVoltagemV[4], data.BMS[2].cellVoltagemV[5], data.BMS[2].cellVoltagemV[6], data.BMS[2].cellVoltagemV[7], data.BMS[2].cellVoltagemV[8], data.BMS[2].cellVoltagemV[9], data.BMS[2].cellVoltagemV[10], data.BMS[2].cellVoltagemV[11]);
    fprintf(filePointer, "\"temperatures\": [%d,%d]\n", data.BMS[2].temperatures[0], data.BMS[2].temperatures[1]);
    fprintf(filePointer, "}],\n");
    fprintf(filePointer, "\"SEVCON\": {\n");
    fprintf(filePointer, "\"TPDO1_1\": %d\n", data.SEVCON.TPDO1_1);
    fprintf(filePointer, "\n");
    fprintf(filePointer, "},\n");
    fprintf(filePointer, "\"CHARGER\": {\n");
    fprintf(filePointer, "\"Vtotal\": %d,\n", data.CHARGER.Vtotal);
    fprintf(filePointer, "\"Icharge\": %d,\n", data.CHARGER.Icharge);
    fprintf(filePointer, "\"flags\": [%d, %d, %d, %d, %d]\n", data.CHARGER.flags[0], data.CHARGER.flags[1], data.CHARGER.flags[2], data.CHARGER.flags[3], data.CHARGER.flags[4]);
    fprintf(filePointer, "}\n");
    fprintf(filePointer, "}\n");
    fprintf(filePointer, "}\n");
    fprintf(filePointer, "\n");

    fclose(filePointer);
}
