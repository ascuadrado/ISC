#!/usr/bin/python3

# Imports
import can
import time
import threading
import sqlite3

# File names
dbFile = '/home/pi/Desktop/Rasp-main/Database.db'

# Data Setup
timer0_period = 1  # seconds
general = dict()
charger = dict()
sevcon = dict()
bms1 = dict()
bms2 = dict()
bms3 = dict()
bms = [bms1, bms2, bms3]

# CAN Setup
chargerID = 0x1806E7F4
print("Welcome to CANCollect.py")
can_interface = 'can0'
bus = can.interface.Bus(can_interface, bustype='socketcan_native')

# Timers


def init_dict():
    d = -1  # default value
    emptyV = [d, d, d, d, d, d, d, d, d, d, d, d]
    emptyT = [d, d]
    bms1['voltages'] = emptyV
    bms2['voltages'] = emptyV
    bms3['voltages'] = emptyV
    bms1['temperatures'] = emptyT
    bms2['temperatures'] = emptyT
    bms3['temperatures'] = emptyT

    charger['voltage'] = d
    charger['current'] = d
    charger['flags'] = [d,d,d,d,d]

    sevcon['TPDO1_1'] = d

    general['allOK'] = d


def save_to_db(general, charger, sevcon, bms1, bms2, bms3):
    print(general)
    print(charger)
    print(sevcon)
    print(bms1)
    print(bms2)
    print(bms3)
    conn = sqlite3.connect(dbFile)
    c = conn.cursor()

    c.execute("INSERT INTO general (timestamp, date, time, allOK) VALUES (?,?,?,?)",
              (general['timestamp'], general['date'], general['time'],
               general['allOK']))

    c.execute("INSERT INTO charger (timestamp, date, time, voltage, current,"
              " flag0, flag1, flag2, flag3, flag4) VALUES (?,?,?,?,?,?,?,?,?,?)", (
                  general['timestamp'], general['date'], general['time'],
                  charger['voltage'], charger['current'],
                  charger['flags'][0], charger['flags'][1], charger['flags'][2],
                  charger['flags'][3], charger['flags'][4]))

    c.execute("INSERT INTO sevcon (timestamp, date, time, TPDO1_1"
              ") VALUES (?,?,?,?)", (general['timestamp'], general['date'],
                                     general['time'], sevcon['TPDO1_1']))

    c.execute("INSERT INTO bms1 (timestamp, date, time, voltage1, voltage2, "
              "voltage3, voltage4, voltage5, voltage6, voltage7, voltage8, "
              "voltage9, voltage10, voltage11, voltage12, temperature1, "
              "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", (
                  general['timestamp'], general['date'], general['time'],
                  bms1['voltages'][0], bms1['voltages'][1], bms1['voltages'][2],
                  bms1['voltages'][3], bms1['voltages'][4], bms1['voltages'][5],
                  bms1['voltages'][6], bms1['voltages'][7], bms1['voltages'][8],
                  bms1['voltages'][9], bms1['voltages'][10], bms1['voltages'][11],
                  bms1['temperatures'][0], bms1['temperatures'][1]))

    c.execute("INSERT INTO bms2 (timestamp, date, time, voltage1, voltage2, "
              "voltage3, voltage4, voltage5, voltage6, voltage7, voltage8, "
              "voltage9, voltage10, voltage11, voltage12, temperature1, "
              "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", (
                  general['timestamp'], general['date'], general['time'],
                  bms2['voltages'][0], bms2['voltages'][1], bms2['voltages'][2],
                  bms2['voltages'][3], bms2['voltages'][4], bms2['voltages'][5],
                  bms2['voltages'][6], bms2['voltages'][7], bms2['voltages'][8],
                  bms2['voltages'][9], bms2['voltages'][10], bms2['voltages'][11],
                  bms2['temperatures'][0], bms2['temperatures'][1]))

    c.execute("INSERT INTO bms3 (timestamp, date, time, voltage1, voltage2, "
              "voltage3, voltage4, voltage5, voltage6, voltage7, voltage8, "
              "voltage9, voltage10, voltage11, voltage12, temperature1, "
              "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", (
                  general['timestamp'], general['date'], general['time'],
                  bms3['voltages'][0], bms3['voltages'][1], bms3['voltages'][2],
                  bms3['voltages'][3], bms3['voltages'][4], bms3['voltages'][5],
                  bms3['voltages'][6], bms3['voltages'][7], bms3['voltages'][8],
                  bms3['voltages'][9], bms3['voltages'][10], bms3['voltages'][11],
                  bms3['temperatures'][0], bms3['temperatures'][1]))

    conn.commit()


def timer0():
    t = time.time()
    print("Saving to DB")
    general['timestamp'] = time.time()
    general['date'] = time.strftime("%Y-%m-%d")
    general['time'] = time.strftime("%H:%M:%S")
    save_to_db(general, charger, sevcon, bms1, bms2, bms3)
    init_dict()
    print("DB took: " + str(1000*(time.time() - t)) + 'ms')
    threading.Timer(timer0_period, timer0).start()


# Script execution
init_dict()
timer0()
while True:
    #t = time.time()
    msg = bus.recv()
    id = msg.arbitration_id
    if id > 300 and id < 400: # BMS
        n = int((id-300)/10) # BMS number (0-2) -> 3 bms modules
        m = (id-300-n*10-1) # Message number (0-3) -> 4 different messages

        if m>=0 and m<3: # voltage frame
            for i in range(4): # we read 4 cell voltages
                bms[n]['voltages'][4*m+i] = (msg.data[2*i]<<8) + msg.data[2*i+1]
        else: # temperature frame
            for i in range(2): # we read 2 temperatures
                bms[n]['temperatures'][i] = msg.data[i]-40

    elif id == chargerID: # CHARGER
        print(msg)
        charger['voltage'] = ((msg.data[0]<<8) + msg.data[1])/10
        charger['current'] = ((msg.data[2]<<8) + msg.data[3])/10
        charger['flags'][0] = msg.data[4]>>7 & 0x01
        charger['flags'][1] = msg.data[4]>>6 & 0x01
        charger['flags'][2] = msg.data[4]>>5 & 0x01
        charger['flags'][3] = msg.data[4]>>4 & 0x01
        charger['flags'][4] = msg.data[4]>>3 & 0x01

    #print("MSG took: " + str(1000*(time.time() - t)) + 'ms')
