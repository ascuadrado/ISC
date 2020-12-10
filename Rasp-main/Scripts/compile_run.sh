#!/bin/sh

g++ -lstdc++ -lwiringPi -Wall -Wno-narrowing -o "../CAN/main.cxx" "../can_main"
sudo chmod +x "../can_main"
