import sqlite3
import time

# Create the connecection to the database
conn = sqlite3.connect('../../DataBaseMoto.db')
c = conn.cursor()


def collect_data():
    """
    Function to collect data from the bms and store it in the appropiate dictionary

    Args:
        En caso de que haya que pasarle algo a la funcion

    Returns:
        bms1: dictionary with the data collected from the first bms
        bms2: dictionary with the data collected from the second bms
        bms1: dictionary with the data collected from the third bms
        general: dictionary with the data collected from the general parameters
    """

    "Aqui va el codigo de alberto para recoger los datos que puede venir en forma de diccionario"
    #TODO: Función para recoger los datos de los bms y meterlos en diccionarios (Alberto jr.)

    bms1 = dict()
    bms2 = dict()
    bms3 = dict()
    general = dict()


    # Ejemplos de datos para meter en los diccionarios

    temperature = 35.5
    voltage1 = 15.2
    voltage2 = 14.8
    date = time.strftime("%Y-%m-%d")  # Current date
    t = time.strftime("%H:%M:%S")  # Current time

    return bms1, bms2, bms3, general


# Collecting data
bms1, bms2, bms3, general = collect_data()

c.execute("INSERT INTO bms1 (date, time, voltage1, voltage2, voltage3, voltage4, voltage5, voltage6, voltage7,"
          "voltage8, voltage9, voltage10, voltage11, voltage12, temperature1, "
          "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
          (bms1['date'], bms1['time'], bms1['voltages'][0], bms1['voltages'][1], bms1['voltages'][2],
           bms1['voltages'][3], bms1['voltages'][4], bms1['voltages'][5], bms1['voltages'][6], bms1['voltages'][7],
           bms1['voltages'][8], bms1['voltages'][9], bms1['voltages'][10], bms1['voltages'][11], bms1['temp1'],
           bms1['temp2']))


c.execute("INSERT INTO bms2 (date, time, voltage1, voltage2, voltage3, voltage4, voltage5, voltage6, voltage7,"
          "voltage8, voltage9, voltage10, voltage11, voltage12, temperature1, "
          "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
          (bms2['date'], bms2['time'], bms2['voltages'][0], bms2['voltages'][1], bms2['voltages'][2],
           bms2['voltages'][3], bms2['voltages'][4], bms2['voltages'][5], bms2['voltages'][6], bms2['voltages'][7],
           bms2['voltages'][8], bms2['voltages'][9], bms2['voltages'][10], bms2['voltages'][11], bms2['temp1'],
           bms2['temp2']))

c.execute("INSERT INTO bms3 (date, time, voltage1, voltage2, voltage3, voltage4, voltage5, voltage6, voltage7,"
          "voltage8, voltage9, voltage10, voltage11, voltage12, temperature1, "
          "temperature2) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
          (bms3['date'], bms3['time'], bms3['voltages'][0], bms3['voltages'][1], bms3['voltages'][2],
           bms3['voltages'][3], bms3['voltages'][4], bms3['voltages'][5], bms3['voltages'][6], bms3['voltages'][7],
           bms3['voltages'][8], bms3['voltages'][9], bms3['voltages'][10], bms3['voltages'][11], bms3['temp1'],
           bms3['temp2']))

c.execute("INSERT INTO general (date, time, total_voltage, charger_voltage, charger_current, charger_flag1, "
          "charger_flag2, charger_flag3, charger_flag4 VALUES (?,?,?,?,?,?,?,?,?)",
          (general['date'], general['time'], general['total_voltage'], general['charger_voltage'],
           general['charger_current'], general['charger_flag1'], general['charger_flag1'], general['charger_flag1'],
           general['charger_flag1']))

conn.commit()