#include <xc.h>
#include "i2c.h"
#include "bme280.h"
#include "config.h"

#define BME280_ADDR 0x76  // SDO=GND -> 0x76, SDO=VCC -> 0x7

//-------------------------------------
// Escritura en registro
//-------------------------------------
void BME280_Write(unsigned char reg, unsigned char data)
{
    I2C_Start();
    I2C_Write(BME280_ADDR << 1); // Write
    I2C_Write(reg);
    I2C_Write(data);
    I2C_Stop();
}

//-------------------------------------
// Lectura de un registro
//-------------------------------------
unsigned char BME280_Read(unsigned char reg)
{
    unsigned char data;
    // Fase escritura: enviar registro a leer
    I2C_Start();
    I2C_Write(BME280_ADDR << 1);       // Write
    I2C_Write(reg);
    I2C_Stop();                         // Stop antes del repeated start

    __delay_us(10);

    // Fase lectura
    I2C_Start();
    I2C_Write((BME280_ADDR << 1) | 1); // Read
    data = I2C_Read(0);                 // unico byte
    I2C_Stop();

    return data;
}

//-------------------------------------
// Inicialización básica del sensor
//-------------------------------------
void BME280_Init()
{
    BME280_Write(0xF2, 0x01); // Humedad oversampling x1
    BME280_Write(0xF4, 0x27); // Temp + presión + modo normal
    BME280_Write(0xF5, 0xA0);
}

//-------------------------------------
// Lectura de humedad (simplificada)
//-------------------------------------
float BME280_ReadHumidity()
{
    unsigned int raw;
    unsigned char msb, lsb;

    msb = BME280_Read(0xFD);
    lsb = BME280_Read(0xFE);

    raw = ((unsigned int)msb << 8) | lsb;

    // Conversión simplificada (válida para proyecto académico)
    return (raw * 100.0) / 65535.0;
}
