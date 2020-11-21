/*
 * 0_recopilacion_datos.cxx
 * Alberto Sánchez Cuadrado
 *
 */

// Presinstalled libraries
#include <iostream>
#include <unistd.h>
#include <signal.h>

// CAN library (with mcp2515)
#include "src/mcp_can_rpi.h"
#include "src/setup_config.h"

// CAN instances
MCP_CAN CAN0(0, 1000000, CAN0IntPin, CAN0CS);
MCP_CAN CAN0(0, 1000000, CAN1IntPin, CAN1CS);

// Timer help
#define period0    1000

// Functions
void parseMessage(INT32U id, INT8U len, INT8U *buf, int busN);
void checkData();
void timer0();
void setup();
void loop();

// Data structures
struct Data data;

int main()
{
    setup();
    while (1)
    {
        loop();
    }
}


void setup()
{
    printf("Hello World! Program 3_recopilacion_datos.cxx is running!\n\n");

    // Initialize GPIO pins and SPI bus of the Raspberry Pi
    wiringPiSetup();
    CAN.setupInterruptGpio();
    CAN.setupSpi();
    printf("GPIO Pins initialized & SPI started\n");

    // Attach interrupt to read incoming messages and timer
    signal(SIGALRM, period0);
    ualarm(period0, period0);

    //wiringPiISR(IntPIN, INT_EDGE_FALLING, detectInterrupt());

    while (CAN0.begin(MCP_ANY, CAN0Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN0");
        usleep(1000000);
    }

    while (CAN1.begin(MCP_ANY, CAN1Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN1");
        usleep(1000000);
    }

    CAN0.setMode(MCP_NORMAL);
    CAN1.setMode(MCP_NORMAL);

    printf("CAN ready: CAN0 - %d\n", CAN0Speed);
    printf("CAN ready: CAN0 - %d\n", CAN1Speed);
}


void loop()
{
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
    usleep(100);
}


void timer0()
{
    // Do something
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
