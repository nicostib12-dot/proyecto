/**
 * @file    sensores.c
 * @brief   Lectura y conversion de sensores analogicos (LM35 y MQ-135).
 *
 * Utiliza el modulo ADC (adc.h/adc.c) para obtener valores digitales y
 * los convierte a magnitudes fisicas mediante formulas basadas en las
 * especificaciones electricas de cada sensor.
 */

#include <xc.h>
#include "adc.h"
#include "sensores.h"
#include "config.h"

/**
 * @brief  Lee la temperatura desde el sensor LM35 en el canal AN0.
 *
 * El LM35 produce una tension de salida proporcional a la temperatura:
 *   Vout = 10 mV/gC
 *
 * Con un ADC de 10 bits y Vref = 5 V:
 *   Vout (mV) = ADC x (5000 / 1023) aprox ADC x 4.8876 mV
 *
 * Para obtener la temperatura:
 *   Temp (gC) = Vout (mV) / 10 = ADC x 4.8876 / 10
 *
 * @return Temperatura en grados Celsius.
 */
float leer_temperatura(void) {
    int adc = leer_adc(0);           /* Leer canal AN0 */
    return (adc * 4.8876f) / 10.0f; /* Convertir ADC -> gC */
}

/**
 * @brief  Lee la calidad del aire desde el sensor MQ-135 en el canal AN2.
 *
 * El MQ-135 detecta gases como CO2, NH3, benceno y humo. Su salida
 * analogica aumenta proporcionalmente a la concentracion de gases.
 *
 * Se devuelve el valor ADC crudo (0-1023) sin convertir a PPM, ya
 * que la calibracion exacta requeriria conocer la resistencia de carga
 * (RL) y los parametros del sensor en atmosfera limpia (Rs/R0).
 *
 * El umbral de decision en el modulo de control es: ADC > 700 -> alarma.
 *
 * @return Valor ADC del sensor MQ-135 (0 = aire limpio, 1023 = maximo).
 */
int leer_aire(void) {
    return leer_adc(2);  /* Leer canal AN2 */
}