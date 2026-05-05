#ifndef BH1750_H
#define BH1750_H

#include <xc.h>
#define _XTAL_FREQ 8000000UL

// Direccion I2C ? ADDR pin a GND
// Cambia a 0xB8 si ADDR esta a VCC
#define BH1750_ADDR         0x46    // 0x23 << 1

// Comandos del sensor
#define BH1750_POWER_ON     0x01    // Enciende el sensor
#define BH1750_RESET        0x07    // Limpia registro de datos
#define BH1750_CONT_H_RES   0x10    // Modo continuo alta resolucion (1 lux)
#define BH1750_CONT_H_RES2  0x11    // Modo continuo alta resolucion (0.5 lux)
#define BH1750_CONT_L_RES   0x13    // Modo continuo baja resolucion (4 lux)

// Numero de muestras para promedio
#define BH1750_MUESTRAS     4

void  BH1750_Init(void);
float BH1750_ReadLux(void);

#endif