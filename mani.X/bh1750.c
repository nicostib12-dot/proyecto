
#include "bh1750.h"
#include "i12c.h"

// ?????????????????????????????????????????????????????
// Funcion interna: enviar comando al BH1750
// ?????????????????????????????????????????????????????
static void BH1750_SendCmd(unsigned char cmd) {
    I2C_Start();
    I2C_Write(BH1750_ADDR);   // Direccion + escritura
    I2C_Write(cmd);            // Comando
    I2C_Stop();
    __delay_ms(5);
}

static unsigned int BH1750_LeerBytes(void) {
    unsigned char msb, lsb;
    unsigned int resultado;

    I2C_Start();
    I2C_Write(BH1750_ADDR | 0x01);  // Direccion + lectura

    // Leer MSB con ACK
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    msb = SSPBUF;
    SSPCON2bits.ACKDT = 0;   // ACK ? quiero otro byte
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);

    // Leer LSB con NACK ? ultimo byte
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    lsb = SSPBUF;
    SSPCON2bits.ACKDT = 1;   // NACK ? ultimo byte
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);

    I2C_Stop();

    // Combinar MSB y LSB en valor de 16 bits
    resultado = ((unsigned int)msb << 8) | lsb;
    return resultado;
}

void BH1750_Init(void) {
    __delay_ms(100);                    // Espera arranque del sensor

    BH1750_SendCmd(BH1750_POWER_ON);    // Encender sensor
    BH1750_SendCmd(BH1750_RESET);       // Limpiar datos anteriores
    BH1750_SendCmd(BH1750_CONT_H_RES);  // Modo continuo alta resolucion

    // Espera primera medicion ? modo alta resolucion tarda 180ms
    __delay_ms(200);
}

float BH1750_ReadLux(void) {
    unsigned int suma = 0;
    unsigned char i;
    float lux;

    // Promedio de BH1750_MUESTRAS lecturas
    for(i = 0; i < BH1750_MUESTRAS; i++) {
        suma += BH1750_LeerBytes();
        __delay_ms(200);   // Espera entre lecturas (tiempo conversion)
    }

    // Calcular promedio y convertir a lux
    lux = (float)(suma / BH1750_MUESTRAS) / 1.2;

    // Validar rango fisico del sensor (1 a 65535 lux)
    if(lux < 0.0)     lux = 0.0;
    if(lux > 65535.0) lux = 65535.0;

    return lux;
}