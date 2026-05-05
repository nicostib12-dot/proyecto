#include <xc.h>
#include "i2c.h"
#include "bh1750.h"
#include "config.h"

// Direccion BH1750: ADDR=GND -> 0x23, ADDR=VCC -> 0x5C
#define BH1750_ADDR         0x23
#define BH1750_CONT_HRES    0x10  // Medicion continua alta resolucion

void BH1750_Init(void) {
    I2C_Start();
    I2C_Write(BH1750_ADDR << 1);
    I2C_Write(0x01);   // Power On
    I2C_Stop();
    __delay_ms(10);
}

float BH1750_ReadLux(void) {
    unsigned char msb, lsb;
    unsigned int raw;

    // Reenviar comando cada vez para forzar nueva medicion
    I2C_Start();
    I2C_Write(BH1750_ADDR << 1);
    I2C_Write(0x10);
    I2C_Stop();
    __delay_ms(180);

    // Leer resultado
    I2C_Start();
    I2C_Write((BH1750_ADDR << 1) | 1);
    msb = I2C_Read(1);
    lsb = I2C_Read(0);
    I2C_Stop();

    raw = ((unsigned int)msb << 8) | lsb;
    return (float)raw / 1.2;
}
