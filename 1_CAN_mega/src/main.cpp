// Libraries
#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>

#include "setup_config.h"

// CAN instances
MCP_CAN CAN0(CAN0CS);
MCP_CAN CAN1(CAN1CS);

// Timer help
int startMillis0 = 0;
int startMillis1 = 0;
int period0      = 1000;
int period1      = 10000;

// Functions
void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN);
void checkData();
void printData();

// Data structures
struct Data data;

/*
 * Main program
 */
void setup()
{
    Serial.begin(115200);

    Serial.println("Arduino MEGA. 2 CAN buses, same SPI");

    while (CAN0.begin(MCP_ANY, CAN0Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN");
        delay(1000);
    }

    while (CAN1.begin(MCP_ANY, CAN1Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN");
        delay(1000);
    }

    CAN0.setMode(MCP_NORMAL);
    CAN1.setMode(MCP_NORMAL);

    Serial.print("CAN ready: CAN0 - ");
    Serial.print(CAN0Speed);
    Serial.print(" , CAN1 - ");
    Serial.print(CAN1Speed);
}


void loop()
{
    int currentMillis = millis();

    if (currentMillis - startMillis0 >= period0) // Timer0
    {
        INT32U id       = 0x12C;
        INT8U  ext      = 1;
        INT8U  len      = 2;
        INT8U  buf[len] = { 0, 0 };

        CAN0.sendMsgBuf(id, ext, len, buf);

        CAN1.sendMsgBuf(id, ext, len, buf);

        startMillis0 = currentMillis;
    }

    if (currentMillis - startMillis1 >= period1) // Timer1
    {
        checkData();

        startMillis1 = currentMillis;
    }

    if (!digitalRead(CAN0IntPin)) // New message on CAN 0
    {
        INT32U id;
        INT8U  len = 0;
        INT8U  buf[8];

        CAN0.readMsgBuf(&id, &len, buf);
        parseMessage(id, len, buf, 0);
    }

    if (!digitalRead(CAN1IntPin)) // New message on CAN 1
    {
        INT32U id;
        INT8U  len = 0;
        INT8U  buf[8];

        CAN0.readMsgBuf(&id, &len, buf);
        parseMessage(id, len, buf, 1);
    }
}


void parseMessage(INT32U id, INT8U len, INT8U *buf, int busN)
{
    id = id & 0x1FFFFFFF;

    if (busN == 0)
    {
        // Charger
        if ((id == chargerID))
        {
            // Voltage measured by charger
            data.CHARGER.Vtotal        = ((buf[0] << 8) + buf[1]) * 100;
            data.CHARGER.VtotalUpdated = 1;
            // Instant charging current
            data.CHARGER.Icharge        = ((buf[2] << 8) + buf[3]) * 100;
            data.CHARGER.IchargeUpdated = 1;
            // Charger flags
            int flags = buf[4];
            data.CHARGER.flags[0] = (flags >> 7) & 0x1; // Flag 0
            data.CHARGER.flags[1] = (flags >> 6) & 0x1; // Flag 1
            data.CHARGER.flags[2] = (flags >> 5) & 0x1; // Flag 2
            data.CHARGER.flags[3] = (flags >> 4) & 0x1; // Flag 3
            data.CHARGER.flags[4] = (flags >> 3) & 0x1; // Flag 4
        }

        // BMS Modules
        if ((id > 300) && (id < 300 + 3 * 10))
        {
            // BMS number (1-16)
            int n = (id - 300) / 10;
            // Message number (0-3)
            int m = (id - 300 - n * 10 - 1);

            // Voltage frame
            if (m < 3)
            {
                for (int i = 0; i < 4; i++)
                {
                    // i = number of cell within message
                    data.BMS[n].cellVoltagemV[m * 4 + i]        = (buf[2 * i] << 8) + buf[2 * i + 1];
                    data.BMS[n].cellVoltagemVUpdated[m * 4 + i] = 1;
                }
            }

            // Temperature frame
            else if (m == 3)
            {
                for (int i = 0; i < 2; i++)
                {
                    data.BMS[n].temperatures[i]        = buf[i] - 40;
                    data.BMS[n].temperaturesUpdated[i] = 1;
                }
            }
        }
    }



    // SEVCON controller
    if (busN == 1)
    {
        switch (id)
        {
        case 0x274:
            // TPDO1
            data.SEVCON.TPDO1_1 = (buf[0] << 8) + buf[1];
            break;

        case 0x195:
            // TPDO2

            break;

        case 0x146:
            // TPDO3

            break;

        case 0x168:
            // TPDO4

            break;

        case 0x370:
            // TPDO5

            break;
        }
    }
}


void checkData()
{
    int allOK = 1;

    // Check all BMS data
    for (int i = 0; i < 3; i++)
    {
        // i = num BMS
        int allUpdated = 1;
        for (int j = 0; j < 12; j++)
        {
            allUpdated *= data.BMS[i].cellVoltagemVUpdated[j];
        }
        for (int j = 0; j < 2; j++)
        {
            allUpdated *= data.BMS[i].temperaturesUpdated[j];
        }
        if (!allUpdated)
        {
            allOK = 0;
        }
    }

    // Check Charger Data
    int allUpdated = 1;
    allUpdated *= data.CHARGER.VtotalUpdated;
    allUpdated *= data.CHARGER.IchargeUpdated;
    for (int i = 0; i < 5; i++)
    {
        allUpdated *= data.CHARGER.flags[i];
    }
    if (!allUpdated)
    {
        allOK = 0;
    }

    // Check SEVCON Data
    allUpdated = 1;

    data.allOK = allOK;
}


void printData()
{
    char buffer[128];

    Serial.print("{");
    Serial.println("data: {");

    sprintf(buffer, "allOK: %d", data.allOK);
    Serial.println(buffer);

    Serial.println("BMS: [{");
    sprintf(buffer, "cellVoltagemV: [%d, %d, %d, %d, %d, %d, %d, %d, %d,%d ,%d ,%d]", data.BMS[0].cellVoltagemV[0], data.BMS[0].cellVoltagemV[1], data.BMS[0].cellVoltagemV[2], data.BMS[0].cellVoltagemV[3], data.BMS[0].cellVoltagemV[4], data.BMS[0].cellVoltagemV[5], data.BMS[0].cellVoltagemV[6], data.BMS[0].cellVoltagemV[7], data.BMS[0].cellVoltagemV[8], data.BMS[0].cellVoltagemV[9], data.BMS[0].cellVoltagemV[10], data.BMS[0].cellVoltagemV[11]);
    Serial.println(buffer);
    sprintf(buffer, "temperatures: [%d, %d]", data.BMS[0].temperatures[0], data.BMS[0].temperatures[1]);
    Serial.println(buffer);

    Serial.println("},");
    Serial.println("{");
    sprintf(buffer, "cellVoltagemV: [%d, %d, %d, %d, %d, %d, %d, %d, %d,%d ,%d ,%d]", data.BMS[1].cellVoltagemV[0], data.BMS[1].cellVoltagemV[1], data.BMS[1].cellVoltagemV[2], data.BMS[1].cellVoltagemV[3], data.BMS[1].cellVoltagemV[4], data.BMS[1].cellVoltagemV[5], data.BMS[1].cellVoltagemV[6], data.BMS[1].cellVoltagemV[7], data.BMS[1].cellVoltagemV[8], data.BMS[1].cellVoltagemV[9], data.BMS[1].cellVoltagemV[10], data.BMS[1].cellVoltagemV[11]);
    Serial.println(buffer);
    sprintf(buffer, "temperatures: [%d, %d]", data.BMS[1].temperatures[0], data.BMS[1].temperatures[1]);
    Serial.println(buffer);

    Serial.println("},");
    Serial.println("{");
    sprintf(buffer, "cellVoltagemV: [%d, %d, %d, %d, %d, %d, %d, %d, %d,%d ,%d ,%d]", data.BMS[2].cellVoltagemV[0], data.BMS[2].cellVoltagemV[1], data.BMS[2].cellVoltagemV[2], data.BMS[2].cellVoltagemV[3], data.BMS[2].cellVoltagemV[4], data.BMS[2].cellVoltagemV[5], data.BMS[2].cellVoltagemV[6], data.BMS[2].cellVoltagemV[7], data.BMS[2].cellVoltagemV[8], data.BMS[2].cellVoltagemV[9], data.BMS[2].cellVoltagemV[10], data.BMS[2].cellVoltagemV[11]);
    Serial.println(buffer);
    sprintf(buffer, "temperatures: [%d, %d]", data.BMS[2].temperatures[0], data.BMS[2].temperatures[1]);
    Serial.println(buffer);

    Serial.println("}");
    Serial.println("[,");
    Serial.println("SEVCON: {");
    sprintf(buffer, "TPDO1_1: %d", data.SEVCON.TPDO1_1);
    Serial.println(buffer);
    Serial.println("},");

    Serial.println("CHARGER: {");
    sprintf(buffer, "Vtotal: %d,", data.CHARGER.Vtotal);
    Serial.println(buffer);
    sprintf(buffer, "Icharge: %d,", data.CHARGER.Icharge);
    Serial.println(buffer);
    sprintf(buffer, "flags: [%d, %d, %d, %d, %d]", data.CHARGER.flags[0], data.CHARGER.flags[1], data.CHARGER.flags[2], data.CHARGER.flags[3], data.CHARGER.flags[4]);
    Serial.println(buffer);
    Serial.println("}");
    Serial.println("}");
    Serial.println("}");

    Serial.println("\n\n");
}
