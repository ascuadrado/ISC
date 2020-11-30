/*
 * 2_CAN_Nano
 *
 * Arduino nano that interfaces between SEVCON controller (CAN 1)
 * and Arduino MEGA & RPI (CAN2).
 *
 * SEVCON messages are read as soon as they are received and are sent back periodically
 *
 */

#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>

// Bus 1: SEVCON
#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    2
#define CAN1CS        10
MCP_CAN CAN1(CAN1CS);

// Bus 2: MEGA & RPI
#define CAN2Speed     CAN_250KBPS
#define CAN2IntPin    49
#define CAN2CS        53
MCP_CAN CAN2(CAN2CS);

// Timer help
int startMillis = 0;
int period      = 20;

// Counters
int newMessageCount = 0;
int count           = 0;

struct CANMsg
{
    INT32U id;
    INT8U  len;
    INT8U  buf;
};

struct CANMsg MSGBuffer[20];

void CAN1Interrupt();
void tryToReadMessage();

void setup()
{
    Serial.begin(115200);

    Serial.println("Arduino NANO. 2 CAN buses, same SPI");

    while (CAN1.begin(MCP_ANY, CAN1Speed, MCP_8MHZ))
    {
        Serial.print("Failed starting CAN");
        delay(1000);
    }

    CAN1.setMode(MCP_NORMAL);

    Serial.print(" , CAN1 - ");
    Serial.println(CAN1Speed);

    delay(100);

    attachInterrupt(digitalPinToInterrupt(CAN1IntPin), CAN1Interrupt, LOW);
    delay(100);
}


void loop()
{
    int currentMillis = millis();

    tryToReadMessage();

    if (currentMillis - startMillis >= period) // Timer
    {
        startMillis = currentMillis;
    }
}


void tryToReadMessage()
{
    if (newMessageCount)
    {
        Serial.print(newMessageCount);
        for (int i = 0; i < newMessageCount; i++)
        {
            char message[128];
            sprintf(message, "New msg to read. Position: %d. ID: 0x%lx", i, MSGBuffer[i].id);
            Serial.println(message);
            delay(2);
        }
        newMessageCount = 0;
    }
}


void CAN1Interrupt()
{
    CAN1.readMsgBuf(&MSGBuffer[newMessageCount].id, &MSGBuffer[newMessageCount].len, &MSGBuffer[newMessageCount].buf);
    newMessageCount++;
}
