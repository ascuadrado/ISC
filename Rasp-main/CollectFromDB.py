import sqlite3
import json
from sockets.python3.client import Client
import requests

# File names
dbFile = 'ISC/Rasp-main/Database.db'
data = dict()


def configure_data():
    # Configure general dictionary
    data['general'] = dict()
    data['general']['time_stamp'] = []
    data['general']['nombre'] = []
    data['general']['date'] = []
    data['general']['time'] = []
    data['general']['allOK'] = []
    data['general']['stateOfCharge'] = []
    data['general']['sevconConnected'] = []
    data['general']['chargerConnected'] = []
    data['general']['bms1Connected'] = []
    data['general']['bms2Connected'] = []
    data['general']['bms3Connected'] = []

    # Configure the charger dictionary
    data['charger'] = dict()
    data['charger']['time_stamp'] = []
    data['charger']['nombre'] = []
    data['charger']['date'] = []
    data['charger']['time'] = []
    data['charger']['voltage'] = []
    data['charger']['curr'] = []
    data['charger']['flag0'] = []
    data['charger']['flag1'] = []
    data['charger']['flag2'] = []
    data['charger']['flag3'] = []
    data['charger']['flag4'] = []

    # Configure Sevcon Dictionary
    data['sevcon'] = dict()
    data['sevcon']['time_stamp'] = []
    data['sevcon']['nombre'] = []
    data['sevcon']['date'] = []
    data['sevcon']['time'] = []
    data['sevcon']['target_id'] = []
    data['sevcon']['ident'] = []
    data['sevcon']['target_iq'] = []
    data['sevcon']['iq'] = []
    data['sevcon']['battery_voltage'] = []
    data['sevcon']['battery_current'] = []
    data['sevcon']['line_contactor'] = []
    data['sevcon']['capacitor_voltage'] = []
    data['sevcon']['throttle_value'] = []
    data['sevcon']['target_torque'] = []
    data['sevcon']['torque'] = []
    data['sevcon']['heatsink_temp'] = []
    data['sevcon']['maximum_motor_speed'] = []
    data['sevcon']['velocity'] = []

    # Configure bms1 Dictionary
    data['bms1'] = dict()
    data['bms1']['time_stamp'] = []
    data['bms1']['nombre'] = []
    data['bms1']['date'] = []
    data['bms1']['time'] = []
    data['bms1']['voltage1'] = []
    data['bms1']['voltage2'] = []
    data['bms1']['voltage3'] = []
    data['bms1']['voltage4'] = []
    data['bms1']['voltage5'] = []
    data['bms1']['voltage6'] = []
    data['bms1']['voltage7'] = []
    data['bms1']['voltage8'] = []
    data['bms1']['voltage9'] = []
    data['bms1']['voltage10'] = []
    data['bms1']['voltage11'] = []
    data['bms1']['voltage12'] = []
    data['bms1']['temperature1'] = []
    data['bms1']['temperature2'] = []

    # Configure bms2 dictionary
    data['bms2'] = dict()
    data['bms2']['time_stamp'] = []
    data['bms2']['nombre'] = []
    data['bms2']['date'] = []
    data['bms2']['time'] = []
    data['bms2']['voltage1'] = []
    data['bms2']['voltage2'] = []
    data['bms2']['voltage3'] = []
    data['bms2']['voltage4'] = []
    data['bms2']['voltage5'] = []
    data['bms2']['voltage6'] = []
    data['bms2']['voltage7'] = []
    data['bms2']['voltage8'] = []
    data['bms2']['voltage9'] = []
    data['bms2']['voltage10'] = []
    data['bms2']['voltage11'] = []
    data['bms2']['voltage12'] = []
    data['bms2']['temperature1'] = []
    data['bms2']['temperature2'] = []

    # Configure bms3 dictionary
    data['bms3'] = dict()
    data['bms3']['time_stamp'] = []
    data['bms3']['nombre'] = []
    data['bms3']['date'] = []
    data['bms3']['time'] = []
    data['bms3']['voltage1'] = []
    data['bms3']['voltage2'] = []
    data['bms3']['voltage3'] = []
    data['bms3']['voltage4'] = []
    data['bms3']['voltage5'] = []
    data['bms3']['voltage6'] = []
    data['bms3']['voltage7'] = []
    data['bms3']['voltage8'] = []
    data['bms3']['voltage9'] = []
    data['bms3']['voltage10'] = []
    data['bms3']['voltage11'] = []
    data['bms3']['voltage12'] = []
    data['bms3']['temperature1'] = []
    data['bms3']['temperature2'] = []


def read_from_database():
    conn = sqlite3.connect(dbFile)
    c = conn.cursor()
    c.execute("SELECT * FROM general")
    rows = c.fetchall()
    for row in rows:
        data['general']['time_stamp'].append(row[0])
        data['general']['date'].append(row[1])
        data['general']['time'].append(row[2])
        data['general']['allOK'].append(row[3])
        data['general']['stateOfCharge'].append(row[4])
        data['general']['sevconConnected'].append(row[5])
        data['general']['chargerConnected'].append(row[6])
        data['general']['bms1Connected'].append(row[7])
        data['general']['bms2Connected'].append(row[8])
        data['general']['bms3Connected'].append(row[9])
        data['general']['nombre'].append('Prueba_' + row[1])

    c.execute("SELECT * FROM charger")
    rows = c.fetchall()
    for row in rows:
        data['charger']['time_stamp'].append(row[0])
        data['charger']['date'].append(row[1])
        data['charger']['time'].append(row[2])
        data['charger']['voltage'].append(row[3])
        data['charger']['curr'].append(row[4])
        data['charger']['flag0'].append(row[5])
        data['charger']['flag1'].append(row[6])
        data['charger']['flag2'].append(row[7])
        data['charger']['flag3'].append(row[8])
        data['charger']['flag4'].append(row[9])
        data['charger']['nombre'].append('Prueba_' + row[1])

    c.execute("SELECT * FROM sevcon")
    rows = c.fetchall()
    for row in rows:
        data['sevcon']['time_stamp'].append(row[0])
        data['sevcon']['date'].append(row[1])
        data['sevcon']['time'].append(row[2])
        data['sevcon']['target_id'].append(row[3])
        data['sevcon']['ident'].append(row[4])
        data['sevcon']['target_iq'].append(row[5])
        data['sevcon']['iq'].append(row[6])
        data['sevcon']['battery_voltage'].append(row[7])
        data['sevcon']['battery_current'].append(row[8])
        data['sevcon']['line_contactor'].append(row[9])
        data['sevcon']['capacitor_voltage'].append(row[10])
        data['sevcon']['throttle_value'].append(row[11])
        data['sevcon']['target_torque'].append(row[12])
        data['sevcon']['torque'].append(row[13])
        data['sevcon']['heatsink_temp'].append(row[14])
        data['sevcon']['maximum_motor_speed'].append(row[15])
        data['sevcon']['velocity'].append(row[16])
        data['sevcon']['nombre'].append('Prueba_' + row[1])

    c.execute("SELECT * FROM bms1")
    rows = c.fetchall()
    for row in rows:
        data['bms1']['time_stamp'].append(row[0])
        data['bms1']['date'].append(row[1])
        data['bms1']['time'].append(row[2])
        data['bms1']['voltage1'].append(round(row[3], 2))
        data['bms1']['voltage2'].append(round(row[4], 2))
        data['bms1']['voltage3'].append(round(row[5], 2))
        data['bms1']['voltage4'].append(round(row[6], 2))
        data['bms1']['voltage5'].append(round(row[7], 2))
        data['bms1']['voltage6'].append(round(row[8], 2))
        data['bms1']['voltage7'].append(round(row[9], 2))
        data['bms1']['voltage8'].append(round(row[10], 2))
        data['bms1']['voltage9'].append(round(row[11], 2))
        data['bms1']['voltage10'].append(round(row[12], 2))
        data['bms1']['voltage11'].append(round(row[13], 2))
        data['bms1']['voltage12'].append(round(row[14], 2))
        data['bms1']['temperature1'].append(round(row[15], 2))
        data['bms1']['temperature2'].append(round(row[16], 2))
        data['bms1']['nombre'].append('Prueba_' + row[1])

    c.execute("SELECT * FROM bms2")
    rows = c.fetchall()
    for row in rows:
        data['bms2']['time_stamp'].append(row[0])
        data['bms2']['date'].append(row[1])
        data['bms2']['time'].append(row[2])
        data['bms2']['voltage1'].append(round(row[3], 2))
        data['bms2']['voltage2'].append(round(row[4], 2))
        data['bms2']['voltage3'].append(round(row[5], 2))
        data['bms2']['voltage4'].append(round(row[6], 2))
        data['bms2']['voltage5'].append(round(row[7], 2))
        data['bms2']['voltage6'].append(round(row[8], 2))
        data['bms2']['voltage7'].append(round(row[9], 2))
        data['bms2']['voltage8'].append(round(row[10], 2))
        data['bms2']['voltage9'].append(round(row[11], 2))
        data['bms2']['voltage10'].append(round(row[12], 2))
        data['bms2']['voltage11'].append(round(row[13], 2))
        data['bms2']['voltage12'].append(round(row[14], 2))
        data['bms2']['temperature1'].append(round(row[15], 2))
        data['bms2']['temperature2'].append(round(row[16], 2))
        data['bms2']['nombre'].append('Prueba_' + row[1])

    c.execute("SELECT * FROM bms3")
    rows = c.fetchall()
    for row in rows:
        data['bms3']['time_stamp'].append(row[0])
        data['bms3']['date'].append(row[1])
        data['bms3']['time'].append(row[2])
        data['bms3']['voltage1'].append(round(row[3], 2))
        data['bms3']['voltage2'].append(round(row[4], 2))
        data['bms3']['voltage3'].append(round(row[5], 2))
        data['bms3']['voltage4'].append(round(row[6], 2))
        data['bms3']['voltage5'].append(round(row[7], 2))
        data['bms3']['voltage6'].append(round(row[8], 2))
        data['bms3']['voltage7'].append(round(row[9], 2))
        data['bms3']['voltage8'].append(round(row[10], 2))
        data['bms3']['voltage9'].append(round(row[11], 2))
        data['bms3']['voltage10'].append(round(row[12], 2))
        data['bms3']['voltage11'].append(round(row[13], 2))
        data['bms3']['voltage12'].append(round(row[14], 2))
        data['bms3']['temperature1'].append(round(row[15], 2))
        data['bms3']['temperature2'].append(round(row[16], 2))
        data['bms3']['nombre'].append('Prueba_' + row[1])

    conn.commit()
    conn.close()


if __name__ == '__main__':
    configure_data()
    read_from_database()
    # clean_database()
    data['_id'] = 'Prueba_BaseDatos'
    dataJSON = json.dumps(data)
    API_ENDPOINT = 'https://motostudent2021nodered.eu-gb.mybluemix.net/send_data'
    headers = {'Content-type': 'application/json'}
    trial_data = {'hola': 'hola_mundo',
                  }
    r = requests.post(url=API_ENDPOINT, data=dataJSON, headers=headers)
    print(r)

