/**
 * @file    bh1750.h
 * @brief   Interfaz publica del driver para el sensor de luz BH1750.
 *
 * El BH1750 es un sensor digital de iluminancia (lux) con comunicacion I2C.
 * Direccion I2C: 0x23 (pin ADDR conectado a GND).
 */

#ifndef BH1750_H
#define BH1750_H

/**
 * @brief  Enciende el sensor BH1750 y lo deja listo para medir.
 *
 * Envia el comando "Power On" (0x01) al sensor.
 */
void BH1750_Init(void);

/**
 * @brief  Realiza una medicion y devuelve la iluminancia en lux.
 *
 * Envia el comando de medicion continua en alta resolucion,
 * espera la conversion y devuelve el resultado escalado a lux.
 *
 * @return Iluminancia en lux (float). Rango: 1 - 65535 lux.
 */
float BH1750_ReadLux(void);

#endif /* BH1750_H */