#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>

#define CAN0Speed     CAN_250KBPS
#define CAN0IntPin    49
#define CAN0CS        53
MCP_CAN CAN0(CAN0CS);

#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    2
#define CAN1CS        48
MCP_CAN CAN1(CAN1CS);

int startMillis = 0;
int period      = 5000;
int c           = 0;

struct CANMsg
{
    INT32U id;
    INT8U  len;
    INT8U  buf;
};

struct CANMsg MSGBuffer[10];

void CAN1Interrupt();

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
    Serial.println(CAN1Speed);

    attachInterrupt(digitalPinToInterrupt(CAN1IntPin), CAN1Interrupt, LOW);
}


void loop()
{
    int currentMillis = millis();

    if (currentMillis - startMillis >= period) // Timer
    {
        startMillis = currentMillis;

        for (size_t i = 0; i < 5; i++)
        {
            Serial.println(MSGBuffer[i].id, HEX);
        }
        Serial.println("\n");
    }
}


void CAN1Interrupt()
{
    CAN1.readMsgBuf(&MSGBuffer[c].id, &MSGBuffer[c].len, &MSGBuffer[c].buf);
    MSGBuffer[c].id = MSGBuffer[c].id & 0x1FFFFFFF;
    c++;
    c = c % 5;
}
