// Libraries
#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>

// CAN config
#define CAN0Speed     CAN_250KBPS
#define CAN0IntPin    49
#define CAN0CS        53
MCP_CAN CAN0(CAN0CS);
#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    46
#define CAN1CS        48
MCP_CAN CAN1(CAN1CS);

// Setup parameters
#define chargerID    0x1806E7F4

// Timer help
int startMillis0 = 0;
int startMillis1 = 0;
int period0      = 1000;
int period1      = 10000;

// Functions
void parseMessage(INT32U id, INT8U len, INT8U *buf);
void checkData();

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
    int totalVoltagemV        = 0;
    int totalVoltagemVUpdated = 0;
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
        Serial.println("Sent message 0!");

        CAN1.sendMsgBuf(id, ext, len, buf);
        Serial.println("Sent message 1!");

        startMillis0 = currentMillis;
    }

    if (currentMillis - startMillis1 >= period1) // Timer1
    {
        checkData();

        startMillis1 = currentMillis;
    }

    if (!digitalRead(CAN0IntPin)) // New message on CAN0
    {
        INT32U id;
        INT8U  len = 0;
        INT8U  buf[8];

        CAN0.readMsgBuf(&id, &len, buf);
        parseMessage(id, len, buf);
    }

    if (!digitalRead(CAN1IntPin))     // New message on CAN1
    {
        INT32U id;
        INT8U  len = 0;
        INT8U  buf[8];

        CAN1.readMsgBuf(&id, &len, buf);
        parseMessage(id, len, buf);
    }
}


void parseMessage(INT32U id, INT8U len, INT8U *buf)
{
    id = id & 0x1FFFFFFF;

    // Charger
    if (id == chargerID)
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
    if ((id > 300) && (id < 300 + 16 * 10))
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

    // SEVCON controller
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
            Serial.print("Some problem in BMS ");
            Serial.println(i);
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
        Serial.print("Some problem in Charger ");
        allOK = 0;
    }

    // Check SEVCON Data
    allUpdated = 1;

    data.allOK = allOK;
}
