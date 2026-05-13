/**
 * @file    control.h
 * @brief   Interfaz publica del modulo de logica de control.
 *
 * Contiene las funciones que evaluan las lecturas de los sensores
 * y accionan los actuadores correspondientes (calentador, LED, buzzer,
 * ventilador) mediante logica de histeresis y rampa suave.
 */

#ifndef CONTROL_H
#define CONTROL_H

/**
 * @brief  Controla el calentador segun la temperatura medida.
 *
 * Logica de histeresis:
 *   - Enciende el calentador si T < 24 gC.
 *   - Lo apaga si T > 30 gC.
 *
 * @param  T  Temperatura en gC (leida del LM35).
 */
void control_temperatura(float T);

/**
 * @brief  Controla la tira LED de iluminacion segun el nivel de luz.
 *
 * Logica de histeresis:
 *   - Enciende la tira si lux < 450.
 *   - La apaga si lux > 550.
 *
 * @param  lux  Iluminancia en lux (leida del BH1750).
 */
void control_luz(float lux);

/**
 * @brief  Activa la alarma sonora/visual si la calidad del aire es mala.
 *
 * Si el valor ADC del MQ-135 supera 700 durante mas de 3 ciclos
 * consecutivos, se activa el buzzer/LED de alarma (200 pulsos de 500 us).
 *
 * @param  aire  Valor ADC del MQ-135 (0-1023).
 */
void control_aire(int aire);

/**
 * @brief  Stub de compatibilidad para el control de humedad.
 *
 * La regulacion real del ventilador por humedad se realiza dentro
 * de control_ventilador(). Esta funcion se conserva para no modificar
 * las firmas en main.c ni en este header.
 *
 * @param  H  Humedad relativa en % (no usada internamente).
 */
void control_humedad(float H);

/**
 * @brief  Controla la velocidad del ventilador unificando aire y humedad.
 *
 * Prioridades (de mayor a menor):
 *  1. Aire malo (ADC > 700) -> velocidad maxima (duty = 1023).
 *  2. Humedad baja (H < 40 %) -> velocidad baja (duty = 200).
 *  3. Humedad alta (H > 45 %) -> velocidad alta (duty = 900).
 *  4. Zona normal -> ventilador apagado (duty = 0).
 *
 * Implementa rampa suave: el duty cambia de a 50 unidades por ciclo
 * para evitar picos de corriente al arrancar el motor.
 *
 * @param  aire  Valor ADC del MQ-135 (calidad de aire).
 * @param  H     Humedad relativa en %.
 */
void control_ventilador(int aire, float H);

#endif /* CONTROL_H */