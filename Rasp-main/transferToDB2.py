# Load The Appropriate Python Modules
import sys         # Provides Information About Python Interpreter Constants, Functions, & Methods
import ibm_db      # Contains The APIs Needed To Work With Db2 Databases
# from ibm_db_tools import Db2ConnectionMgr
# from ipynb_exit import exit
import sqlite3

# Define And Initialize The Appropriate Variables
dbName = "BLUDB"
userID = "nps94705"
passWord = "39pqv7xm7v79^2mh"
svrConnection = None
clientInfo = False

dbFile = 'ISC/Rasp-main/Database.db'


def transfer_to_database():
    # Activate connection between databases
    conn_sqlite = sqlite3.connect(dbFile)
    conn_db2 = ibm_db.connect(
        "DATABASE=BLUDB;HOSTNAME=dashdb-txn-sbox-yp-lon02-02.services.eu-gb.bluemix.net;PORT=50000;PROTOCOL=TCPIP;UID=nps94705;PWD=39pqv7xm7v79^2mh;",
        "", "")

    conn_db2.openConnection()
    curs = conn_db2.cursor()

    c = conn_sqlite.cursor()
    c.execute("SELECT * FROM general")
    rows = c.fetchall()
    for row in rows:
        '''
        0: timestamp,
        1: allok
        2: stateofcharge
        3: sevconnected
        4: chargerconnected
        5: bms1connected
        6: bms2connected
        6: bms2connected'''
        curs.execute("insert into generalparams values (?, ?, ?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2], row[3], row[4],
                                                                    row[0], row[6], row[7]))

    c.execute("SELECT * FROM charger")
    rows = c.fetchall()
    for row in rows:
        curs.execute("insert into charger values (?, ?, ?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2], row[3], row[4],
                                                              row[0], row[6], row[7]))

    c.execute("SELECT * FROM sevcon")
    rows = c.fetchall()
    for row in rows:
        curs.execute("insert into sevcon values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2],
                                                                                                 row[3], row[4], row[0],
                                                                                                 row[6], row[7], row[8],
                                                                                                 row[9], row[10],
                                                                                                 row[11], row[12],
                                                                                                 row[13], row[14]))

    c.execute("SELECT * FROM bms1")
    rows = c.fetchall()
    for row in rows:
        curs.execute("insert into bms1 values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2],
                                                                                                 row[3], row[4], row[0],
                                                                                                 row[6], row[7], row[8],
                                                                                                 row[9], row[10],
                                                                                                 row[11], row[12],
                                                                                                 row[13], row[14]))

    c.execute("SELECT * FROM bms2")
    rows = c.fetchall()
    for row in rows:
        curs.execute("insert into bms2 values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2],
                                                                                                 row[3], row[4], row[0],
                                                                                                 row[6], row[7], row[8],
                                                                                                 row[9], row[10],
                                                                                                 row[11], row[12],
                                                                                                 row[13], row[14]))

    c.execute("SELECT * FROM bms1")
    rows = c.fetchall()
    for row in rows:
        curs.execute("insert into bms2 values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (row[0], row[1], row[2],
                                                                                                 row[3], row[4], row[0],
                                                                                                 row[6], row[7], row[8],
                                                                                                 row[9], row[10],
                                                                                                 row[11], row[12],
                                                                                                 row[13], row[14]))
    # Close The Server Connection That Was Opened Earlier
    conn_sqlite.commit()
    conn_sqlite.close()
    conn_db2.closeConnection()

# Create An Instance Of The Db2ConnectionMgr Class And Use It To Connect To A Db2 Server
# conn = Db2ConnectionMgr('SERVER', dbName, '', '', userID, passWord)


if __name__ == '__main__':
    transfer_to_database()

