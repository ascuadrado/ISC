import sqlite3
import json
import time

dbFile = 'Database.db'
DBDelay = 2 # seconds
chargerID = 0x1806E7F4
rpmToSpeedMultiplier = 1
maxVoltage = 4.2*30
minVoltage = 2.8*30 #
soc_method = 0 # 0=battery voltage, 1=integrate current loss
battery_capacity = 70*1000 # Milliamps-hour

class ISCData:
    general = dict()
    charger = dict()
    sevcon = dict()
    bms1 = dict()
    bms2 = dict()
    bms3 = dict()
    bms = []

    dischargeIntegrator = []

    def __init__(self):
        self.bms.append(self.bms1)
        self.bms.append(self.bms2)
        self.bms.append(self.bms3)
        self.init_dict(0)

    def init_dict(self, d):
        '''
        Initializes internal dictionaries (general, sevcon, charger,  bms1, bms2, bms3)

        Args:
            d: value to default all dictionaries

        '''
        self.bms1['voltages'] = [d,d,d,d,d,d,d,d,d,d,d,d]
        self.bms2['voltages'] = [d,d,d,d,d,d,d,d,d,d,d,d]
        self.bms3['voltages'] = [d,d,d,d,d,d,d,d,d,d,d,d]

        self.bms1['temperatures'] = [d,d]
        self.bms2['temperatures'] = [d,d]
        self.bms3['temperatures'] = [d,d]

        self.charger['voltage'] = d
        self.charger['current'] = d
        self.charger['flags'] = [d,d,d,d,d]

        self.sevcon['target_id'] = d
        self.sevcon['id'] = d
        self.sevcon['target_iq'] = d
        self.sevcon['iq'] = d

        self.sevcon['battery_voltage'] = d
        self.sevcon['battery_current'] = d
        self.sevcon['line_contactor'] = d
        self.sevcon['capacitor_voltage'] = d

        self.sevcon['throttle_value'] = d
        self.sevcon['target_torque'] = d
        self.sevcon['torque'] = d

        self.sevcon['heatsink_temp'] = d

        self.sevcon['maximum_motor_speed'] = d
        self.sevcon['velocity'] = d

        self.general['allOK'] = d
        self.general['stateOfCharge'] = 1
        self.general['opMode'] = "Testing"
        self.general['sevconConnected'] = 0
        self.general['chargerConnected'] = 0
        self.general['bms1Connected'] = 0
        self.general['bms2Connected'] = 0
        self.general['bms3Connected'] = 0

    def save_to_db(self, dataBaseName="Database.db"):
        '''
        Saves the data collected to database

        Args:
            dataBaseName: Name of the file to sqlite3 database
        '''
        conn = sqlite3.connect(dataBaseName)
        c = conn.cursor()

        self.general['timestamp'] = time.time()
        self.general['date'] = time.strftime("%Y-%m-%d")
        self.general['time'] = time.strftime("%H:%M:%S")
        self.calculate_new_soc(method=soc_method)

        general = self.general
        charger = self.charger
        sevcon = self.sevcon
        bms1 = self.bms1
        bms2 = self.bms2
        bms3 = self.bms3

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

    def connected_systems_to_default(self):
        self.general['sevconConnected'] = 0
        self.general['chargerConnected'] = 0
        self.general['bms1Connected'] = 0
        self.general['bms2Connected'] = 0
        self.general['bms3Connected'] = 0

    def interpret_can_msg(self, bus, id, msg):
        if bus == 0: # CAN0 bus (BMS & Charger)
            if (id > 300) and (id < 400): # BMS
                n = int((id-300)/10) # BMS number (0-2) -> 3 bms modules
                m = (id-300-n*10-1) # Message number (0-3) -> 4 different messages

                # Show that a certain bms is connected
                self.general['bms'+str(n)+'Connected'] = 1

                if m>=0 and m<3: # voltage frame
                    for i in range(4): # we read 4 cell voltages
                        self.bms[n]['voltages'][4*m+i] = ((msg[2*i]<<8) + msg[2*i+1])*1.0/1000
                else: # temperature frame
                    for i in range(2): # we read 2 temperatures
                        self.bms[n]['temperatures'][i] = msg[i]-40

            elif id == chargerID: # CHARGER
                self.general['chargerConnected'] = 1

                self.charger['voltage'] = ((msg[0]<<8) + msg[1])/10
                self.charger['current'] = ((msg[2]<<8) + msg[3])/10
                self.charger['flags'][0] = (msg[4]>>7) & 0x01
                self.charger['flags'][1] = (msg[4]>>6) & 0x01
                self.charger['flags'][2] = (msg[4]>>5) & 0x01
                self.charger['flags'][3] = (msg[4]>>4) & 0x01
                self.charger['flags'][4] = (msg[4]>>3) & 0x01

        elif bus == 1: # CAN1 bus (SEVCON)
            self.general['sevconConnected'] = 1
            if id == 0x101:
                self.sevcon['target_id'] = ((msg[1]<<8)+msg[0])*0.0625
                self.sevcon['id'] = ((msg[3]<<8)+msg[2])*0.0625
                self.sevcon['target_iq'] = ((msg[5]<<8)+msg[4])*0.0625
                self.sevcon['iq'] = ((msg[7]<<8)+msg[6])*0.0625
            elif id == 0x102:
                self.sevcon['battery_voltage'] = ((msg[1]<<8)+msg[0])*0.0625
                self.sevcon['battery_current'] = ((msg[3]<<8)+msg[2])*0.0625
                self.dischargeIntegrator.append(self.sevcon['battery_current'])
                self.sevcon['line_contactor'] = ((msg[5]<<8)+msg[4])*1.0
                self.sevcon['capacitor_voltage'] = ((msg[7]<<8)+msg[6])*0.0625
            elif id == 0x103:
                self.sevcon['throttle_value'] = ((msg[1]<<8)+msg[0])*1.0/32767
                self.sevcon['target_torque'] = ((msg[3]<<8)+msg[2])*0.1/100
                self.sevcon['torque'] = ((msg[5]<<8)+msg[4])*0.1/100
            elif id == 0x104:
                self.sevcon['heatsink_temp'] = msg[0]
            elif id == 0x105:
                self.sevcon['maximum_motor_speed'] = (msg[3]<<24)+(msg[2]<<16)+(msg[1]<<8)+msg[0]
                self.sevcon['velocity'] =(msg[7]<<24)+(msg[6]<<16)+(msg[5]<<8)+msg[4]

    def fetch_processing_data(self, msg):
        replyDict = dict()

        # Calculate values
        v = []
        t = []
        for b in self.bms:
            v+=(b.get('voltages'))
            t+=(b.get('temperatures'))
        maxCellV = max(v)
        minCellV = min(v)
        maxTemp = max(t)

        # Save values
        replyDict['vTotal'] = sum(v)
        replyDict['speed'] = self.sevcon.get("velocity")*rpmToSpeedMultiplier
        replyDict['torque'] = self.sevcon.get("torque")
        replyDict['throttle'] = self.sevcon.get("throttle_value")
        replyDict['stateOfCharge'] = self.general.get("stateOfCharge")
        replyDict['opMode'] = self.general.get("opMode")

        replyDict['maxCellV'] = maxCellV
        replyDict['minCellV'] = minCellV
        replyDict['maxTemp'] = maxTemp
        replyDict['capacitorV'] = self.sevcon.get("capacitor_voltage")
        replyDict['velocity'] = self.sevcon.get("velocity")
        replyDict['current'] = self.sevcon.get("battery_current")

        reply = json.dumps(replyDict) + '\n'
        return reply.encode()

    def calculate_new_soc(self, method=0):
        if method == 0:
            v = []
            for b in self.bms:
                v+=(b.get('voltages'))
            print(sum(v))
            self.general['stateOfCharge'] = 1.0*(float(sum(v))-minVoltage)/(maxVoltage-minVoltage)
        elif method == 1:
            new_discharge = sum(dischargeIntegrator)
            self.general['stateOfCharge'] -= new_discharge/battery_capacity
