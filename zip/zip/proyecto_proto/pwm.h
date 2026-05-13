/**
 * @file    pwm.h
 * @brief   Interfaz publica del modulo PWM para el ventilador.
 *
 * Usa el modulo CCP1 del PIC18F4550 junto con el Timer2 para
 * generar una senal PWM en el pin RC2 con resolucion de 10 bits.
 */

#ifndef PWM_H
#define PWM_H

/**
 * @brief  Inicializa el modulo PWM en el pin RC2 (CCP1).
 *
 * Configura Timer2 y CCP1 para generar PWM con:
 *   - Periodo definido por PR2 = 255.
 *   - Prescaler del Timer2 = 1:1.
 *   - Frecuencia PWM aprox. Fosc / (4 x (PR2+1) x prescaler) aprox. 7.8 kHz a 8 MHz.
 */
void init_PWM(void);

/**
 * @brief  Ajusta el ciclo de trabajo (duty cycle) del PWM.
 *
 * El valor se limita automaticamente al rango [0, 1023].
 *   - duty = 0    -> ventilador apagado (senal siempre en bajo).
 *   - duty = 1023 -> velocidad maxima  (senal siempre en alto).
 *
 * @param  duty  Ciclo de trabajo de 10 bits (0-1023).
 */
void set_PWM_duty(int duty);

#endif /* PWM_H */