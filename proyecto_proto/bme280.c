#include <xc.h>
#include "i2c.h"
#include "bme280.h"
#include "config.h"

#define BME280_ADDR 0x76  // SDO=GND -> 0x76, SDO=VCC -> 0x77

// Escritura en registro
void BME280_Write(unsigned char reg, unsigned char data) {
    I2C_Start();
    I2C_Write(BME280_ADDR << 1);  // Write
    I2C_Write(reg);
    I2C_Write(data);
    I2C_Stop();
}

// Lectura de un registro con Repeated Start correcto
unsigned char BME280_Read(unsigned char reg) {
    unsigned char data;

    I2C_Start();
    I2C_Write(BME280_ADDR << 1);
    I2C_Write(reg);
    I2C_Stop();
    __delay_us(50);          // ? agregar este delay

    I2C_Start();
    I2C_Write((BME280_ADDR << 1) | 1);
    data = I2C_Read(0);
    I2C_Stop();
    __delay_us(50);          // ? y este

    return data;
}

// Inicializacion del sensor
void BME280_Init(void) {
    __delay_ms(100);
    BME280_Write(0xF2, 0x01);  // ctrl_hum: oversampling x1
    BME280_Write(0xF5, 0xA0);  // config: standby, filtro OFF
    // No escribir ctrl_meas aqui ? se escribe en cada lectura (forced mode)
    __delay_ms(10);
}

// Lectura de humedad simplificada (valida para proyecto academico)
float BME280_ReadHumidity(void) {
    unsigned char msb, lsb;
    unsigned int raw;

    // Forzar nueva medicion
    BME280_Write(0xF2, 0x01);  // ctrl_hum: oversampling x1
    BME280_Write(0xF4, 0x25);  // forced mode ? dispara una medicion
    __delay_ms(10);            // esperar conversion

    // Leer primero temperatura (necesario para actualizar hum)
    BME280_Read(0xFA);
    BME280_Read(0xFB);
    BME280_Read(0xFC);

    // Leer humedad
    msb = BME280_Read(0xFD);
    lsb = BME280_Read(0xFE);

    raw = ((unsigned int)msb << 8) | lsb;

    float hum = (raw * 100.0) / 65535.0;
    if (hum > 100.0) hum = 100.0;
    if (hum < 0.0)   hum = 0.0;
    return hum;
}
