import sqlite3


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
    charger['flags'] = [d, d, d, d, d]

    sevcon['TPDO1_1'] = d

    general['allOK'] = d


def save_to_db(general, charger, sevcon, bms1, bms2, bms3):

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
    conn.close()


if __name__ == '__main__':
    init_dict()
    save_to_db(general, charger, sevcon, bms1, bms2, bms3)
