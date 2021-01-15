import sqlite3
import json

# File names
dbFile = 'ISC/Rasp-main/Database.db'
data = dict()


def configure_data():
    # Configure general dictionary
    data['general'] = dict()
    data['general']['timestamp'] = []
    data['general']['date'] = []
    data['general']['time'] = []
    data['general']['allOK'] = []

    # Configure the charger dictionary
    data['charger'] = dict()
    data['charger']['timestamp'] = []
    data['charger']['date'] = []
    data['charger']['time'] = []
    data['charger']['voltage'] = []
    data['charger']['current'] = []
    data['charger']['flag0'] = []
    data['charger']['flag1'] = []
    data['charger']['flag2'] = []
    data['charger']['flag3'] = []
    data['charger']['flag4'] = []

    # Configure Sevcon Dictionary
    data['sevcon'] = dict()
    data['sevcon']['timestamp'] = []
    data['sevcon']['date'] = []
    data['sevcon']['time'] = []
    data['sevcon']['TPD01_1'] = []

    # Configure bms1 Dictionary
    data['bms1'] = dict()
    data['bms1']['timestamp'] = []
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
    data['bms2']['timestamp'] = []
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
    data['bms3']['timestamp'] = []
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
        data['general']['timestamp'].append(row[0])
        data['general']['date'].append(row[1])
        data['general']['time'].append(row[2])
        data['general']['allOK'].append(row[3])

    c.execute("SELECT * FROM charger")
    rows = c.fetchall()
    for row in rows:
        data['charger']['timestamp'].append(row[0])
        data['charger']['date'].append(row[1])
        data['charger']['time'].append(row[2])
        data['charger']['voltage'].append(row[3])
        data['charger']['current'].append(row[4])
        data['charger']['flag0'].append(row[5])
        data['charger']['flag1'].append(row[6])
        data['charger']['flag2'].append(row[7])
        data['charger']['flag3'].append(row[8])
        data['charger']['flag4'].append(row[9])

    c.execute("SELECT * FROM sevcon")
    rows = c.fetchall()
    for row in rows:
        data['sevcon']['timestamp'].append(row[0])
        data['sevcon']['date'].append(row[1])
        data['sevcon']['time'].append(row[2])
        data['sevcon']['TPD01_1'].append(row[3])

    c.execute("SELECT * FROM bms1")
    rows = c.fetchall()
    for row in rows:
        data['bms1']['timestamp'].append(row[0])
        data['bms1']['date'].append(row[1])
        data['bms1']['time'].append(row[2])
        data['bms1']['voltage1'].append(row[3])
        data['bms1']['voltage2'].append(row[4])
        data['bms1']['voltage3'].append(row[5])
        data['bms1']['voltage4'].append(row[6])
        data['bms1']['voltage5'].append(row[7])
        data['bms1']['voltage6'].append(row[8])
        data['bms1']['voltage7'].append(row[9])
        data['bms1']['voltage8'].append(row[10])
        data['bms1']['voltage9'].append(row[11])
        data['bms1']['voltage10'].append(row[12])
        data['bms1']['voltage11'].append(row[13])
        data['bms1']['voltage12'].append(row[14])
        data['bms1']['temperature1'].append(row[15])
        data['bms1']['temperature2'].append(row[16])

    c.execute("SELECT * FROM bms2")
    rows = c.fetchall()
    for row in rows:
        data['bms2']['timestamp'].append(row[0])
        data['bms2']['date'].append(row[1])
        data['bms2']['time'].append(row[2])
        data['bms2']['voltage1'].append(row[3])
        data['bms2']['voltage2'].append(row[4])
        data['bms2']['voltage3'].append(row[5])
        data['bms2']['voltage4'].append(row[6])
        data['bms2']['voltage5'].append(row[7])
        data['bms2']['voltage6'].append(row[8])
        data['bms2']['voltage7'].append(row[9])
        data['bms2']['voltage8'].append(row[10])
        data['bms2']['voltage9'].append(row[11])
        data['bms2']['voltage10'].append(row[12])
        data['bms2']['voltage11'].append(row[13])
        data['bms2']['voltage12'].append(row[14])
        data['bms2']['temperature1'].append(row[15])
        data['bms2']['temperature2'].append(row[16])

    c.execute("SELECT * FROM bms3")
    rows = c.fetchall()
    for row in rows:
        data['bms3']['timestamp'].append(row[0])
        data['bms3']['date'].append(row[1])
        data['bms3']['time'].append(row[2])
        data['bms3']['voltage1'].append(row[3])
        data['bms3']['voltage2'].append(row[4])
        data['bms3']['voltage3'].append(row[5])
        data['bms3']['voltage4'].append(row[6])
        data['bms3']['voltage5'].append(row[7])
        data['bms3']['voltage6'].append(row[8])
        data['bms3']['voltage7'].append(row[9])
        data['bms3']['voltage8'].append(row[10])
        data['bms3']['voltage9'].append(row[11])
        data['bms3']['voltage10'].append(row[12])
        data['bms3']['voltage11'].append(row[13])
        data['bms3']['voltage12'].append(row[14])
        data['bms3']['temperature1'].append(row[15])
        data['bms3']['temperature2'].append(row[16])

    conn.commit()
    conn.close()


def clean_database():
    conn = sqlite3.connect(dbFile)
    c = conn.cursor()

    c.execute('DELETE FROM general;', )
    c.execute('DELETE FROM charger;', )
    c.execute('DELETE FROM sevcon;', )
    c.execute('DELETE FROM bms1;', )
    c.execute('DELETE FROM bms2;', )
    c.execute('DELETE FROM bms3;', )

    # commit the changes to db
    conn.commit()
    # close the connection
    conn.close()


if __name__ == '__main__':
    configure_data()
    read_from_database()
    # clean_database()
    data['_id'] = 'Prueba_BaseDatos '
    dataJSON = json.dumps(data)
    print(dataJSON)

