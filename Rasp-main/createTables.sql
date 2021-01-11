
CREATE TABLE IF NOT EXISTS 'general' (
    'timestamp' REAL NOT NULL,
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,

    'allOK' INT NOT NULL,
    'stateOfCharge' REAL NOT NULL,
    'sevconConnected' INT NOT NULL,
    'chargerConnected' INT NOT NULL,
    'bms1Connected' INT NOT NULL,
    'bms2Connected' INT NOT NULL,
    'bms3Connected' INT NOT NULL
);

CREATE TABLE IF NOT EXISTS 'charger' (
    'timestamp' REAL NOT NULL,
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,

    'voltage' REAL NOT NULL,
    'current' REAL NOT NULL,
    'flag0' INT NOT NULL,
    'flag1' INT NOT NULL,
    'flag2' INT NOT NULL,
    'flag3' INT NOT NULL,
    'flag4' INT NOT NULL
);

CREATE TABLE IF NOT EXISTS 'sevcon' (
    'timestamp' REAL NOT NULL,
    'date' DATE NOT NULL,
    'time' TIME NOT NULL,

    'target_id' REAL NOT NULL,
    'id' REAL NOT NULL,
    'target_iq' REAL NOT NULL,
    'iq' REAL NOT NULL,

    'battery_voltage' REAL NOT NULL,
    'battery_current' REAL NOT NULL,
    'line_contactor' REAL NOT NULL,
    'capacitor_voltage' REAL NOT NULL,

    'throttle_value' REAL NOT NULL,
    'target_torque' REAL NOT NULL,
    'torque' REAL NOT NULL,

    'heatsink_temp' REAL NOT NULL,

    'maximum_motor_speed' REAL NOT NULL,
    'velocity' REAL NOT NULL
);

CREATE TABLE IF NOT EXISTS 'bms1' (
    'timestamp' REAL NOT NULL,
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
    'timestamp' REAL NOT NULL,
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
    'timestamp' REAL NOT NULL,
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
