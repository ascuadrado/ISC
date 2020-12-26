#include "mcp_can_dfs.h"

// CAN config
#define CAN0Speed           CAN_250KBPS
#define CAN0IntPin          2
#define CAN0CS              53

#define CAN1Speed           CAN_500KBPS
#define CAN1IntPin          3
#define CAN1CS              48

// Setup parameters
#define BMS0ID              0x12C
#define chargerID           0x1806E7F4
#define shuntVoltagemV      0
#define maxChargeCurrent    1   // 1 amp
#define maxChargeVoltage    120 // 120 volts
#define chargeIfPossible    0   // 0 = don't charge

struct CANMsg
{
    INT32U id;
    INT8U  len;
    INT8U  buf[8];
    int    bus;
};

struct BMSData
{
    int cellVoltagemV[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int temperatures[2]   = { 0, 0 };
};

struct SEVCONData
{
    // TPDO 1
    int  TPDO1_1 = 0;
    int  TPDO1_2 = 0;
    int  TPDO1_3 = 0;
    int  TPDO1_4 = 0;

    // TPDO 2
    int  TPDO2_1 = 0;
    int  TPDO2_2 = 0;
    int  TPDO2_3 = 0;

    // TPDO 3
    int  TPDO3_1 = 0;
    int  TPDO3_2 = 0;
    int  TPDO3_3 = 0;
    int  TPDO3_4 = 0;

    // TPDO 4
    int  TPDO4_1 = 0;
    int  TPDO4_2 = 0;
    int  TPDO4_3 = 0;
    int  TPDO4_4 = 0;

    // TPDO 5
    long TPDO5_1 = 0;
    long TPDO5_2 = 0;
};

struct CHARGERData
{
    int Vtotal   = 0;
    int Icharge  = 0;
    int flags[5] = { 0, 0, 0, 0, 0 };
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

    char buffer[256];

    sprintf(buffer, "{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"data\": {\n");
//    sprintf(buffer, "\"timeStamp\": %ld,\n", timestamp);
    sprintf(buffer, "\"allOK\": %d,\n", data.allOK);
    Serial.print(buffer);
    sprintf(buffer, "\"BMS\": [{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[0].cellVoltagemV[0], data.BMS[0].cellVoltagemV[1], data.BMS[0].cellVoltagemV[2], data.BMS[0].cellVoltagemV[3], data.BMS[0].cellVoltagemV[4], data.BMS[0].cellVoltagemV[5], data.BMS[0].cellVoltagemV[6], data.BMS[0].cellVoltagemV[7], data.BMS[0].cellVoltagemV[8], data.BMS[0].cellVoltagemV[9], data.BMS[0].cellVoltagemV[10], data.BMS[0].cellVoltagemV[11]);
    Serial.print(buffer);
    sprintf(buffer, "\"temperatures\": [%d,%d]\n", data.BMS[0].temperatures[0], data.BMS[0].temperatures[1]);
    Serial.print(buffer);
    sprintf(buffer, "},{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[1].cellVoltagemV[0], data.BMS[1].cellVoltagemV[1], data.BMS[1].cellVoltagemV[2], data.BMS[1].cellVoltagemV[3], data.BMS[1].cellVoltagemV[4], data.BMS[1].cellVoltagemV[5], data.BMS[1].cellVoltagemV[6], data.BMS[1].cellVoltagemV[7], data.BMS[1].cellVoltagemV[8], data.BMS[1].cellVoltagemV[9], data.BMS[1].cellVoltagemV[10], data.BMS[1].cellVoltagemV[11]);
    Serial.print(buffer);
    sprintf(buffer, "\"temperatures\": [%d,%d]\n", data.BMS[1].temperatures[0], data.BMS[1].temperatures[1]);
    Serial.print(buffer);
    sprintf(buffer, "},{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[2].cellVoltagemV[0], data.BMS[2].cellVoltagemV[1], data.BMS[2].cellVoltagemV[2], data.BMS[2].cellVoltagemV[3], data.BMS[2].cellVoltagemV[4], data.BMS[2].cellVoltagemV[5], data.BMS[2].cellVoltagemV[6], data.BMS[2].cellVoltagemV[7], data.BMS[2].cellVoltagemV[8], data.BMS[2].cellVoltagemV[9], data.BMS[2].cellVoltagemV[10], data.BMS[2].cellVoltagemV[11]);
    Serial.print(buffer);
    sprintf(buffer, "\"temperatures\": [%d,%d]\n", data.BMS[2].temperatures[0], data.BMS[2].temperatures[1]);
    Serial.print(buffer);
    sprintf(buffer, "}],\n");
    Serial.print(buffer);
    sprintf(buffer, "\"SEVCON\": {\n");
    Serial.print(buffer);
    sprintf(buffer, "\"TPDO1_1\": %d\n", data.SEVCON.TPDO1_1);
    Serial.print(buffer);
    sprintf(buffer, "\n");
    Serial.print(buffer);
    sprintf(buffer, "},\n");
    Serial.print(buffer);
    sprintf(buffer, "\"CHARGER\": {\n");
    Serial.print(buffer);
    sprintf(buffer, "\"Vtotal\": %d,\n", data.CHARGER.Vtotal);
    Serial.print(buffer);
    sprintf(buffer, "\"Icharge\": %d,\n", data.CHARGER.Icharge);
    Serial.print(buffer);
    sprintf(buffer, "\"flags\": [%d, %d, %d, %d, %d]\n", data.CHARGER.flags[0], data.CHARGER.flags[1], data.CHARGER.flags[2], data.CHARGER.flags[3], data.CHARGER.flags[4]);
    Serial.print(buffer);
    sprintf(buffer, "}\n");
    Serial.print(buffer);
    sprintf(buffer, "}\n");
    Serial.print(buffer);
    sprintf(buffer, "}\n");
    Serial.print(buffer);
    sprintf(buffer, "\n");
    Serial.print(buffer);

// Summary
    int               max  = 0;
    int               min  = 5000;
    long unsigned int sum  = 0;
    int               temp = data.BMS[1].temperatures[0];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            int v = data.BMS[i].cellVoltagemV[j];
            if (((i == 1) && (j >= 8)) || ((i == 2) && (j >= 10)))
            {
                continue;
            }
            sum += v;
            if (v < min)
            {
                min = v;
            }
            if (v > max)
            {
                max = v;
            }
        }
    }

    if ((min < 3200) || (max > 4000) || (temp > 35))
    {
        Serial.println("ALERT, something is wrong!!!");
        Serial.println("-----------------------------");
    }
    sprintf(buffer, "Max: %d, Min: %d, Total: %lu, temp: %d \n", max, min, sum, temp);
    Serial.print(buffer);
}
