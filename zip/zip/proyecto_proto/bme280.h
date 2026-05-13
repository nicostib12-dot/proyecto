/**
 * @file    bme280.h
 * @brief   Interfaz publica del driver para el sensor BME280.
 *
 * El BME280 es un sensor ambiental de Bosch que mide temperatura,
 * presion y humedad relativa por I2C. En este proyecto solo se
 * utiliza la medicion de humedad relativa.
 *
 * Direccion I2C: 0x76 (pin SDO conectado a GND).
 */

#ifndef BME280_H
#define BME280_H

/**
 * @brief  Inicializa el BME280 con configuracion basica de humedad.
 *
 * Configura el oversampling de humedad x1 y el modo de espera.
 * Requiere al menos 100 ms de retardo tras el encendido.
 */
void BME280_Init(void);

/**
 * @brief  Lee la humedad relativa del BME280.
 *
 * Dispara una medicion en modo forzado y devuelve el porcentaje
 * de humedad relativa sin aplicar calibracion de fabrica.
 *
 * @note   Para uso academico. En produccion se recomienda aplicar
 *         los coeficientes de calibracion del sensor (registros 0xE1-0xE7).
 *
 * @return Humedad relativa en % (float, rango aproximado 0-100 %).
 */
float BME280_ReadHumidity(void);

#endif /* BME280_H */