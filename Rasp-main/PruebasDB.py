import sqlite3
import time
import random

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


def init_dict():
    '''
    Initializes internal dictionaries (general, sevcon, charger,  bms1, bms2, bms3)

    Args:
        d: value to default all dictionaries

    '''

    d = -1
    bms1['voltages'] = [d, d, d, d, d, d, d, d, d, d, d, d]
    bms2['voltages'] = [d, d, d, d, d, d, d, d, d, d, d, d]
    bms3['voltages'] = [d, d, d, d, d, d, d, d, d, d, d, d]

    bms1['temperatures'] = [d, d]
    bms2['temperatures'] = [d, d]
    bms3['temperatures'] = [d, d]

    charger['voltage'] = d
    charger['current'] = d
    charger['flags'] = [d, d, d, d, d]

    sevcon['target_id'] = d
    sevcon['id'] = d
    sevcon['target_iq'] = d
    sevcon['iq'] = d

    sevcon['battery_voltage'] = d
    sevcon['battery_current'] = d
    sevcon['line_contactor'] = d
    sevcon['capacitor_voltage'] = d

    sevcon['throttle_value'] = d
    sevcon['target_torque'] = d
    sevcon['torque'] = d

    sevcon['heatsink_temp'] = d

    sevcon['maximum_motor_speed'] = d
    sevcon['velocity'] = d

    general['allOK'] = d
    general['stateOfCharge'] = 1
    general['opMode'] = "Testing"
    general['sevconConnected'] = 0
    general['chargerConnected'] = 0
    general['bms1Connected'] = 0
    general['bms2Connected'] = 0
    general['bms3Connected'] = 0

def generate_random_values():

    res = [random.randrange(-0.7, 0.7, 0.01) for i in range(12)]
    new_list = [x + 3.5 for x in res]
    bms1['voltages'] = new_list

    res = [random.randrange(-0.7, 0.7, 0.01) for i in range(12)]
    new_list = [x + 3.5 for x in res]
    bms2['voltages'] = new_list

    res = [random.randrange(-0.7, 0.7, 0.01) for i in range(12)]
    new_list = [x + 3.5 for x in res]
    bms3['voltages'] = new_list

    res = [random.randrange(-15, 60, 2) for i in range(2)]
    new_list = [x + 30 for x in res]
    bms1['temperatures'] = new_list

    res = [random.randrange(-15, 60, 2) for i in range(2)]
    new_list = [x + 30 for x in res]
    bms2['temperatures'] = new_list

    res = [random.randrange(-15, 60, 2) for i in range(2)]
    new_list = [x + 30 for x in res]
    bms3['temperatures'] = new_list

    sevcon['throttle_value'] = random.randrange(-6, 6, 1)
    sevcon['velocity'] = random.randrange(20, 160, 1)

def save_to_db(dataBaseName="Database.db"):
    '''
    Saves the data collected to database

    Args:
        dataBaseName: Name of the file to sqlite3 database
    '''
    conn = sqlite3.connect(dataBaseName)
    c = conn.cursor()

    general['timestamp'] = time.time()
    general['date'] = time.strftime("%Y-%m-%d")
    general['time'] = time.strftime("%H:%M:%S")

    c.execute("INSERT INTO general (timestamp, date, time, allOK, stateOfCharge,"
              "sevconConnected, chargerConnected, bms1Connected,"
              "bms2Connected, bms3Connected) VALUES (?,?,?,?,?,?,?,?,?,?)",
              (general['timestamp'], general['date'], general['time'],
               general['allOK'], general['stateOfCharge'], general['sevconConnected'],
               general['chargerConnected'], general['bms1Connected'],
               general['bms2Connected'], general['bms3Connected']))

    c.execute("INSERT INTO charger (timestamp, date, time, voltage, current,"
              " flag0, flag1, flag2, flag3, flag4) VALUES (?,?,?,?,?,?,?,?,?,?)", (
                  general['timestamp'], general['date'], general['time'],
                  charger['voltage'], charger['current'],
                  charger['flags'][0], charger['flags'][1], charger['flags'][2],
                  charger['flags'][3], charger['flags'][4]))

    c.execute("INSERT INTO sevcon (timestamp, date, time, target_id, id, "
              "target_iq, iq, battery_voltage, battery_current, line_contactor, "
              "capacitor_voltage, throttle_value, target_torque, torque, "
              "heatsink_temp, maximum_motor_speed, velocity "
              ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
              (general['timestamp'], general['date'], general['time'],
              sevcon['target_id'], sevcon['id'], sevcon['target_iq'],
              sevcon['iq'], sevcon['battery_voltage'], sevcon['battery_current'],
              sevcon['line_contactor'], sevcon['capacitor_voltage'],
              sevcon['throttle_value'], sevcon['target_torque'],
              sevcon['torque'], sevcon['heatsink_temp'],
              sevcon['maximum_motor_speed'], sevcon['velocity']))

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
    conn.close()


if __name__ == '__main__':
    init_dict()
    for i in range(300):
        time.sleep(1)
        generate_random_values()
        save_to_db()

