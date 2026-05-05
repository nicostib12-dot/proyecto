#include <xc.h>
#include "i2c.h"
#include "bh1750.h"
#include "config.h"

// Direccion BH1750: ADDR=GND -> 0x23, ADDR=VCC -> 0x5C
#define BH1750_ADDR         0x23
#define BH1750_CONT_HRES    0x10  // Medicion continua alta resolucion

void BH1750_Init(void) {
    I2C_Start();
    I2C_Write(BH1750_ADDR << 1);      // Write
    I2C_Write(BH1750_CONT_HRES);      // Comando medicion continua
    I2C_Stop();
    __delay_ms(180);  // Tiempo de medicion alta resolucion
}

float BH1750_ReadLux(void) {
    unsigned char msb, lsb;
    unsigned int raw;

    I2C_Start();
    I2C_Write((BH1750_ADDR << 1) | 1); // Read
    msb = I2C_Read(1);  // ACK
    lsb = I2C_Read(0);  // NACK — ultimo byte
    I2C_Stop();

    raw = ((unsigned int)msb << 8) | lsb;
    // Formula datasheet: lux = raw / 1.2
    return (float)raw / 1.2;
}

