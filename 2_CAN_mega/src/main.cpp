// Libraries
#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>

#include "setup_config.h"

// CAN instances
MCP_CAN CAN0(CAN0CS);
MCP_CAN CAN1(CAN1CS);

// Timer help
int startMillis0    = 0;
int startMillis1    = 5000;
int period0         = 1000;
int period1         = 10000;
int BMSQueryCounter = 0;

// Functions
void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN);
int checkData();
void CAN0Interrupt();
void CAN1Interrupt();
void timer0();
void timer1();

// Data structures
struct Data data;

// CAN MSG Buffer
int           bufferCount = 0;
struct CANMsg MSGBuffer[20];

/*
 * Main program
 */
void setup()
{
    Serial.begin(115200);

    Serial.println("Arduino MEGA. 2 CAN buses, same SPI");

    attachInterrupt(digitalPinToInterrupt(CAN0IntPin), CAN0Interrupt, LOW);
    attachInterrupt(digitalPinToInterrupt(CAN1IntPin), CAN1Interrupt, LOW);

    while (CAN0.begin(MCP_ANY, CAN0Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN 0");
        delay(1000);
    }

    while (CAN1.begin(MCP_ANY, CAN1Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN 1");
        delay(1000);
    }

    CAN0.setMode(MCP_NORMAL);
    CAN1.setMode(MCP_NORMAL);

    Serial.print("CAN ready: CAN0 - ");
    Serial.print(CAN0Speed);
    Serial.print(" , CAN1 - ");
    Serial.print(CAN1Speed);
    Serial.println();
}


void loop()
{
    int currentMillis = millis();

    if (currentMillis - startMillis0 >= period0)   // Timer0
    {
        timer0();
    }

    if (currentMillis - startMillis1 >= period1)   // Timer1
    {
        //checkData();
        timer1();
        startMillis1 = currentMillis;
    }

    if (bufferCount)
    {
        for (int i = 0; i < bufferCount; i++)
        {
            parseMessage(MSGBuffer[i].id, MSGBuffer[i].len, &MSGBuffer[i].buf, MSGBuffer[i].bus);
        }
        bufferCount = 0;
    }
}


void timer0()
{
    // Do something

    INT32U id       = 0x12C;
    INT8U  ext      = 1;
    INT8U  len      = 2;
    INT8U  buf[len] = { (shuntVoltagemV >> 8) & 0xFF, shuntVoltagemV & 0xFF };

    // Query all BMS
    if (BMSQueryCounter == 0)
    {
        CAN0.sendMsgBuf(id, ext, len, buf);
    }
    else if (BMSQueryCounter == 1)
    {
        CAN0.sendMsgBuf(id + 10, ext, len, buf);
    }
    else if (BMSQueryCounter == 2)
    {
        CAN0.sendMsgBuf(id + 20, ext, len, buf);
    }

    // Test msg for BUS 1
    //CAN1.sendMsgBuf(id, ext, len, buf);
    BMSQueryCounter++;
    BMSQueryCounter = BMSQueryCounter % 3;
}


void timer1()
{
    // Do something
    Serial.print("Check data: ");
    Serial.println(checkData());
    writeData(data);
}


void CAN0Interrupt()
{
    if (!CAN0.readMsgBuf(&MSGBuffer[bufferCount].id, &MSGBuffer[bufferCount].len, &MSGBuffer[bufferCount].buf))
    {
        MSGBuffer[bufferCount].bus = 0;
        bufferCount++;
        //Serial.println("Int 0");
    }
}


void CAN1Interrupt()
{
    int a = micros();

    if (!CAN1.readMsgBuf(&MSGBuffer[bufferCount].id, &MSGBuffer[bufferCount].len, &MSGBuffer[bufferCount].buf))
    {
        MSGBuffer[bufferCount].bus = 1;
        bufferCount++;
        //Serial.println("Int 1");
    }
    int b = micros();
    Serial.print(a);
    Serial.print(" - ");
    Serial.println(b);
}


void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN)
{
    id = id & 0x1FFFFFFF;

    char message[128];

    sprintf(message, "Message on Bus %d, id: 0x%lx, len: %d", busN, id, len);
    Serial.println(message);
    for (int i = 0; i < len; i++)
    {
        sprintf(message, " 0x%.2X", buf[i]);
        Serial.print(message);
    }
    Serial.println("\n\n");


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
            data.CHARGER.flags[0] = (flags >> 7) & 0x1;     // Flag 0
            data.CHARGER.flags[1] = (flags >> 6) & 0x1;     // Flag 1
            data.CHARGER.flags[2] = (flags >> 5) & 0x1;     // Flag 2
            data.CHARGER.flags[3] = (flags >> 4) & 0x1;     // Flag 3
            data.CHARGER.flags[4] = (flags >> 3) & 0x1;     // Flag 4
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
            data.SEVCON.TPDO1_1        = (buf[0] << 8) + buf[1];
            data.SEVCON.TPDO1_2        = (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO1_3        = ((int16_t)buf[4] << 8) + buf[5];
            data.SEVCON.TPDO1_4        = (buf[6] << 8) + buf[7];
            data.SEVCON.TPDO1_1Updated = 1;
            data.SEVCON.TPDO1_2Updated = 1;
            data.SEVCON.TPDO1_3Updated = 1;
            data.SEVCON.TPDO1_4Updated = 1;
            break;

        case 0x195:
            // TPDO2
            data.SEVCON.TPDO2_1        = (buf[0] << 8) + buf[1];
            data.SEVCON.TPDO2_2        = buf[2];
            data.SEVCON.TPDO2_3        = (buf[3] << 8) + buf[4];
            data.SEVCON.TPDO2_1Updated = 1;
            data.SEVCON.TPDO2_2Updated = 1;
            data.SEVCON.TPDO2_3Updated = 1;
            break;

        case 0x146:
            // TPDO3
            data.SEVCON.TPDO3_1        = ((int16_t)buf[0] << 8) + buf[1];
            data.SEVCON.TPDO3_2        = (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO3_3        = (buf[4] << 8) + buf[5];
            data.SEVCON.TPDO3_4        = (buf[6] << 8) + buf[7];
            data.SEVCON.TPDO3_1Updated = 1;
            data.SEVCON.TPDO3_2Updated = 1;
            data.SEVCON.TPDO3_3Updated = 1;
            data.SEVCON.TPDO3_4Updated = 1;
            break;

        case 0x168:
            // TPDO4
            data.SEVCON.TPDO4_1        = (buf[0] << 8) + buf[1];
            data.SEVCON.TPDO4_2        = (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO4_3        = (buf[4] << 8) + buf[5];
            data.SEVCON.TPDO4_4        = (buf[6] << 8) + buf[7];
            data.SEVCON.TPDO4_1Updated = 1;
            data.SEVCON.TPDO4_2Updated = 1;
            data.SEVCON.TPDO4_3Updated = 1;
            data.SEVCON.TPDO4_4Updated = 1;
            break;

        case 0x370:
            // TPDO5
            data.SEVCON.TPDO5_1        = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO5_2        = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + buf[7];
            data.SEVCON.TPDO5_1Updated = 1;
            data.SEVCON.TPDO5_2Updated = 1;
            break;
        }
    }
}


int checkData()
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
            data.BMS[i].cellVoltagemVUpdated[j] = 0;
        }

        for (int j = 0; j < 2; j++)
        {
            allUpdated *= data.BMS[i].temperaturesUpdated[j];
            data.BMS[i].temperaturesUpdated[j] = 0;
        }

        if (!allUpdated)
        {
            allOK = 0;
            //Serial.println("Some BMS missing");
        }
    }

    // Check Charger Data
    int allUpdated = 1;

    allUpdated *= data.CHARGER.VtotalUpdated;
    data.CHARGER.VtotalUpdated = 0;
    allUpdated *= data.CHARGER.IchargeUpdated;
    data.CHARGER.IchargeUpdated = 0;

    for (int i = 0; i < 5; i++)
    {
        allUpdated           *= data.CHARGER.flags[i];
        data.CHARGER.flags[i] = 0;
    }

    if (!allUpdated)
    {
        allOK = 0;
        //Serial.println("No connection with CHARGER");
    }

    // Check SEVCON Data
    allUpdated = 1;

    return allOK;
}
