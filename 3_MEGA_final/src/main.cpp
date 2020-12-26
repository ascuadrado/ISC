// Libraries
#include <Arduino.h> # For platformio development
#include "mcp_can.h" # mcp2515 library
#include <SPI.h>

#include "setup_config.h" # Contains all CAN configuration(pins, ids, etc)

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
void dataToDefault();
void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN);
int checkData();
void CAN0Interrupt();
void CAN1Interrupt();
void timer0();
void timer1();
void timer2();

// Data structures
struct Data data;

// CAN MSG Buffer
#define N    20
int           tailCounter = 0;
int           headCounter = 0;
struct CANMsg MSGBuffer[N];

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

    if (headCounter != tailCounter) // Process new msg in queue
    {
        parseMessage(MSGBuffer[tailCounter].id, MSGBuffer[tailCounter].len,
                     &MSGBuffer[tailCounter].buf[0], MSGBuffer[tailCounter].bus);
        tailCounter++;
        tailCounter %= N;
    }
}


void timer0() // BMS and charger queries
{
    INT32U id       = BMS0ID;
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

    BMSQueryCounter++;
    BMSQueryCounter = BMSQueryCounter % 3;

    id = chargerID;
    int v      = (uint8_t)(maxChargeVoltage * 10);
    int i      = (uint8_t)(maxChargeCurrent * 10);
    int charge = chargeIfPossible;

    uint8_t messageCharger[5] = { (v >> 8) & 0xFF,
                                  v & 0xFF,
                                  (i >> 8) & 0xFF,
                                  i & 0xFF,
                                  (1 - charge) };

    CAN0.sendMsgBuf(id, 1, 5, messageCharger);
}


void timer1() // Check data and output to serial
{
    Serial.print("Check data: ");
    Serial.println(checkData());
    writeData(data);
}


void timer2() // Free to use timer
{
    // Timer 2
}


void CAN0Interrupt()
{
    if (!CAN0.readMsgBuf(&MSGBuffer[headCounter].id, &MSGBuffer[headCounter].len, &MSGBuffer[headCounter].buf[0]))
    {
        MSGBuffer[headCounter].bus = 0;
        headCounter++;
        headCounter %= N;
    }
}


void CAN1Interrupt()
{
    int a = micros(); // Count time of interrupt

    if (!CAN1.readMsgBuf(&MSGBuffer[headCounter].id, &MSGBuffer[headCounter].len, &MSGBuffer[headCounter].buf[0]))
    {
        MSGBuffer[headCounter].bus = 1;
        headCounter++;
        headCounter %= N;
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

    // BMS and charger BUS
    if (busN == 0)
    {
        // Charger
        if ((id == chargerID))
        {
            // Voltage measured by charger
            data.CHARGER.Vtotal = ((buf[0] << 8) + buf[1]) * 100;
            // Instant charging current
            data.CHARGER.Icharge = ((buf[2] << 8) + buf[3]) * 100;
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
                    data.BMS[n].cellVoltagemV[m * 4 + i] = (buf[2 * i] << 8) + buf[2 * i + 1];
                }
            }
            // Temperature frame
            else if (m == 3)
            {
                for (int i = 0; i < 2; i++)
                {
                    data.BMS[n].temperatures[i] = buf[i] - 40;
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
            data.SEVCON.TPDO1_2 = (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO1_3 = ((int16_t)buf[4] << 8) + buf[5];
            data.SEVCON.TPDO1_4 = (buf[6] << 8) + buf[7];
            break;

        case 0x195:
            // TPDO2
            data.SEVCON.TPDO2_1 = (buf[0] << 8) + buf[1];
            data.SEVCON.TPDO2_2 = buf[2];
            data.SEVCON.TPDO2_3 = (buf[3] << 8) + buf[4];
            break;

        case 0x146:
            // TPDO3
            data.SEVCON.TPDO3_1 = ((int16_t)buf[0] << 8) + buf[1];
            data.SEVCON.TPDO3_2 = (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO3_3 = (buf[4] << 8) + buf[5];
            data.SEVCON.TPDO3_4 = (buf[6] << 8) + buf[7];
            break;

        case 0x168:
            // TPDO4
            data.SEVCON.TPDO4_1 = (buf[0] << 8) + buf[1];
            data.SEVCON.TPDO4_2 = (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO4_3 = (buf[4] << 8) + buf[5];
            data.SEVCON.TPDO4_4 = (buf[6] << 8) + buf[7];
            break;

        case 0x370:
            // TPDO5
            data.SEVCON.TPDO5_1 = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
            data.SEVCON.TPDO5_2 = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + buf[7];
            break;
        }
    }
}


int checkData()
{
    int allOK = 1;

    dataToDefault();
    return allOK;
}


void dataToDefault() // Set all values to -1 to check if we are receiving new data
{
    int d = -1;

    data.allOK = d;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            data.BMS[i].cellVoltagemV[j] = d;
        }
        data.BMS[i].temperatures[0] = d;
        data.BMS[i].temperatures[1] = d;
    }
    data.CHARGER.Vtotal  = d;
    data.CHARGER.Icharge = d;
    for (int i = 0; i < 5; i++)
    {
        data.CHARGER.flags[i] = d;
    }
    data.SEVCON.TPDO1_1 = d;
}
