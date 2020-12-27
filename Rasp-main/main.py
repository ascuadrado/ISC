#!/usr/bin/python3
'''
This python program manages all can communications
Tasks of the program:
1. Receive CAN messages
2. Periodically write to database
3. Respond to Processing queries

1. Receive CAN messages
    First thread is in charge of reading CAN messages, interpreting them and
    saving the information in the data dictionaries (general, charger, sevcon,
    bms1, bms2, bms3). It does not need to send any message since this is done
    by the Arduino MEGA

2. Periodically write to Database
    Write entries for all the tables (general, charger, sevcon, bms1, bms2,
    bms3) in the SQLite database periodically.

'''

# External Imports
#import can
import time
import threading
import sqlite3
import socket
import os
import json

# Setup
dbFile = 'Database.db'
DBDelay = 2 # seconds

# Data
general = dict()
charger = dict()
sevcon = dict()
bms1 = dict()
bms2 = dict()
bms3 = dict()
bms = [bms1, bms2, bms3]

# Auxiliary functions
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
    #print(general)
    #print(charger)
    #print(sevcon)
    #print(bms1)
    #print(bms2)
    #print(bms3)
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

def fetch_data(s):
    s = s.decode()
    replyDict = dict()
    replyDict['vtotal'] = 0
    for b in bms:
        replyDict['vtotal'] += sum(b['voltages'])
    replyDict['power'] = 0.5
    replyDict['speed'] = 119.12
    reply = json.dumps(replyDict) + '\n'
    return reply.encode()


# Daemons
def canManager():
    print("Can manager online")
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

def dbManager():
    print("Database manager online")
    while True:
        t = time.time()
        general['timestamp'] = t
        general['date'] = time.strftime("%Y-%m-%d")
        general['time'] = time.strftime("%H:%M:%S")
        save_to_db(general, charger, sevcon, bms1, bms2, bms3)
        init_dict() # Reset dictionary to defaults to check if any system is disconnected
        time.sleep(2-t) # Avoids time drifting (although we wouldn't mind)


def serverManager():
    print("Server manager online")
    HOST = ''
    PORT = 50007
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)
    while True:
        try:
            conn, addr = s.accept()
            print('Connected by', addr)
            while True:
                reply = fetch_data(conn.recv(1024))
                conn.send(reply)
            conn.close()
        except:
            print("Connection closed")



# Main execution
if __name__ == "__main__":
    # Create database if it does not exist
    if not os.path.exists(dbFile):
        print("No database, creating one")
        os.system('sqlite3 "Database.db" < "createTables.sql"')

    # Set dictionaries to default values
    init_dict()

    # Create threads for CAN, Processing Server and DB management
    canM = threading.Thread(target=canManager, daemon=True)
    dbM = threading.Thread(target=dbManager, daemon=True)
    serverM = threading.Thread(target=serverManager, daemon=True)
    #canM.start()
    dbM.start()
    serverM.start()

    while True:
        # Just wait while daemons do everything
        time.sleep(1)



    # End of file















    #
