import sqlite3
import json
import wiotp

myConfig = {
    "identity": {
        "orgId": "i47wil",
        "typeId": "Rasp",
        "deviceId": "rasp-moto",
    },
    "auth": {
        "token": "tokenmoto",
    }
}

client = wiotp.sdk.device.DeviceClient(config=myConfig)

# File names
dbFile = 'ISC/Rasp-main/Database.db'
data = dict()


def configure_data():
    data['general'] = dict()
    data['charger'] = dict()
    data['sevcon'] = dict()
    data['bms1'] = dict()
    data['bms2'] = dict()
    data['bms3'] = dict()


def read_from_database():
    conn = sqlite3.connect(dbFile)
    c = conn.cursor()
    c.execute("SELECT * FROM general")
    rows_general = c.fetchall()

    c.execute("SELECT * FROM charger")
    rows_charger = c.fetchall()

    c.execute("SELECT * FROM sevcon")
    rows_sevcon = c.fetchall()

    c.execute("SELECT * FROM bms1")
    rows_bms1 = c.fetchall()

    c.execute("SELECT * FROM bms2")
    rows_bms2 = c.fetchall()

    c.execute("SELECT * FROM bms3")
    rows_bms3 = c.fetchall()

    for i in range(len(rows_general)):

        row = rows_general[i]
        data['general']['timestamp'] = row[0]
        data['general']['date'] = row[1]
        data['general']['time'] = row[2]
        data['general']['allOK'] = row[3]

        row = rows_charger[i]
        data['charger']['timestamp'] = row[0]
        data['charger']['date'] = row[1]
        data['charger']['time'] = row[2]
        data['charger']['voltage'] = row[3]
        data['charger']['current'] = row[4]
        data['charger']['flag0'] = row[5]
        data['charger']['flag1'] = row[6]
        data['charger']['flag2'] = row[7]
        data['charger']['flag3'] = row[8]
        data['charger']['flag4'] = row[9]

        row = rows_sevcon[i]
        data['sevcon']['timestamp'] = row[0]
        data['sevcon']['date'] = row[1]
        data['sevcon']['time'] = row[2]
        data['sevcon']['TPD01_1'] = row[3]

        row = rows_bms1[i]
        data['bms1']['timestamp'] = row[0]
        data['bms1']['date'] = row[1]
        data['bms1']['time'] = row[2]
        data['bms1']['voltage1'] = row[3]
        data['bms1']['voltage2'] = row[4]
        data['bms1']['voltage3'] = row[5]
        data['bms1']['voltage4'] = row[6]
        data['bms1']['voltage5'] = row[7]
        data['bms1']['voltage6'] = row[8]
        data['bms1']['voltage7'] = row[9]
        data['bms1']['voltage8'] = row[10]
        data['bms1']['voltage9'] = row[11]
        data['bms1']['voltage10'] = row[12]
        data['bms1']['voltage11'] = row[13]
        data['bms1']['voltage12'] = row[14]
        data['bms1']['temperature1'] = row[15]
        data['bms1']['temperature2'] = row[16]

        row = rows_bms2[i]
        data['bms2']['timestamp'] = row[0]
        data['bms2']['date'] = row[1]
        data['bms2']['time'] = row[2]
        data['bms2']['voltage1'] = row[3]
        data['bms2']['voltage2'] = row[4]
        data['bms2']['voltage3'] = row[5]
        data['bms2']['voltage4'] = row[6]
        data['bms2']['voltage5'] = row[7]
        data['bms2']['voltage6'] = row[8]
        data['bms2']['voltage7'] = row[9]
        data['bms2']['voltage8'] = row[10]
        data['bms2']['voltage9'] = row[11]
        data['bms2']['voltage10'] = row[12]
        data['bms2']['voltage11'] = row[13]
        data['bms2']['voltage12'] = row[14]
        data['bms2']['temperature1'] = row[15]
        data['bms2']['temperature2'] = row[16]

        row = rows_bms3[i]
        data['bms3']['timestamp'] = row[0]
        data['bms3']['date'] = row[1]
        data['bms3']['time'] = row[2]
        data['bms3']['voltage1'] = row[3]
        data['bms3']['voltage2'] = row[4]
        data['bms3']['voltage3'] = row[5]
        data['bms3']['voltage4'] = row[6]
        data['bms3']['voltage5'] = row[7]
        data['bms3']['voltage6'] = row[8]
        data['bms3']['voltage7'] = row[9]
        data['bms3']['voltage8'] = row[10]
        data['bms3']['voltage9'] = row[11]
        data['bms3']['voltage10'] = row[12]
        data['bms3']['voltage11'] = row[13]
        data['bms3']['voltage12'] = row[14]
        data['bms3']['temperature1'] = row[15]
        data['bms3']['temperature2'] = row[16]

        # Connect
        client.connect()

        # Send Data
        dataJSON = json.dumps(data)
        client.publishEvent(eventId="status", msgFormat="json", data=dataJSON, qos=0, onPublish=None)

        # Disconnect
        client.disconnect()

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
