/**
 * @file    i2c.c
 * @brief   Implementacion I2C por software (bit-banging) para el PIC18F4550.
 *
 * El protocolo I2C se implementa manualmente controlando los pines
 * RB0 (SCL) y RB1 (SDA) con retardos de 5 us entre cada transicion,
 * lo que da una velocidad aproximada de ~50 kHz (modo estandar lento).
 *
 * Las resistencias pull-up internas del PORTB se habilitan mediante
 * INTCON2bits.RBPU = 0.
 */

#include <xc.h>
#include "config.h"
#include "i2c.h"

/* =========================================================
 *  DEFINICION DE PINES
 * ========================================================= */
#define SCL_DIR  TRISBbits.TRISB0  /**< Direccion del pin SCL (0=salida, 1=entrada) */
#define SDA_DIR  TRISBbits.TRISB1  /**< Direccion del pin SDA (0=salida, 1=entrada) */
#define SCL      LATBbits.LATB0    /**< Escritura en el pin SCL */
#define SDA      LATBbits.LATB1    /**< Escritura en el pin SDA */
#define SDA_IN   PORTBbits.RB1     /**< Lectura del pin SDA (para recibir datos) */

/**
 * @brief  Inicializa el bus I2C.
 *
 * Configura SCL y SDA como salidas y los pone en estado alto (bus libre).
 * Activa las resistencias pull-up internas del PORTB.
 */
void I2C_Init(void) {
    SCL_DIR = 0;           /* SCL como salida */
    SDA_DIR = 0;           /* SDA como salida */
    SCL = 1;               /* Bus libre: SCL en alto */
    SDA = 1;               /* Bus libre: SDA en alto */
    INTCON2bits.RBPU = 0;  /* Habilitar pull-ups internos del PORTB */
}

/**
 * @brief  Genera la condicion de START del bus I2C.
 *
 * Secuencia: SDA baja mientras SCL esta en alto.
 *
 *   SDA: -----+
 *             +-----
 *   SCL: ----------
 */
void I2C_Start(void) {
    SDA_DIR = 0;            /* SDA como salida */
    SDA = 1; __delay_us(5);
    SCL = 1; __delay_us(5);
    SDA = 0; __delay_us(5); /* Flanco de bajada de SDA -> condicion START */
    SCL = 0; __delay_us(5);
}

/**
 * @brief  Genera la condicion de STOP del bus I2C.
 *
 * Secuencia: SDA sube mientras SCL esta en alto.
 *
 *   SDA:      +-----
 *   ----------+
 *   SCL: ----------
 */
void I2C_Stop(void) {
    SDA_DIR = 0;            /* SDA como salida */
    SDA = 0; __delay_us(5);
    SCL = 1; __delay_us(5);
    SDA = 1; __delay_us(5); /* Flanco de subida de SDA -> condicion STOP */
}

/**
 * @brief  Transmite un byte por el bus I2C (bit mas significativo primero).
 *
 * Por cada uno de los 8 bits:
 *  1. Coloca el bit en SDA.
 *  2. Sube SCL (el esclavo lee en el flanco de subida).
 *  3. Baja SCL.
 *
 * Al final, libera SDA (entrada) y genera un pulso de SCL para
 * recibir el bit de ACK/NACK del esclavo (se ignora en esta implementacion).
 *
 * @param  data  Byte a enviar.
 */
void I2C_Write(unsigned char data) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        /* Colocar el bit MSB en SDA */
        SDA = (data & 0x80) ? 1 : 0;
        data <<= 1;             /* Desplazar al siguiente bit */
        __delay_us(5);
        SCL = 1; __delay_us(5); /* El esclavo muestrea SDA en este momento */
        SCL = 0; __delay_us(5);
    }
    /* Pulso de reloj para el ACK del esclavo (se descarta) */
    SDA_DIR = 1; __delay_us(5); /* SDA como entrada para leer ACK */
    SCL = 1;     __delay_us(5);
    SCL = 0;
    SDA_DIR = 0; __delay_us(5); /* Volver SDA a salida */
}

/**
 * @brief  Recibe un byte del bus I2C (bit mas significativo primero).
 *
 * Por cada uno de los 8 bits:
 *  1. Sube SCL (el esclavo coloca el bit en SDA).
 *  2. Lee SDA.
 *  3. Baja SCL.
 *
 * Al final envia ACK (SDA=0) o NACK (SDA=1) segun el parametro.
 *
 * @param  ack  1 -> ACK  (el maestro quiere recibir mas bytes).
 *              0 -> NACK (ultimo byte, finalizar la lectura).
 * @return Byte recibido del esclavo.
 */
unsigned char I2C_Read(char ack) {
    unsigned char data = 0;
    unsigned char i;

    SDA_DIR = 1; /* SDA como entrada para recibir bits */

    for (i = 0; i < 8; i++) {
        data <<= 1;                 /* Hacer espacio para el nuevo bit */
        __delay_us(5);
        SCL = 1; __delay_us(5);     /* El esclavo coloca el bit en SDA */
        if (SDA_IN) data |= 0x01;   /* Leer el bit recibido */
        SCL = 0; __delay_us(5);
    }

    /* Enviar respuesta al esclavo */
    SDA_DIR = 0;
    SDA = (ack) ? 0 : 1;  /* ACK = SDA en bajo;  NACK = SDA en alto */
    __delay_us(5);
    SCL = 1; __delay_us(5);
    SCL = 0;
    SDA = 1; __delay_us(5);

    return data;
}