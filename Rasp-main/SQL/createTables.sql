CREATE TABLE IF NOT EXISTS 'bms1' (
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,
    'voltage1' REAL NOT NULL,
    'voltage2' REAL NOT NULL,
    'voltage3' REAL NOT NULL,
    'voltage4' REAL NOT NULL,
    'voltage5' REAL NOT NULL,
    'voltage6' REAL NOT NULL,
    'voltage7' REAL NOT NULL,
    'voltage8' REAL NOT NULL,
    'voltage9' REAL NOT NULL,
    'voltage10' REAL NOT NULL,
    'voltage11' REAL NOT NULL,
    'voltage12' REAL NOT NULL,
    'temperature1' REAL NOT NULL,
    'temperature2' REAL NOT NULL
);

CREATE TABLE IF NOT EXISTS 'bms2' (
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,
    'voltage1' REAL NOT NULL,
    'voltage2' REAL NOT NULL,
    'voltage3' REAL NOT NULL,
    'voltage4' REAL NOT NULL,
    'voltage5' REAL NOT NULL,
    'voltage6' REAL NOT NULL,
    'voltage7' REAL NOT NULL,
    'voltage8' REAL NOT NULL,
    'voltage9' REAL NOT NULL,
    'voltage10' REAL NOT NULL,
    'voltage11' REAL NOT NULL,
    'voltage12' REAL NOT NULL,
    'temperature1' REAL NOT NULL,
    'temperature2' REAL NOT NULL
);

CREATE TABLE IF NOT EXISTS 'bms3' (
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,
    'voltage1' REAL NOT NULL,
    'voltage2' REAL NOT NULL,
    'voltage3' REAL NOT NULL,
    'voltage4' REAL NOT NULL,
    'voltage5' REAL NOT NULL,
    'voltage6' REAL NOT NULL,
    'voltage7' REAL NOT NULL,
    'voltage8' REAL NOT NULL,
    'voltage9' REAL NOT NULL,
    'voltage10' REAL NOT NULL,
    'voltage11' REAL NOT NULL,
    'voltage12' REAL NOT NULL,
    'temperature1' REAL NOT NULL,
    'temperature2' REAL NOT NULL
);

CREATE TABLE IF NOT EXISTS 'general' (
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,
    'total_voltage' REAL,
    'charger_voltage' REAL,
    'charger_current' REAL,
    'charger_flag1' INTEGER,
    'charger_flag2' INTEGER,
    'charger_flag3' INTEGER,
    'charger_flag4' INTEGER
);
