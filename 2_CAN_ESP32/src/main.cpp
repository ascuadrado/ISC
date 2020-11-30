#include <Arduino.h>

// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ESP32SJA1000.h"

// CAN 1: SEVCON
#define CTXPin       5
#define CRXPin       4

#define BufferLEN    20

struct CANMessage
{
    uint32_t id;
    uint8_t  len;
    uint8_t  data[8];
};

struct CANMessage buffer[BufferLEN];

int nextBufferIn  = 0;
int nextBufferOut = 0;

void setup()
{
    // Serial setup
    Serial.begin(115200);

    while (!Serial)
    {
        delay(1);
    }
    Serial.println("CAN Receiver on ESP32!");

    // Pins setup
    pinMode(CTXPin, INPUT);
    pinMode(CRXPin, OUTPUT);

    // CAN bus startup
    if (!CAN1.begin(1000E3))
    {
        Serial.println("Starting CAN failed!");

        while (1)
        {
            delay(1);
        }
    }

    Serial.println("Ready!");
}


void loop()
{
    // CAN managing
    int packetSize = CAN1.parsePacket();

    if (packetSize)
    {
        Serial.println("New CAN message: ");

        // Move messages over
        for (int i = BufferLEN - 1; i > 0; i--)
        {
            buffer[i] = buffer[i - 1];
        }

        Serial.print("Received ");

        if (CAN1.packetExtended())
        {
            Serial.print("extended ");
        }

        if (CAN1.packetRtr())
        {
            Serial.print("RTR "); // Remote transmission request, packet contains no data
        }

        Serial.print("packet with id 0x");
        buffer[0].id = CAN1.packetId();
        Serial.print(buffer[0].id, HEX);

        if (CAN1.packetRtr())
        {
            Serial.print(" and requested length ");
            Serial.println(CAN1.packetDlc());
        }
        else
        {
            Serial.print(" and length ");
            buffer[0].len = packetSize;
            Serial.println(packetSize);

            for (int i = 0; i < packetSize; i++)
            {
                buffer[0].data[i] = CAN1.read();
                Serial.print(buffer[0].data[i]);
                Serial.print(" ");
            }
        }

        Serial.println();
    }
}
