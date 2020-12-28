import sqlite3

# File names
dbFile = '/home/pi/Desktop/Rasp-main/Database.db'
data = dict()


def read_from_database():
    conn = sqlite3.connect(dbFile)
    c = conn.cursor()

    data['general'] = c.execute("SELECT * FROM general")
    data['charger'] = c.execute("SELECT * FROM charger")
    data['sevcon'] = c.execute("SELECT * FROM sevcon")
    data['bms1'] = c.execute("SELECT * FROM bms1")
    data['bms2'] = c.execute("SELECT * FROM bms2")
    data['bms3'] = c.execute("SELECT * FROM bms3")

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
    read_from_database()
    clean_database()
    print(data)
