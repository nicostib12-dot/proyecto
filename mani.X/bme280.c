#include "bme280.h"
#include "i12c.h"          // ? nombre correcto con el 1
#include <xc.h>

static BME280_Calib calib;
static int32_t t_fine;

static void BME280_Escribir(uint8_t reg, uint8_t valor) {
    I2C_Start();
    I2C_Write((BME280_ADDR << 1) | 0x00);
    I2C_Write(reg);
    I2C_Write(valor);
    I2C_Stop();
}

static uint8_t BME280_LeerByte(uint8_t reg) {
    uint8_t dato;
    I2C_Start();
    I2C_Write((BME280_ADDR << 1) | 0x00);
    I2C_Write(reg);
    I2C_RepeatedStart();
    I2C_Write((BME280_ADDR << 1) | 0x01);
    dato = (uint8_t)I2C_Read(0);        // NACK ? último byte
    I2C_Stop();
    return dato;
}

static void BME280_LeerBytes(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t i;
    I2C_Start();
    I2C_Write((BME280_ADDR << 1) | 0x00);
    I2C_Write(reg);
    I2C_RepeatedStart();
    I2C_Write((BME280_ADDR << 1) | 0x01);
    for (i = 0; i < len; i++) {
        buf[i] = (uint8_t)I2C_Read(i == (len - 1) ? 0 : 1);
    }
    I2C_Stop();
}

static void BME280_LeerCalibracion(void) {
    uint8_t buf[26];

    BME280_LeerBytes(0x88, buf, 24);
    calib.dig_T1 = (uint16_t)(buf[1]  << 8) | buf[0];
    calib.dig_T2 = (int16_t) (buf[3]  << 8) | buf[2];
    calib.dig_T3 = (int16_t) (buf[5]  << 8) | buf[4];
    calib.dig_P1 = (uint16_t)(buf[7]  << 8) | buf[6];
    calib.dig_P2 = (int16_t) (buf[9]  << 8) | buf[8];
    calib.dig_P3 = (int16_t) (buf[11] << 8) | buf[10];
    calib.dig_P4 = (int16_t) (buf[13] << 8) | buf[12];
    calib.dig_P5 = (int16_t) (buf[15] << 8) | buf[14];
    calib.dig_P6 = (int16_t) (buf[17] << 8) | buf[16];
    calib.dig_P7 = (int16_t) (buf[19] << 8) | buf[18];
    calib.dig_P8 = (int16_t) (buf[21] << 8) | buf[20];
    calib.dig_P9 = (int16_t) (buf[23] << 8) | buf[22];

    calib.dig_H1 = BME280_LeerByte(0xA1);

    BME280_LeerBytes(0xE1, buf, 7);
    calib.dig_H2 = (int16_t)(buf[1] << 8) | buf[0];
    calib.dig_H3 = buf[2];
    calib.dig_H4 = (int16_t)(buf[3] << 4) | (buf[4] & 0x0F);
    calib.dig_H5 = (int16_t)(buf[5] << 4) | (buf[4] >> 4);
    calib.dig_H6 = (int8_t)buf[6];
}

static int32_t BME280_CompTemp(int32_t adc_T) {
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1)))
             * (int32_t)calib.dig_T2) >> 11;
    var2 = (((((adc_T >> 4) - (int32_t)calib.dig_T1)
             * ((adc_T >> 4) - (int32_t)calib.dig_T1)) >> 12)
             * (int32_t)calib.dig_T3) >> 14;
    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;
}

static uint32_t BME280_CompPresion(int32_t adc_P) {
    int32_t  var1, var2;
    uint32_t p;

    var1 = ((int32_t)t_fine >> 1) - 64000L;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int32_t)calib.dig_P5) << 1);
    var2 = (var2 >> 2) + ((int32_t)calib.dig_P4 << 16);

    // Fix warning de precedencia ? paréntesis explícitos
    var1 = ((((int32_t)calib.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
            + (((int32_t)calib.dig_P2 * var1) >> 1)) >> 18;

    var1 = ((32768L + var1) * (int32_t)calib.dig_P1) >> 15;
    if (var1 == 0) return 0;

    p = ((uint32_t)(1048576L - adc_P) - (uint32_t)(var2 >> 12)) * 3125UL;
    p = (p < 0x80000000UL) ? (p << 1) / (uint32_t)var1
                            : (p / (uint32_t)var1) * 2;

    var1 = ((int32_t)calib.dig_P9
            * (int32_t)(((p >> 3) * (p >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(p >> 2) * (int32_t)calib.dig_P8) >> 13;

    p = (uint32_t)((int32_t)p + ((var1 + var2 + calib.dig_P7) >> 4));
    return p;
}

static uint32_t BME280_CompHumedad(int32_t adc_H) {
    int32_t v;
    v = t_fine - 76800L;
    v = (((adc_H << 14) - ((int32_t)calib.dig_H4 << 20)
          - ((int32_t)calib.dig_H5 * v)) + 16384L) >> 15;
    v = v * (((((((v * (int32_t)calib.dig_H6) >> 10)
                * (((v * (int32_t)calib.dig_H3) >> 11) + 32768L)) >> 10)
               + 2097152L) * (int32_t)calib.dig_H2 + 8192L) >> 14);
    v = v - (((((v >> 15) * (v >> 15)) >> 7)
               * (int32_t)calib.dig_H1) >> 4);
    if (v < 0)          v = 0;
    if (v > 419430400L) v = 419430400L;
    return (uint32_t)((v >> 12) * 10 / 256);
}

uint8_t BME280_Init(void) {
    uint8_t id = BME280_LeerByte(BME280_REG_ID);
    if (id != 0x60) return 0;

    BME280_Escribir(BME280_REG_RESET, 0xB6);
    __delay_ms(10);

    BME280_LeerCalibracion();

    BME280_Escribir(BME280_REG_CTRL_HUM,  0x01);
    BME280_Escribir(BME280_REG_CTRL_MEAS, 0x27);
    BME280_Escribir(BME280_REG_CONFIG,    0xA0);

    __delay_ms(100);
    return 1;
}

void BME280_LeerDatos(BME280_Data *data) {
    uint8_t buf[8];
    int32_t adc_P, adc_T, adc_H;
    int32_t temp_cruda;

    BME280_LeerBytes(BME280_REG_DATA_START, buf, 8);

    adc_P = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    adc_T = ((int32_t)buf[3] << 12) | ((int32_t)buf[4] << 4) | (buf[5] >> 4);
    adc_H = ((int32_t)buf[6] << 8)  |  buf[7];

    temp_cruda        = BME280_CompTemp(adc_T);
    data->temperatura = temp_cruda / 10;
    data->presion     = BME280_CompPresion(adc_P);
    data->humedad     = BME280_CompHumedad(adc_H);
}