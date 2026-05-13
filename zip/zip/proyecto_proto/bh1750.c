/**
 * @file    bh1750.c
 * @brief   Driver para el sensor de iluminancia BH1750 (comunicacion I2C).
 *
 * El BH1750 mide la cantidad de luz visible y devuelve un valor de 16 bits
 * que se convierte a lux dividiendo entre 1.2 (factor del datasheet).
 *
 * Direccion I2C utilizada: 0x23 (pin ADDR = GND).
 * Modo de medicion: continua, alta resolucion (0x10), resolucion ~1 lux.
 */

#include <xc.h>
#include "i2c.h"
#include "bh1750.h"
#include "config.h"

/* =========================================================
 *  CONSTANTES DEL SENSOR
 * ========================================================= */
/** Direccion I2C del BH1750 con ADDR = GND (alternativa: 0x5C si ADDR = VCC) */
#define BH1750_ADDR         0x23

/** Comando: medicion continua en alta resolucion (~1 lux, tiempo ~120 ms) */
#define BH1750_CONT_HRES    0x10

/**
 * @brief  Inicializa el sensor BH1750.
 *
 * Envia el comando "Power On" (0x01) para sacar el sensor
 * del estado de reposo y dejarlo listo para recibir comandos.
 */
void BH1750_Init(void) {
    I2C_Start();
    I2C_Write(BH1750_ADDR << 1); /* Direccion + bit de escritura (0) */
    I2C_Write(0x01);              /* Comando Power On */
    I2C_Stop();
    __delay_ms(10);               /* Esperar estabilizacion */
}

/**
 * @brief  Lee la iluminancia actual del sensor BH1750 en lux.
 *
 * Proceso de lectura:
 *  1. Enviar el comando de medicion continua alta resolucion (0x10)
 *     para disparar una nueva conversion.
 *  2. Esperar 180 ms (tiempo de conversion del sensor en alta resolucion).
 *  3. Leer 2 bytes (MSB y LSB) del resultado en modo lectura.
 *  4. Combinar los bytes en un valor de 16 bits.
 *  5. Dividir entre 1.2 para obtener lux (conversion del datasheet).
 *
 * @return Iluminancia en lux (float).
 */
float BH1750_ReadLux(void) {
    unsigned char msb, lsb;
    unsigned int raw;

    /* --- Paso 1: Enviar comando de medicion --- */
    I2C_Start();
    I2C_Write(BH1750_ADDR << 1); /* Escritura */
    I2C_Write(BH1750_CONT_HRES); /* Comando: medir en alta resolucion */
    I2C_Stop();

    /* --- Paso 2: Esperar conversion (~120 ms tipico, 180 ms seguro) --- */
    __delay_ms(180);

    /* --- Paso 3: Leer 2 bytes del resultado --- */
    I2C_Start();
    I2C_Write((BH1750_ADDR << 1) | 1); /* Direccion + bit de lectura (1) */
    msb = I2C_Read(1);  /* Byte alto, enviar ACK (hay mas datos) */
    lsb = I2C_Read(0);  /* Byte bajo, enviar NACK (fin de lectura) */
    I2C_Stop();

    /* --- Paso 4: Combinar bytes en valor de 16 bits --- */
    raw = ((unsigned int)msb << 8) | lsb;

    /* --- Paso 5: Convertir a lux segun el datasheet del BH1750 --- */
    /* Lux = raw / 1.2 */
    return (float)raw / 1.2f;
}