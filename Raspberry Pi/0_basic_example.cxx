/*
 * 0_basic_example.cxx
 * Alberto Sánchez Cuadrado
 *
 * This example uses interrupts to read messages
 * It also sends a message periodically
 *
 * Connexions:
 * If using SPI bus 0 in Raspberry Pi:
 * MOSI (GPIO10);
 * MISO (GPIO9);
 * SCLK (GPIO11);
 * CE0 (GPIO8);
 * INT (GPIO25)
 *
 */


// Presinstalled libraries
#include <iostream>
#include <unistd.h>
#include <signal.h>

// CAN library (with mcp2515)
#include "src/mcp_can_rpi.h"

// Shows more info on the console
#define DEBUG_MODE    1

// CAN setup
#define IntPIN        24
#define CSPIN         7
#define SPIBus        0
#define CANSpeed      CAN_250KBPS
#define MCPClock      MCP_8MHZ
#define MCPMode       MCP_NORMAL

// Message to be sent
#define N             2       // Max is 8
#define EXT           1       // 1=extended, 0=normal
#define DELAY         999999 // Delay in microseconds
uint32_t id         = 0x12C;
uint8_t  data[N]    = { 0, 0 };
bool     newMessage = false;

// Auxiliary function
void detectInterrupt();
void printCANMsg();
void sendCANMsg(int n);

// New MCP_CAN instance
// MCP_CAN(int spi_channel, int spi_baudrate, INT8U gpio_can_interrupt);
MCP_CAN CAN(SPIBus, 10000000, IntPIN, CSPIN);

int main()
{
    /* -----------------------------------------------------------------
     * SETUP LOOP
     * -----------------------------------------------------------------
     */

    printf("Hello World! Program 0_basic_example.cxx is running!\n\n");
    
    printf("%d\n\n", (14<<8) + (108));
    usleep(2000000);

    // Initialize GPIO pins and SPI bus of the Raspberry Pi
    wiringPiSetup();
    CAN.setupInterruptGpio();
    CAN.setupSpi();
    printf("GPIO Pins initialized & SPI started\n");

    // Attach interrupt to read incoming messages and timer
    signal(SIGALRM, sendCANMsg);
    ualarm(DELAY, DELAY);

    wiringPiISR(IntPIN, INT_EDGE_FALLING, detectInterrupt);

    /* Start CAN bus
     * INT8U begin(INT8U idmodeset, INT8U speedset, INT8U clockset);
     */

    while (CAN_OK != CAN.begin(MCP_ANY, CANSpeed, MCPClock))
    {
        printf("CAN BUS Shield init fail\n");
        printf("Trying to init CAN BUS Shield again\n\n");
        usleep(1000000);
    }
    printf("CAN BUS Shield init ok!\n");
    CAN.setMode(MCPMode);

    while (1)
    {
        /* -----------------------------------------------------------------
         * MAIN LOOP
         * -----------------------------------------------------------------
         */

        if (newMessage)
        {
            printCANMsg();
        }
    }
    return 0;
}


void detectInterrupt()
{
    newMessage = true;
}


void printCANMsg()
{
    INT8U  len    = 0;
    INT8U  buf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    INT32U canId  = 0;

    if (CAN_MSGAVAIL == CAN.checkReceive())  // check if data coming
    {
        // INT8U MCP_CAN::readMsgBuf(INT32U *id, INT8U *len, INT8U buf[])
        // Read data rellena canId, len y guarda los datos en buf
        CAN.readMsgBuf(&canId, &len, &buf[0]);

        canId = canId & 0x1FFFFFFF;

        printf("-----------------------------\n");
        printf("Received data from ID: %lu | len:%d\n", canId, len);

        for (int i = 0; i < len; i++) // print the data
        {
            printf("(%d)", buf[i]);
            printf("\t");
        }
    }
}


void sendCANMsg(int n)
{
    printf("\n\nMessage sent: %d\n", CAN.sendMsgBuf(id+10, EXT, N, data));
}


// ---------------------------------------------------------------------------
