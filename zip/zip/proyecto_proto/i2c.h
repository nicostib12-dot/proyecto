/**
 * @file    i2c.h
 * @brief   Interfaz publica del modulo I2C por software (bit-banging).
 *
 * Implementa el protocolo I2C usando dos pines GPIO del PORTB:
 *   - RB0: linea SCL (reloj)
 *   - RB1: linea SDA (datos)
 *
 * Todos los dispositivos I2C del sistema (OLED, BME280, BH1750)
 * comparten este bus.
 */

#ifndef I2C_H
#define I2C_H

/** @brief  Configura los pines SCL y SDA como salidas y los pone en alto. */
void I2C_Init(void);

/** @brief  Genera la condicion de START del protocolo I2C. */
void I2C_Start(void);

/** @brief  Genera la condicion de STOP del protocolo I2C. */
void I2C_Stop(void);

/**
 * @brief  Envia un byte por el bus I2C (MSB primero).
 * @param  data  Byte a transmitir.
 */
void I2C_Write(unsigned char data);

/**
 * @brief  Recibe un byte del bus I2C.
 * @param  ack   1 -> enviar ACK al esclavo (hay mas bytes por leer).
 *               0 -> enviar NACK al esclavo (ultimo byte de la trama).
 * @return Byte recibido.
 */
unsigned char I2C_Read(char ack);

/**
 * @brief  Genera una condicion de Repeated START (declarada para
 *         compatibilidad futura, no implementada en i2c.c).
 */
void I2C_RepeatedStart(void);

#endif /* I2C_H */