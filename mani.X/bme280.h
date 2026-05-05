#ifndef BME280_H
#define BME280_H

#include <xc.h>
#define _XTAL_FREQ 8000000UL

// Direccion I2C ? cambia a 0xEE si tu modulo usa 0x77
#define BME280_ADDR         0xEC    // 0x76 << 1

// Registros principales
#define BME280_REG_ID       0xD0    // Chip ID ? debe leer 0x60
#define BME280_REG_RESET    0xE0    // Reset del sensor
#define BME280_REG_CTRL_HUM 0xF2    // Control humedad
#define BME280_REG_CTRL_MSR 0xF4    // Control medicion
#define BME280_REG_CONFIG   0xF5    // Configuracion
#define BME280_REG_HUM_MSB  0xFD    // Dato humedad crudo (2 bytes)
#define BME280_REG_TEMP_MSB 0xFA    // Dato temperatura crudo (3 bytes)

// Registros coeficientes calibracion humedad
#define BME280_REG_DIG_H1   0xA1
#define BME280_REG_DIG_H2   0xE1    // H2 al H6 estan seguidos desde aqui

// Registros coeficientes calibracion temperatura
#define BME280_REG_DIG_T1   0x88    // T1 al T3 estan seguidos desde aqui

unsigned char BME280_Init(void);
float         BME280_ReadHumidity(void);

#endif