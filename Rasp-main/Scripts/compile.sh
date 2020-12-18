#!/bin/sh

g++ -lstdc++ -lwiringPi -Wall -Wno-narrowing "/home/pi/Desktop/Rasp-main/CAN/main.cxx" -o "/home/pi/Desktop/Rasp-main/can_main"
sudo chmod +x "/home/pi/Desktop/Rasp-main/can_main"
