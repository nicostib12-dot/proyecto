/**
 * @file    sensores.h
 * @brief   Interfaz publica del modulo de sensores analogicos.
 *
 * Envuelve las lecturas ADC de los sensores LM35 (temperatura) y
 * MQ-135 (calidad de aire) con conversiones a unidades fisicas.
 */

#ifndef SENSORES_H
#define SENSORES_H

/**
 * @brief  Lee la temperatura ambiente desde el sensor LM35 (canal AN0).
 *
 * El LM35 genera 10 mV por cada grado Celsius. Con Vref = 5 V y
 * ADC de 10 bits, la formula de conversion es:
 *   Temp (gC) = ADC x (4.8876 / 10)
 *
 * @return Temperatura en grados Celsius (float).
 */
float leer_temperatura(void);

/**
 * @brief  Lee el nivel de calidad del aire desde el sensor MQ-135 (canal AN2).
 *
 * Devuelve el valor ADC crudo (0-1023). Valores mas altos indican
 * mayor concentracion de gases contaminantes.
 * El umbral de "aire malo" esta definido en el modulo de control (> 700).
 *
 * @return Valor ADC entero en el rango [0, 1023].
 */
int leer_aire(void);

#endif /* SENSORES_H */