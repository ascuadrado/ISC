# %% Definitions

import sqlite3
import time
import json

t = time.time() # Measure how long the script takes to run

# Create the connecection to the database
dbFile = '/home/pi/Desktop/Rasp-main/Database.db'
jsonFile = '/home/pi/Desktop/Rasp-main/datos.json'

#dbFile = 'Rasp-main/SQL/DataBase.db'
#jsonFile = 'Rasp-main/SQL/datos.json'

conn = sqlite3.connect(dbFile)
c = conn.cursor()

def collect_data(jsonFile):
    """
    Function to collect data from JSON file and store it in python dictionary

    Args:
        jsonFile contains the dictionary with BMS data

    Returns:
        bms1: dictionary with the data collected from the first bms
        bms2: dictionary with the data collected from the second bms
        bms1: dictionary with the data collected from the third bms
        general: dictionary with the data collected from the general parameters
    """

    with open(jsonFile) as f:
        data = json.load(f)

    date = time.strftime("%Y-%m-%d")  # Current date
    t = time.strftime("%H:%M:%S")  # Current time

    general = dict()
    charger = dict()
    sevcon = dict()
    bms1 = dict()
    bms2 = dict()
    bms3 = dict()

    bms1['voltages'] = data['BMS'][0]['cellVoltagemV']
    bms1['temperatures'] = data['BMS'][0]['temperatures']

    bms2['voltages'] = data['BMS'][1]['cellVoltagemV']
    bms2['temperatures'] = data['BMS'][1]['temperatures']

    bms3['voltages'] = data['BMS'][2]['cellVoltagemV']
    bms3['temperatures'] = data['BMS'][2]['temperatures']

    charger['voltage'] = data['CHARGER']['Vtotal']
    charger['current'] = data['CHARGER']['Icharge']
    charger['flags'] = data['CHARGER']['flags']

    sevcon['TPDO1_1'] = data['SEVCON']['TPDO1_1']

    general['allOK'] = data['allOK']
    general['timestamp'] = time.time()
    general['date'] = date
    general['time'] = t

    return general, charger, sevcon, bms1, bms2, bms3

# %% Main execution

# Collecting data


general, charger, sevcon, bms1, bms2, bms3 = collect_data(jsonFile)

c.execute("INSERT INTO general (timestamp, date, time, allOK) VALUES (?,?,?,?)",
        (general['timestamp'], general['date'], general['time'],
        general['allOK']))

c.execute("INSERT INTO charger (timestamp, date, time, voltage, current,"
        " flag0, flag1, flag2, flag3, flag4) VALUES (?,?,?,?,?,?,?,?,?,?)",(
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
        "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",(
         general['timestamp'], general['date'], general['time'],
         bms1['voltages'][0], bms1['voltages'][1], bms1['voltages'][2],
         bms1['voltages'][3], bms1['voltages'][4], bms1['voltages'][5],
         bms1['voltages'][6], bms1['voltages'][7], bms1['voltages'][8],
         bms1['voltages'][9], bms1['voltages'][10], bms1['voltages'][11],
         bms1['temperatures'][0], bms1['temperatures'][1]))

c.execute("INSERT INTO bms2 (timestamp, date, time, voltage1, voltage2, "
        "voltage3, voltage4, voltage5, voltage6, voltage7, voltage8, "
        "voltage9, voltage10, voltage11, voltage12, temperature1, "
        "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",(
         general['timestamp'], general['date'], general['time'],
         bms2['voltages'][0], bms2['voltages'][1], bms2['voltages'][2],
         bms2['voltages'][3], bms2['voltages'][4], bms2['voltages'][5],
         bms2['voltages'][6], bms2['voltages'][7], bms2['voltages'][8],
         bms2['voltages'][9], bms2['voltages'][10], bms2['voltages'][11],
         bms2['temperatures'][0], bms2['temperatures'][1]))

c.execute("INSERT INTO bms3 (timestamp, date, time, voltage1, voltage2, "
        "voltage3, voltage4, voltage5, voltage6, voltage7, voltage8, "
        "voltage9, voltage10, voltage11, voltage12, temperature1, "
        "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",(
         general['timestamp'], general['date'], general['time'],
         bms3['voltages'][0], bms3['voltages'][1], bms3['voltages'][2],
         bms3['voltages'][3], bms3['voltages'][4], bms3['voltages'][5],
         bms3['voltages'][6], bms3['voltages'][7], bms3['voltages'][8],
         bms3['voltages'][9], bms3['voltages'][10], bms3['voltages'][11],
         bms3['temperatures'][0], bms3['temperatures'][1]))

# c.execute("INSERT INTO charger (timestamp, date, time) VALUES (?,?,?)", (general['timestamp'], general['date'], general['time']))

conn.commit()

#print("Time in secs: " + str(time.time()-t))
