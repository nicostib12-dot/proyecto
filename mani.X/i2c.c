#include "i12c.h"

void I2C_Init(void) {
    TRISB1 = 1;          // SCL
    TRISB0 = 1;          // SDA
    SSPCON1 = 0x28;      // Habilita I2C maestro
    SSPCON2 = 0x00;
    SSPADD  = 19;        // 100kHz @ 8MHz: (8000000/(4*100000))-1 = 19
    SSPSTAT = 0x80;      // Slew rate OFF para 100kHz
    PIR1bits.SSPIF = 0;
}

void I2C_Start(void) {
    SSPCON2bits.SEN = 1;
    while(SSPCON2bits.SEN);
}

void I2C_Stop(void) {
    SSPCON2bits.PEN = 1;
    while(SSPCON2bits.PEN);
}

void I2C_RepeatedStart(void) {
    SSPCON2bits.RSEN = 1;
    while(SSPCON2bits.RSEN);
}

unsigned char I2C_Write(unsigned char data) {
    PIR1bits.SSPIF = 0;
    SSPBUF = data;
    while(!PIR1bits.SSPIF);  // Espera transmisión completa
    PIR1bits.SSPIF = 0;
    return SSPCON2bits.ACKSTAT; // 0 = ACK recibido (OK)
}