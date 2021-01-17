import sqlite3

# File names
dbFile = 'ISC/Rasp-main/Database.db'


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
    clean_database()