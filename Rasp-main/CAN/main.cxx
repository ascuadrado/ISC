/*
 * main.cxx
 * Alberto Sánchez Cuadrado
 *
 * Create a buffer with the last messages and periodically update a JSON file with data
 *
 */

// Presinstalled libraries
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>

// CAN library (with mcp2515)
#include "src/mcp_can_rpi.h"
#include "src/setup_config.h"

// CAN instances
MCP_CAN CAN0(0, 1000000, CAN0IntPin, CAN0CS);
MCP_CAN CAN1(0, 1000000, CAN1IntPin, CAN1CS);

// Timer help
useconds_t period0 = 999999;

// Functions
void setup();
void loop();
void timer0(int sig_num);
void newInterrupt0();
void newInterrupt1();
void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN);
void checkData();
void writeData();

// Data structures
struct Data   data;
struct CANMsg MSGBuffer[20];

// Buffer help
int bufferCount = 0;

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
    printf("Program 4_buffer.cxx is running!\n\n");


    // Initialize GPIO pins and SPI bus of the Raspberry Pi
    wiringPiSetup();
    CAN0.setupInterruptGpio();
    CAN0.setupSpi();
    CAN1.setupInterruptGpio();
    CAN1.setupSpi();
    printf("GPIO Pins initialized & SPI started\n");


    // Attach interrupt to read incoming messages
    wiringPiISR(CAN0IntPin, INT_EDGE_FALLING, newInterrupt0);
    wiringPiISR(CAN1IntPin, INT_EDGE_FALLING, newInterrupt1);


    // Create timer for periodic functions
    signal(SIGALRM, timer0);
    ualarm(period0, period0);


    // Init CAN and enter mode
    while (CAN0.begin(MCP_ANY, CAN0Speed, MCP_8MHZ))
    {
        printf("Failed starting CAN0\n");
        usleep(1000000);
        break; // To remove later on
    }

    while (CAN1.begin(MCP_ANY, CAN1Speed, MCP_8MHZ))
    {
        printf("Failed starting CAN1\n");
        usleep(1000000);
        break; // To remove later on
    }

    CAN0.setMode(MCP_NORMAL);
    CAN1.setMode(MCP_NORMAL);

    printf("CAN ready: CAN0 - %d\n", CAN0Speed);
    printf("CAN ready: CAN1 - %d\n", CAN1Speed);
}


void loop()
{
    if (bufferCount)
    {
        printf("%d", bufferCount);
        for (int i = 0; i < bufferCount; i++)
        {
            printf("New msg to read. Position: %d. ID: 0x%lx", i, MSGBuffer[i].id);
            parseMessage(MSGBuffer[i].id, MSGBuffer[i].len, &MSGBuffer[i].buf, MSGBuffer[i].bus);
        }
        bufferCount = 0;
    }
    else
    {
        usleep(1000);
    }
}


void timer0(int sig_num)
{
    // Do something
    checkData();
    writeData(data);
}


void newInterrupt0()
{
    CAN0.readMsgBuf(&MSGBuffer[bufferCount].id, &MSGBuffer[bufferCount].len, &MSGBuffer[bufferCount].buf);
    MSGBuffer[bufferCount].bus = 0;
    bufferCount++;
}


void newInterrupt1()
{
    CAN1.readMsgBuf(&MSGBuffer[bufferCount].id, &MSGBuffer[bufferCount].len, &MSGBuffer[bufferCount].buf);
    MSGBuffer[bufferCount].bus = 1;
    bufferCount++;
}


void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN)
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
