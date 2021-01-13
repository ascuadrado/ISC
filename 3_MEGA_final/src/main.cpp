// Libraries
#include <Arduino.h> // For platformio development
#include "mcp_can.h" // mcp2515 library
#include <SPI.h>

#include "setup_config.h" // Contains all CAN configuration(pins, ids, etc)

// CAN instances
MCP_CAN CAN0(CAN0CS);
MCP_CAN CAN1(CAN1CS);

// Timer help
int startMillis0    = 0;
int startMillis1    = 0;
int startMillis2    = 0;
int period0         = 300;
int period1         = 1000;
int period2         = 3000;
int BMSQueryCounter = 0;

// Functions
void setup();
void loop();

void timer0(); // Query BMS (one at a time)
void timer1(); // Query charger
void timer2(); // Check data and debug

void CAN0Interrupt();
void CAN1Interrupt();

void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN);
int init_data();
int connectedFlags_to_default();
int checkData();

// Data structures
struct Data data;

// CAN MSG Buffer (circular buffer -> if full, overwrites messages)
#define N    20
int           tailCounter = 0;
int           headCounter = 0;
struct CANMsg MSGBuffer[N];

// Estados posibles
enum Estado
{
    standBy,
    run,
    charge,
    error
};
Estado estado = standBy;

/*
 * Setup function
 */
void setup()
{
    Serial.begin(115200);

    Serial.println("Arduino MEGA. 2 CAN buses, same SPI");

    attachInterrupt(digitalPinToInterrupt(CAN0IntPin), CAN0Interrupt, LOW);
    attachInterrupt(digitalPinToInterrupt(CAN1IntPin), CAN1Interrupt, LOW);
    pinMode(shutdownPIN, OUTPUT);

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

    init_data();
}


/*
 * Loop function (runs continuously)
 */
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
        timer1();
        startMillis1 = currentMillis;
    }

    if (currentMillis - startMillis2 >= period2)   // Timer2
    {
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


void timer0() // BMS queries
{
    INT32U id       = BMS0ID;
    INT8U  ext      = 1;
    INT8U  len      = 2;
    INT8U  buf[len] = { (shuntVoltagemV >> 8) & 0xFF, shuntVoltagemV & 0xFF };

    // Query one BMS at a time
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
}


void timer1() // Query charger (every 1s)
{
    long id     = chargerIDSend;
    int  v      = (maxChargeVoltage * 10);
    int  i      = (maxChargeCurrent * 10);
    int  charge = 0;

    if (estado == charge)
    {
        charge = 1;
    }

    uint8_t messageCharger[5] = { (v >> 8) & 0xFF,
                                  v & 0xFF,
                                  (i >> 8) & 0xFF,
                                  i & 0xFF,
                                  (1 - charge) };

    CAN0.sendMsgBuf(id, 1, 5, messageCharger);
}


void timer2() // Check data and output to serial
{
    writeData(data);
    Serial.print("Check data: ");
    Serial.println(checkData());
}


void CAN0Interrupt() // New msg on CAN0 -> read and into buffer
{
    if (!CAN0.readMsgBuf(&MSGBuffer[headCounter].id, &MSGBuffer[headCounter].len, &MSGBuffer[headCounter].buf[0]))
    {
        MSGBuffer[headCounter].bus = 0;
        headCounter++;
        headCounter %= N;
    }
}


void CAN1Interrupt()  // New msg on CAN0 -> read and into buffer
{
    //int a = micros(); // Count time of interrupt (for debugging only)
    if (!CAN1.readMsgBuf(&MSGBuffer[headCounter].id, &MSGBuffer[headCounter].len, &MSGBuffer[headCounter].buf[0]))
    {
        MSGBuffer[headCounter].bus = 1;
        headCounter++;
        headCounter %= N;
    }
    //int b = micros();
    //Serial.print("t =  ");
    //Serial.println(b-a);
}


void parseMessage(INT32U id, INT8U len, INT8U *buf, INT8U busN)
{
    id = id & 0x1FFFFFFF;

    // PRINT Message for debugging
    char message[128];

    sprintf(message, "Message on Bus %d, id: 0x%lx, len: %d", busN, id, len);
    //Serial.println(message);
    for (int i = 0; i < len; i++)
    {
        sprintf(message, " 0x%.2X", buf[i]);
        //Serial.print(message);
    }
    //Serial.println();

    // ----

    // BUS 0: BMS and charger, 250kbps
    if (busN == 0)
    {
        // Charger
        if ((id == chargerIDRecv))
        {
            data.CHARGER.connected = 1;
            // Voltage measured by charger
            data.CHARGER.Vtotal = ((buf[0] << 8) + buf[1]) / 10;
            // Instant charging current
            data.CHARGER.Icharge = ((buf[2] << 8) + buf[3]) / 10;
            // Charger flags
            int flags = buf[4];
            data.CHARGER.flags[0] = (flags >> 0) & 0x1;     // Flag 0
            data.CHARGER.flags[1] = (flags >> 1) & 0x1;     // Flag 1
            data.CHARGER.flags[2] = (flags >> 2) & 0x1;     // Flag 2
            data.CHARGER.flags[3] = (flags >> 3) & 0x1;     // Flag 3
            data.CHARGER.flags[4] = (flags >> 4) & 0x1;     // Flag 4
        }

        // BMS Modules
        if ((id > 300) && (id < 400))
        {
            // BMS number (1-16)
            int n = (id - 300) / 10;
            // Message number (0-3)
            int m = (id - 300 - n * 10 - 1);

            data.BMS[n].connected = 1;

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

    // BUS 1: SEVCON controller
    else if (busN == 1)
    {
        data.SEVCON.connected = 1;
        switch (id)
        {
        case 0x101:
            // TPDO1
            data.SEVCON.target_id = ((buf[1] << 8) + buf[0]) * 0.0625;
            data.SEVCON.id        = ((buf[3] << 8) + buf[2]) * 0.0625;
            data.SEVCON.target_iq = ((buf[5] << 8) + buf[4]) * 0.0625;
            data.SEVCON.iq        = ((buf[7] << 8) + buf[6]) * 0.0625;
            break;

        case 0x102:
            // TPDO2
            data.SEVCON.battery_voltage   = ((buf[1] << 8) + buf[0]) * 0.0625;
            data.SEVCON.battery_current   = ((buf[3] << 8) + buf[2]) * 0.0625;
            data.SEVCON.line_contactor    = ((buf[5] << 8) + buf[4]) * 1.0;
            data.SEVCON.capacitor_voltage = ((buf[7] << 8) + buf[6]) * 0.0625;
            break;

        case 0x103:
            // TPDO3
            data.SEVCON.throttle_value = ((buf[1] << 8) + buf[0]) * 1.0 / 32767;
            data.SEVCON.target_torque  = ((buf[3] << 8) + buf[2]) * 0.1 / 100;
            data.SEVCON.torque         = ((buf[5] << 8) + buf[4]) * 0.1 / 100;
            break;

        case 0x104:
            // TPDO4
            data.SEVCON.heatsink_temp = buf[0];
            break;

        case 0x105:
            // TPDO5
            data.SEVCON.maximum_motor_speed = 1.0 * (buf[3] << 24) + 1.0 * (buf[2] << 16) + 1.0 * (buf[1] << 8) + 1.0 * buf[0];
            data.SEVCON.velocity            = 1.0 * (buf[7] << 24) + 1.0 * (buf[6] << 16) + 1.0 * (buf[5] << 8) + 1.0 * buf[4];
            break;
        }
    }
}


int init_data()
{
    int d = defaultINT;

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

    data.SEVCON.target_id = d;
    data.SEVCON.id        = d;
    data.SEVCON.target_iq = d;
    data.SEVCON.iq        = d;

    data.SEVCON.battery_voltage   = d;
    data.SEVCON.battery_current   = d;
    data.SEVCON.line_contactor    = d;
    data.SEVCON.capacitor_voltage = d;

    data.SEVCON.throttle_value = d;
    data.SEVCON.target_torque  = d;
    data.SEVCON.torque         = d;

    data.SEVCON.heatsink_temp = d;

    data.SEVCON.maximum_motor_speed = d;
    data.SEVCON.velocity            = d;

    connectedFlags_to_default();

    return 1;
}


int connectedFlags_to_default()
{
    data.allOK = 0;
    for (int i = 0; i < 3; i++)
    {
        data.BMS[i].connected = 0;
    }
    data.CHARGER.connected = 0;
    data.SEVCON.connected  = 0;
    return 1;
}


int checkData()
{
    // allOK = 0 means something is wrong
    int allOK = 1;

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
        allOK = 0;
    }

    // Check if charger connected
    if (data.CHARGER.connected)
    {
        if (estado == standBy)
        {
            estado = charge;
        }
    }
    else if (estado == charge)
    {
        estado = error;
        // Charger not connected
    }

    // Check if sevcon connected
    if (data.SEVCON.connected)
    {
        if (data.SEVCON.line_contactor < 1000)
        {
            allOK = 0;
        }
    }
    else
    {
        allOK = 0;
    }


    // Print summary stats
    char buffer[64];

    sprintf(buffer, "MaxV: %d, MinV: %d, TotalV: %ld, maxT: %d \n", maxV, minV, sumV, maxT);
    Serial.print(buffer);
    Serial.println("-----------------------------");

    if (allOK)
    {
        // All good
        digitalWrite(shutdownPIN, HIGH);
        Serial.println("-----------------------------");
        Serial.println("ALL GOOD");
        Serial.println("-----------------------------");
    }
    else
    {
        // Something is wrong
        estado = error;
        digitalWrite(shutdownPIN, LOW);
        Serial.println("-----------------------------");
        Serial.println("SOMETHING's WRONG!!!");
        Serial.println("-----------------------------");
    }

    connectedFlags_to_default();
    data.allOK = allOK;
    return allOK;
}
