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

3. Respond to Processing queries
    The display will be shown through Processing. A socket connection is
    established to share data between this program and the processing sketch
    The processing sketch will send a message to this program and a json String
    will be sent back: {"vtotal": 110.2, "power": 0.5, "speed": 119.3}
    vtotal in volts, power in (0-1) and speed in km/h

'''

# External Imports
#import can
import threading
import socket
import os
import time

# Internal imports
import configFile

# Data
data = configFile.ISCData()

# Daemons
def canManager():
    print("Can manager online")
    bus = can.interface.Bus('can0', bustype='socketcan_native')
    while True:
        msg = bus.recv()
        data.interpret_can_msg()


def dbManager():
    print("Database manager online")
    while True:
        time.sleep(configFile.DBDelay)
        data.save_to_db()
        data.connected_systems_to_default()

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
                reply = data.fetch_processing_data(conn.recv(1024))
                conn.send(reply)
            conn.close()
        except:
            print("Connection closed")



# Main execution
if __name__ == "__main__":
    # Create database if it does not exist
    if not os.path.exists(configFile.dbFile):
        print("No database, creating one")
        os.system('sqlite3 "Database.db" < "createTables.sql"')
    else:
        pass
    # Create threads for CAN, Processing Server and DB management
    canM = threading.Thread(target=canManager, daemon=True)
    dbM = threading.Thread(target=dbManager, daemon=True)
    serverM = threading.Thread(target=serverManager, daemon=True)
    # And start those threads
    #canM.start()
    dbM.start()
    serverM.start()

    while True:
        # Just wait while daemons do everything
        time.sleep(1)



# End of file









#
