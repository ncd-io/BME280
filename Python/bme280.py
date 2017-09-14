import smbus

BME280_DEFAULT_ADDRESS = 0x76

# Oversampling of humidity data

BME280_CTRL_HUM_REGISTER = 0xF2
BME280_OSRS_H_0 = 0x00
BME280_OSRS_H_1 = 0x01
BME280_OSRS_H_2 = 0x02
BME280_OSRS_H_4 = 0x03
BME280_OSRS_H_8 = 0x04
BME280_OSRS_H_16 = 0x05

# Status register

BME280_STATUS_REGISTER = 0XF3

# masks

BME280_CONVERSION_RUNNING = 0X04
BME280_NVP_COPYING = 0X01

# Control Measure Register

BME280_CTRL_MEAS_REGISTER = 0XF4

# Oversampling of temperature data

BME280_OSRS_T_0 = 0x00
BME280_OSRS_T_1 = 0x20
BME280_OSRS_T_2 = 0x40
BME280_OSRS_T_4 = 0x60
BME280_OSRS_T_8 = 0x80
BME280_OSRS_T_16 = 0xA0

# Oversampling of pressure data

BME280_OSRS_P_0 = 0x00
BME280_OSRS_P_1 = 0x04
BME280_OSRS_P_2 = 0x08
BME280_OSRS_P_4 = 0x0C
BME280_OSRS_P_8 = 0x10
BME280_OSRS_P_16 = 0x14

# Settings mode

BME280_MODE_SLEEP = 0x00
BME280_MODE_FORCED = 0x01
BME280_MODE_NORMAL = 0x03

# Config register This should be written to in sleep mode

BME280_CONFIG_REGISTER = 0xF5

# Standby time for normal mode

BME280_T_SB__5 = 0x00
BME280_T_SB_62_5 = 0x20
BME280_T_SB_125 = 0x40
BME280_T_SB_250 = 0x60
BME280_T_SB_500 = 0x80
BME280_T_SB_1000 = 0xA0
BME280_T_SB_10 = 0xC0
BME280_T_SB_20 = 0xE0

# Filter settings (filtering out noise)

BME280_FILTER_COEF_0 = 0x00
BME280_FILTER_COEF_2 = 0x04
BME280_FILTER_COEF_4 = 0x08
BME280_FILTER_COEF_8 = 0x0C
BME280_FILTER_COEF_16 = 0x10

# Read registers

BME280_READ_PRESS_MSB = 0xF7
BME280_READ_PRESS_LSB = 0xF8
BME280_READ_PRESS_XLSB = 0xF9
BME280_READ_HUM_MSB = 0xFA
BME280_READ_HUM_LSB = 0xFB
BME280_READ_HUM_XLSB = 0xFC
BME280_READ_TEMP_MSB = 0xFD
BME280_READ_TEMP_LSB = 0xFE

class BME280():
    def __init__(self, smbus, kwargs = {}):
        self.__dict__.update(kwargs)
        if not hasattr(self, 'address'):
            self.address = BMG160_DEFAULT_ADDRESS
        if not hasattr(self, 'humidity_sampling_rate'):
            self.humidity_sampling_rate = BME280_OSRS_H_1
        if not hasattr(self, 'pressure_sampling_rate'):
            self.pressure_sampling_rate = BME280_OSRS_P_1
        if not hasattr(self, 'temperature_sampling_rate'):
            self.temperature_sampling_rate = BME280_OSRS_T_1
        if not hasattr(self, 'mode'):
            self.mode = BME280_MODE_NORMAL
        if not hasattr(self, 'standby_time'):
            self.standby_time = BME280_T_SB_1000
        if not hasattr(self, 'filter'):
            self.filter = BME280_FILTER_COEF_0
        self.smbus = smbus
        self.smbus.write_byte_data(self.address, BME280_CTRL_HUM_REGISTER, self.humidity_sampling_rate)
        self.smbus.write_byte_data(self.address, BME280_CTRL_MEAS_REGISTER, self.pressure_sampling_rate | self.temperature_sampling_rate | self.mode)
        self.smbus.write_byte_data(self.address, BME280_CONFIG_REGISTER, self.standby_time | self.filter)
        self.temperature_compensation = []
        self.pressure_compensation = []
        self.humidity_compensation = []
        self.get_parameters()
        
    def get_parameters(self):
        parameters = self.smbus.read_i2c_block_data(self.address, 0x88, 25)
        self.temperature_compensation.append(parameters[0] + (parameters[1] << 8))
        self.temperature_compensation.append(parameters[2] + (parameters[3] << 8))
        self.temperature_compensation.append(parameters[4] + (parameters[5] << 8))
        
        self.pressure_compensation.append(parameters[6] + (parameters[7] << 8))
        self.pressure_compensation.append(parameters[8] + (parameters[9] << 8))
        self.pressure_compensation.append(parameters[10] + (parameters[11] << 8))
        self.pressure_compensation.append(parameters[12] + (parameters[13] << 8))
        self.pressure_compensation.append(parameters[14] + (parameters[15] << 8))
        self.pressure_compensation.append(parameters[16] + (parameters[17] << 8))
        self.pressure_compensation.append(parameters[18] + (parameters[19] << 8))
        self.pressure_compensation.append(parameters[20] + (parameters[21] << 8))
        self.pressure_compensation.append(parameters[22] + (parameters[23] << 8))
        
        
        self.humidity_compensation.append(parameters[24])
        humidity_data = self.smbus.read_i2c_block_data(self.address, 0xE1, 8)
        self.humidity_compensation.append(humidity_data[0] + (humidity_data[1] << 8))
        self.humidity_compensation.append(humidity_data[2])
        self.humidity_compensation.append((humidity_data[3] << 4) + (humidity_data[4] & 0x0F))
        self.humidity_compensation.append((humidity_data[5] >> 4) + (humidity_data[6] >> 4))
        self.humidity_compensation.append(humidity_data[7])
        
        print self.temperature_compensation
        print self.pressure_compensation
        print self.humidity_compensation
        
        return self
        
    def take_readings(self, temperature_unit = 'c'):
        data = self.smbus.read_i2c_block_data(self.address, BME280_READ_PRESS_MSB, 8)
        
        press_var = (data[0] << 12) + (data[1] << 4) + (data[2] >> 4)
        temp_var = (data[3] << 12) + (data[4] << 4) + (data[5] >> 4)
        hum_var = (data[6] << 8) + data[7]
        
        tvar1 = ((float(temp_var))/16384 - (float(self.temperature_compensation[0]))/1024) * (float(self.temperature_compensation[1]))
        tvar2 = ((float(temp_var))/131072 - (float(self.temperature_compensation[0]))/8192) * ((float(temp_var))/131072 - (float(self.temperature_compensation[0]))/8192) * (float(self.temperature_compensation[2]))
        
        t_fine = int(tvar1+tvar2)
        
        temperature_base = (tvar1+tvar2) / 5120
        
        temperature = self.convert_temperature(temperature_unit, temperature_base)
        
        pressure = 0
        
        pvar1 = (t_fine/2) - 64000
        pvar2 = pvar1 * pvar1 * (float(self.pressure_compensation[5])) / 32768
        pvar2 = pvar2 + pvar1 * (float(self.pressure_compensation[4])) * 2
        pvar2 = (pvar2/4) + ((float(self.pressure_compensation[3])) * 65536)
        pvar1 = ((float(self.pressure_compensation[2])) * pvar1 * pvar1 / 524288 + (float(self.pressure_compensation[1])) * pvar1) / 524288
        pvar1 = (1 + pvar1 / 32768) * (float(self.pressure_compensation[0]))
        
        if pvar1>0:
            pressure = 1048576 - float(press_var)
            pressure = (pressure - (pvar2 / 4096)) * 6250 / pvar1
            pvar1 = (float(self.pressure_compensation[8])) * pressure * pressure / 2147483648
            pvar2 = pressure * (float(self.pressure_compensation[7])) / 32768
            pressure = pressure + (pvar1 + pvar2 + (float(self.pressure_compensation[6]))) / 16
            
        humidity = t_fine - 76800
        humidity = (hum_var - ((float(self.humidity_compensation[3])) * 64 + (float(self.humidity_compensation[4])) / 16384 * humidity)) * ((float(self.humidity_compensation[1])) / 65536 * (1 + (float(self.humidity_compensation[5])) / 67108864 * humidity * (1 + (float(self.humidity_compensation[2])) / 67108864 * humidity)));
        humidity = humidity * (1 - (float(self.humidity_compensation[0])) * humidity / 524288)
        
        if humidity > 100 :
            humidity = 100
        elif humidity < 0:
            humidity = 0
            
        return {'temperature': temperature, 'pressure': int(pressure), 'humidity': humidity}
        
    def convert_temperature(self, temperature_unit, temperature_base):
        if temperature_unit.lower() == ("c"):
            return temperature_base
        if temperature_unit.lower() == ("k"):
            return temperature_base + 273.15
        if temperature_unit.lower() == ("f"):
            return temperature_base * 1.8 + 32
        
    def read_rate(self):
        rates = {0,1,2,4,8,16}
        
#         assume the max time
        tmeas = 1.25 + (2.3 * rates[(osrs_t >> 5)]) + (2.3 * rates[(osrs_p & 0x1C) >> 2] + .5) + (2.3 * rates[osrs_h] + 0.575)

        if(mode == BME280_MODE_NORMAL):
            sb_time_modify = {BME280_T_SB__5: .5, BME280_T_SB_62_5: 62.5, BME280_T_SB_125: 125, BME280_T_SB_250: 250, BME280_T_SB_500: 500, BME280_T_SB_1000: 1000, BME280_T_SB_10: 20, BME280_T_SB_10: 20}
            tmeas += sb_time_modify[sb_time]
        hz = 1000 / tmeas
        if filter:
            hz_modifier = {BME280_FILTER_COEF_2: 2000, BME280_FILTER_COEF_4: 5000, BME280_FILTER_COEF_8: 11000, BME280_FILTER_COEF_16: 22000}
            hz = hz_modifier[filter]/hz
            return 1/hz*1000
        
