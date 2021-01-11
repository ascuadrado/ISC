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
int startMillis1    = 0;
int startMillis2    = 0;
int period0         = 1000;
int period1         = 10000;
int period2         = 1000;
int BMSQueryCounter = 0;

// Functions
void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN);
int checkData();
void CAN0Interrupt();
void CAN1Interrupt();
void timer0();
void timer1();
void timer2();
void simBus0Data();
void simBus1Data();

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
        startMillis0 = currentMillis;
    }

    if (currentMillis - startMillis1 >= period1)   // Timer1
    {
        //checkData();
        timer1();
        startMillis1 = currentMillis;
    }

    if (currentMillis - startMillis2 >= period2)   // Timer2
    {
        //checkData();
        timer2();
        startMillis2 = currentMillis;
    }

    if (bufferCount)
    {
        for (int i = 0; i < bufferCount; i++)
        {
            parseMessage(MSGBuffer[i].id, MSGBuffer[i].len, &MSGBuffer[i].buf[0], MSGBuffer[i].bus);
        }
        bufferCount = 0;
    }
}


void timer0()
{
    /*
     * INT32U id       = 0x12C;
     * INT8U  ext      = 1;
     * INT8U  len      = 2;
     * INT8U  buf[len] = { (shuntVoltagemV >> 8) & 0xFF, shuntVoltagemV & 0xFF };
     *
     * // Query all BMS
     * if (BMSQueryCounter == 0)
     * {
     *    CAN0.sendMsgBuf(id, ext, len, buf);
     * }
     * else if (BMSQueryCounter == 1)
     * {
     *    CAN0.sendMsgBuf(id + 10, ext, len, buf);
     * }
     * else if (BMSQueryCounter == 2)
     * {
     *    CAN0.sendMsgBuf(id + 20, ext, len, buf);
     * }
     *
     * BMSQueryCounter++;
     * BMSQueryCounter = BMSQueryCounter % 3;
     */

    simBus0Data();
    simBus1Data();
}


void timer1()
{
    //Serial.print("Check data: ");
    //Serial.println(checkData());
    //writeData(data);
}


void timer2()
{
    INT32U  id     = chargerID;
    uint8_t v      = (uint8_t)(maxChargeVoltage * 10);
    uint8_t i      = (uint8_t)(maxChargeCurrent * 10);
    uint8_t charge = chargeIfPossible;

    uint8_t messageCharger[5] = { (uint8_t)(v >> 8) & 0xFF, (uint8_t)(v) & 0xFF,
                                  (i >> 8) & 0xFF,          (uint8_t)(i) & 0xFF,
                                  (uint8_t)(1 - charge) };

    CAN0.sendMsgBuf(id, 1, 5, messageCharger);
}


void simBus0Data()
{
    // BMS Msgs
    INT32U id           = 0x12C;
    INT8U  ext          = 1;
    INT8U  len          = 2;
    INT8U  buf[len]     = { (shuntVoltagemV >> 8) & 0xFF, shuntVoltagemV & 0xFF };
    INT8U  vresponse[8] = { 13, 134, 13, 134, 13, 134, 13, 134 };
    INT8U  tresponse[8] = { 56, 56, 0, 0, 0, 0, 0, 0 };

    // Charger msgs
    int     v           = (maxChargeVoltage * 10);
    int     i           = (maxChargeCurrent * 10);
    uint8_t charge      = chargeIfPossible;
    INT8U   charger1[5] = { (byte)(v >> 8) & 0xFF,  (byte)(v) & 0xFF,
                            (uint8_t)(i >> 8) & 0xFF, (uint8_t)(i) & 0xFF,
                            (uint8_t)(1 - charge) };
    INT8U   charger2[5] = { (v >> 8) & 0xFF, (v) & 0xFF,
                            (i >> 8) & 0xFF,   (i) & 0xFF,
                            5 };

    // Query all BMS
    if (BMSQueryCounter == 0)
    {
        // BMS1
        CAN0.sendMsgBuf(id, ext, len, buf);
        CAN0.sendMsgBuf(id + 1, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 2, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 3, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 4, ext, 8, tresponse);

        // Charger
        CAN0.sendMsgBuf(chargerID, 1, 5, charger1);
        CAN0.sendMsgBuf(chargerID, 1, 5, charger2);
        Serial.print("CHARGER & ");
    }
    else if (BMSQueryCounter == 1)
    {
        // BMS2
        CAN0.sendMsgBuf(id + 10, ext, len, buf);
        CAN0.sendMsgBuf(id + 11, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 12, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 13, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 14, ext, 8, tresponse);
    }
    else if (BMSQueryCounter == 2)
    {
        // BMS3
        CAN0.sendMsgBuf(id + 20, ext, len, buf);
        CAN0.sendMsgBuf(id + 21, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 22, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 23, ext, 8, vresponse);
        CAN0.sendMsgBuf(id + 24, ext, 8, tresponse);
    }

    BMSQueryCounter++;

    BMSQueryCounter = BMSQueryCounter % 3;

    Serial.print("BMS ");
    Serial.print(BMSQueryCounter);
    Serial.println(" Simulated & bus 0");
}


void simBus1Data()
{
    INT32U id      = 0x101;
    INT8U  len     = 8;
    INT8U  buf1[8] = { 0xDB, 0xFF, 0x21, 0xFF, 0xFE, 0x00, 0x1E, 0x01 };

    CAN1.sendMsgBuf(id, 1, len, buf1);

    id  = 0x102;
    len = 8;
    INT8U buf2[8] = { 0x77, 0x06, 0x42, 0x00, 0x00, 0x09, 0x73, 0x06 };

    CAN1.sendMsgBuf(id, 1, len, buf2);

    id  = 0x103;
    len = 6;
    INT8U buf3[8] = { 0xD8, 0x02, 0x16, 0x00, 0x16, 0x00 };

    CAN1.sendMsgBuf(id, 1, len, buf3);

    id  = 0x104;
    len = 1;
    INT8U buf4[1] = { 0x15 };

    CAN1.sendMsgBuf(id, 1, len, buf4);

    id  = 0x105;
    len = 8;
    INT8U buf5[8] = { 0xB0, 0x09, 0x00, 0x00, 0x94, 0x06, 0x00, 0x00 };

    CAN1.sendMsgBuf(id, 1, len, buf5);

    Serial.println("Simulated bus 1");
}


void CAN0Interrupt()
{
    if (!CAN0.readMsgBuf(&MSGBuffer[bufferCount].id, &MSGBuffer[bufferCount].len, &MSGBuffer[bufferCount].buf[0]))
    {
        MSGBuffer[bufferCount].bus = 0;
        bufferCount++;
        //Serial.println("Int 0");
    }
}


void CAN1Interrupt()
{
    int a = micros();

    if (!CAN1.readMsgBuf(&MSGBuffer[bufferCount].id, &MSGBuffer[bufferCount].len, &MSGBuffer[bufferCount].buf[0]))
    {
        MSGBuffer[bufferCount].bus = 1;
        bufferCount++;
        //Serial.println("Int 1");
    }
    int b = micros();

    Serial.print("t= ");
    Serial.println(a - b);
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
