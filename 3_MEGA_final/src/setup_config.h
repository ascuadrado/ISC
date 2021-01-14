#include "mcp_can_dfs.h"

// CAN config
#define CAN0Speed     CAN_250KBPS
#define CAN0IntPin    2
#define CAN0CS        53

#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    3
#define CAN1CS        48

#define ARD1          4
#define ARD2          5


// Setup parameters
#define BMS0ID              0x12C
#define chargerIDSend       0x1806E7F4
#define chargerIDRecv       0x18FF50E7
#define shuntVoltagemV      0    // 0 for no balancing or balancing voltage in mV
#define maxChargeCurrent    10   // amps
#define maxChargeVoltage    105  // volts
#define minChargeVoltage    70   // volts
#define maxAllowedCellV     4205 // millivolts
#define minAllowedCellV     2795 // millivolts
#define maxAllowedCellT     40   // Celsius º
#define lineContactorUP     1000 // CAN msg for line contactor up
#define startupTime         5000 // [ms] Time to leave standBy mode

// Estados posibles
enum Estado
{
    standBy,
    run,
    charge,
    error
};

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
    int    connected;
    // TPDO 1
    double target_id;
    double id;
    double target_iq;
    double iq;

    // TPDO 2
    double battery_voltage;
    double battery_current;
    double line_contactor;
    double capacitor_voltage;

    // TPDO 3
    double throttle_value;
    double target_torque;
    double torque;

    // TPDO 4
    double heatsink_temp;

    // TPDO 5
    double maximum_motor_speed;
    double velocity;
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
    int                batteryOK; // 0 for error, 1 for OK
    int                runButton;
    int                tryToCharge;
    struct BMSData     BMS[3];
    struct SEVCONData  SEVCON;
    struct CHARGERData CHARGER;
};

/*
 * checkBMSData: check if all cells are fine
 */
int checkBMSData(struct Data data)
{
    // allOK = 0 means something is wrong
    int OK = 1;

    int  minV = data.BMS[0].cellVoltagemV[0];
    int  maxV = minV;
    int  minT = data.BMS[0].temperatures[0];
    int  maxT = minT;
    long sumV = 0;

    // Check if are BMS connected & calculate maxV, minV, maxT, minT, sumV
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            int v = data.BMS[i].cellVoltagemV[j];
            if (((i == 1) && (j >= 8)) || ((i == 2) && (j >= 10)))
            {
                // We don't care about these values because they are not connected
                continue;
            }
            sumV += v;
            if (v < minV)
            {
                minV = v;
            }
            else if (v > maxV)
            {
                maxV = v;
            }
        }
        for (int j = 0; j < 2; j++)
        {
            int t = data.BMS[i].temperatures[j];
            if (t < minT)
            {
                minT = t;
            }
            else if (t > maxT)
            {
                maxT = t;
            }
        }
    }

    // Check if anything is out of limits
    if ((minV < minAllowedCellV) || (maxV > maxAllowedCellV) || (maxT > maxAllowedCellT))
    {
        OK = 0;
    }


    if (OK)
    {
        // All good
    }
    else
    {
        // Something is wrong
    }

    return OK;
}


/*
 * updateState: take decisions regarding finie state machine
 */
Estado updateState(struct Data data, Estado estado)
{
    int    ard1pinstate = 0; // 0 means open, 1 closed
    int    ard2pinstate = 0; // 0 means open, 1 closed
    Estado nuevoEstado  = error;

    switch (estado)
    {
    case standBy:
        ard1pinstate = 1;
        ard2pinstate = 0;

        if (millis() > startupTime)
        {
            // Enter run or charge or error mode
            if (data.SEVCON.connected && data.CHARGER.connected)
            {
                estado = error;
            }
            else if (data.SEVCON.connected)
            {
                estado = run;
            }
            else if (data.CHARGER.connected)
            {
                estado = charge;
            }
        }
        break;

    case run:
        ard1pinstate = 1;
        if (data.SEVCON.connected && (data.SEVCON.line_contactor > lineContactorUP / 2))
        {
            ard2pinstate = 1;
        }
        else
        {
            ard2pinstate = 0;
        }
        break;

    case charge:
        ard1pinstate = 1;
        if (data.CHARGER.connected && (data.CHARGER.Vtotal > minChargeVoltage * 0.95) && (data.CHARGER.Vtotal < maxChargeVoltage * 1.05))
        {
            ard2pinstate     = 1;
            data.tryToCharge = 1;
        }
        else
        {
            ard2pinstate     = 0;
            data.tryToCharge = 0;
        }
        break;

    case error:
        ard1pinstate = 0;
        ard2pinstate = 0;
        break;
    }

    if (!data.batteryOK) // If battery not OK straight to error
    {
        ard1pinstate = 0;
        ard2pinstate = 0;
        nuevoEstado  = error;
    }

    digitalWrite(ARD1, ard1pinstate);
    digitalWrite(ARD2, ard2pinstate);
    return nuevoEstado;
}


/*
 * writeData: Output all data for debugging
 */
void debugInfo(struct Data data, Estado estado)
{
    Serial.println("Data:\n");

    char buffer[256];

    sprintf(buffer, "{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"batteryOK\": %d,\n", data.batteryOK);
    Serial.print(buffer);
    sprintf(buffer, "\"runButton\": %d,\n", data.runButton);
    Serial.print(buffer);
    sprintf(buffer, "\"tryToCharge\": %d,\n", data.tryToCharge);
    Serial.print(buffer);

    sprintf(buffer, "\"BMS\": [{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"connected\": %d,\n", data.BMS[0].connected);
    Serial.print(buffer);
    sprintf(buffer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[0].cellVoltagemV[0], data.BMS[0].cellVoltagemV[1], data.BMS[0].cellVoltagemV[2], data.BMS[0].cellVoltagemV[3], data.BMS[0].cellVoltagemV[4], data.BMS[0].cellVoltagemV[5], data.BMS[0].cellVoltagemV[6], data.BMS[0].cellVoltagemV[7], data.BMS[0].cellVoltagemV[8], data.BMS[0].cellVoltagemV[9], data.BMS[0].cellVoltagemV[10], data.BMS[0].cellVoltagemV[11]);
    Serial.print(buffer);
    sprintf(buffer, "\"temperatures\": [%d,%d]\n", data.BMS[0].temperatures[0], data.BMS[0].temperatures[1]);
    Serial.print(buffer);
    sprintf(buffer, "},{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"connected\": %d,\n", data.BMS[1].connected);
    Serial.print(buffer);
    sprintf(buffer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[1].cellVoltagemV[0], data.BMS[1].cellVoltagemV[1], data.BMS[1].cellVoltagemV[2], data.BMS[1].cellVoltagemV[3], data.BMS[1].cellVoltagemV[4], data.BMS[1].cellVoltagemV[5], data.BMS[1].cellVoltagemV[6], data.BMS[1].cellVoltagemV[7], data.BMS[1].cellVoltagemV[8], data.BMS[1].cellVoltagemV[9], data.BMS[1].cellVoltagemV[10], data.BMS[1].cellVoltagemV[11]);
    Serial.print(buffer);
    sprintf(buffer, "\"temperatures\": [%d,%d]\n", data.BMS[1].temperatures[0], data.BMS[1].temperatures[1]);
    Serial.print(buffer);
    sprintf(buffer, "},{\n");
    Serial.print(buffer);
    sprintf(buffer, "\"connected\": %d,\n", data.BMS[2].connected);
    Serial.print(buffer);
    sprintf(buffer, "\"cellVoltagemV\": [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],\n", data.BMS[2].cellVoltagemV[0], data.BMS[2].cellVoltagemV[1], data.BMS[2].cellVoltagemV[2], data.BMS[2].cellVoltagemV[3], data.BMS[2].cellVoltagemV[4], data.BMS[2].cellVoltagemV[5], data.BMS[2].cellVoltagemV[6], data.BMS[2].cellVoltagemV[7], data.BMS[2].cellVoltagemV[8], data.BMS[2].cellVoltagemV[9], data.BMS[2].cellVoltagemV[10], data.BMS[2].cellVoltagemV[11]);
    Serial.print(buffer);
    sprintf(buffer, "\"temperatures\": [%d,%d]\n", data.BMS[2].temperatures[0], data.BMS[2].temperatures[1]);
    Serial.print(buffer);
    sprintf(buffer, "}],\n");
    Serial.print(buffer);

    sprintf(buffer, "\"SEVCON\": {\n");
    Serial.print(buffer);
    sprintf(buffer, "\"connected\": %d,\n", data.SEVCON.connected);
    Serial.print(buffer);
    sprintf(buffer, "\"target_id\": %.2f,\n", data.SEVCON.target_id);
    Serial.print(buffer);
    sprintf(buffer, "\"id\": %.2f,\n", data.SEVCON.id);
    Serial.print(buffer);
    sprintf(buffer, "\"target_iq\": %.2f,\n", data.SEVCON.target_iq);
    Serial.print(buffer);
    sprintf(buffer, "\"iq\": %.2f,\n", data.SEVCON.iq);
    Serial.print(buffer);
    sprintf(buffer, "\"battery_voltage\": %.2f,\n", data.SEVCON.battery_voltage);
    Serial.print(buffer);
    sprintf(buffer, "\"battery_current\": %.2f,\n", data.SEVCON.battery_current);
    Serial.print(buffer);
    sprintf(buffer, "\"line_contactor\": %.2f,\n", data.SEVCON.line_contactor);
    Serial.print(buffer);
    sprintf(buffer, "\"capacitor_voltage\": %.2f,\n", data.SEVCON.capacitor_voltage);
    Serial.print(buffer);
    sprintf(buffer, "\"throttle_value\": %.2f,\n", data.SEVCON.throttle_value);
    Serial.print(buffer);
    sprintf(buffer, "\"target_torque\": %.2f,\n", data.SEVCON.target_torque);
    Serial.print(buffer);
    sprintf(buffer, "\"torque\": %.2f,\n", data.SEVCON.torque);
    Serial.print(buffer);
    sprintf(buffer, "\"heatsink_temp\": %.2f,\n", data.SEVCON.heatsink_temp);
    Serial.print(buffer);
    sprintf(buffer, "\"maximum_motor_speed\": %.2f,\n", data.SEVCON.maximum_motor_speed);
    Serial.print(buffer);
    sprintf(buffer, "\"velocity\": %.2f,\n", data.SEVCON.velocity);
    Serial.print(buffer);

    sprintf(buffer, "\n");
    Serial.print(buffer);
    sprintf(buffer, "},\n");
    Serial.print(buffer);
    sprintf(buffer, "\"CHARGER\": {\n");
    Serial.print(buffer);
    sprintf(buffer, "\"iq\": %d,\n", data.CHARGER.connected);
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

    sprintf(buffer, "CONNECTIONS - BMS: %d %d %d, SEVCON: %d, CHARGER: %d", data.BMS[0].connected, data.BMS[1].connected, data.BMS[2].connected, data.SEVCON.connected, data.CHARGER.connected);
    Serial.println(buffer);

    switch (estado)
    {
    case standBy:
        sprintf(buffer, "STANDBY MODE");
        break;

    case run:
        sprintf(buffer, "RUN MODE");
        break;

    case charge:
        sprintf(buffer, "CHARGE MODE");
        break;

    case error:
        sprintf(buffer, "ERROR MODE");
        break;
    }
    Serial.println(buffer);
}
