#include "mcp_can_dfs.h"

// CAN config
#define CAN0Speed           CAN_250KBPS
#define CAN0IntPin          2
#define CAN0CS              53

#define CAN1Speed           CAN_500KBPS
#define CAN1IntPin          3
#define CAN1CS              48

#define shutdownPIN         4

// Setup parameters
#define BMS0ID              0x12C
#define chargerIDSend       0x1806E7F4
#define chargerIDRecv       0x18FF50E7
#define shuntVoltagemV      0
#define maxChargeCurrent    10  // 1 amp
#define maxChargeVoltage    105 // 120 volts
#define chargeIfPossible    1   // 0 = don't charge
#define defaultINT          -41 // Default value for debugging system connections (should be <0)
#define maxAllowedCellV     4205
#define minAllowedCellV     2795
#define maxAllowedCellT     40

// CANMsg structure for buffer in main program
struct CANMsg
{
    INT32U id;
    INT8U  len;
    INT8U  buf[8];
    int    bus;
};

// Data structures (see datos.json to understand how they are organized)
struct BMSData
{
    int connected;
    int cellVoltagemV[12];
    int temperatures[2];
};

struct SEVCONData
{
    int   connected;
    // TPDO 1
    float target_id;
    float id;
    float target_iq;
    float iq;

    // TPDO 2
    float battery_voltage;
    float battery_current;
    float line_contactor;
    float capacitor_voltage;

    // TPDO 3
    float throttle_value;
    float target_torque;
    float torque;

    // TPDO 4
    float heatsink_temp;

    // TPDO 5
    float maximum_motor_speed;
    float velocity;
};

struct CHARGERData
{
    int connected;
    int Vtotal;
    int Icharge;
    int flags[5];
};

struct Data
{
    int                allOK;
    struct BMSData     BMS[3];
    struct SEVCONData  SEVCON;
    struct CHARGERData CHARGER;
};

/*
 * writeData: Output all data for debugging
 */
void writeData(struct Data data)
{
    Serial.println("Data:");
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
    //sprintf(buffer, "\"line_contactor\": %.1f\n", data.SEVCON.line_contactor);
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

    sprintf(buffer, "CONNECTIONS - BMS: %d %d %d, SEVCON: %d, CHARGER: %d", data.BMS[0].connected, data.BMS[1].connected, data.BMS[2].connected, data.SEVCON.connected, data.CHARGER.connected);
    Serial.println(buffer);
}
