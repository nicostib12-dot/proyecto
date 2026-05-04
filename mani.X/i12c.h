#ifndef I2C_H
#define I2C_H

#include <xc.h>
#define _XTAL_FREQ 8000000UL   // ? 8MHz cristal externo

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_RepeatedStart(void);
unsigned char I2C_Write(unsigned char data);

#endif