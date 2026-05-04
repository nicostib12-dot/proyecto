#ifndef BME280_H
#define BME280_H

#include <xc.h>
#include <stdint.h>

#define BME280_ADDR     0x76    // Cambiar a 0x77 si SDO = VDD

// Registros principales
#define BME280_REG_ID           0xD0
#define BME280_REG_RESET        0xE0
#define BME280_REG_CTRL_HUM     0xF2
#define BME280_REG_STATUS       0xF3
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_DATA_START   0xF7  // 8 bytes: press MSB..hum LSB

// Registros de calibración temperatura/presión
#define BME280_REG_CALIB00      0x88  // 24 bytes
// Registros de calibración humedad
#define BME280_REG_CALIB26      0xE1  // 7 bytes

// Estructura de calibración
typedef struct {
    // Temperatura
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    // Presión
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    // Humedad
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} BME280_Calib;

// Estructura de resultado final
typedef struct {
    int32_t  temperatura;   // En décimas de °C  (235 = 23.5°C)
    uint32_t presion;       // En Pa enteros      (101325 = 1013.25 hPa)
    uint32_t humedad;       // En décimas de %RH  (654 = 65.4%)
} BME280_Data;

// API pública
uint8_t  BME280_Init(void);
void     BME280_LeerDatos(BME280_Data *data);

#endif