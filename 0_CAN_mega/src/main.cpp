#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>

#define CAN0Speed     CAN_250KBPS
#define CAN0IntPin    49
#define CAN0CS        53
MCP_CAN CAN0(CAN0CS);

#define CAN1Speed     CAN_500KBPS
#define CAN1IntPin    46
#define CAN1CS        48
MCP_CAN CAN1(CAN1CS);

int startMillis = 0;
int period      = 1000;

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

    if (currentMillis - startMillis >= period) // Timer
    {
        INT32U id       = 0x12C;
        INT8U  ext      = 1;
        INT8U  len      = 2;
        INT8U  buf[len] = { 0, 0 };

        CAN0.sendMsgBuf(id, ext, len, buf);
        Serial.println("Sent message 0!");

        CAN1.sendMsgBuf(id, ext, len, buf);
        Serial.println("Sent message 1!");

        startMillis = currentMillis;
    }

    if (!digitalRead(CAN0IntPin)) // New message on CAN0
    {
        INT32U id;
        INT8U  len = 0;
        INT8U  buf[8];
        char   msgString[128];

        CAN0.readMsgBuf(&id, &len, buf);
        Serial.print("New in CAN0: ");

        if ((id & 0x80000000) == 0x80000000)
        {
            sprintf(msgString, "Extended ID: 0x%.8lX DLC: %1d Data: ", (id % 0x1FFFFFFF), len);
        }
        else
        {
            sprintf(msgString, "Standard ID: 0x%.3lX DLC: %1d Data: ", id, len);
        }

        Serial.print(msgString);

        if ((id & 0x40000000) == 0x40000000)
        {
            sprintf(msgString, "REMOTE REQUEST FRAME");
            Serial.print(msgString);
        }
        else
        {
            for (int i = 0; i < len; i++)
            {
                sprintf(msgString, " 0x%.2X", buf[i]);
                Serial.print(msgString);
            }
        }
    }

    if (!digitalRead(CAN1IntPin))     // New message on CAN1
    {
        INT32U id;
        INT8U  len = 0;
        INT8U  buf[8];
        char   msgString[128];

        CAN1.readMsgBuf(&id, &len, buf);
        Serial.print("New in CAN0: ");

        if ((id & 0x80000000) == 0x80000000)
        {
            sprintf(msgString, "Extended ID: 0x%.8lX DLC: %1d Data: ", (id % 0x1FFFFFFF), len);
        }
        else
        {
            sprintf(msgString, "Standard ID: 0x%.3lX DLC: %1d Data: ", id, len);
        }

        Serial.print(msgString);

        if ((id & 0x40000000) == 0x40000000)
        {
            sprintf(msgString, "REMOTE REQUEST FRAME");
            Serial.print(msgString);
        }
        else
        {
            for (int i = 0; i < len; i++)
            {
                sprintf(msgString, " 0x%.2X", buf[i]);
                Serial.print(msgString);
            }
        }
    }
}
