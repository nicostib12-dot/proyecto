/**
 * @file    pwm.c
 * @brief   Modulo PWM usando CCP1 y Timer2 del PIC18F4550.
 *
 * Genera una senal PWM de 10 bits de resolucion en el pin RC2.
 * El PWM se usa para controlar la velocidad del ventilador mediante
 * un circuito de potencia externo (transistor o MOSFET).
 *
 * Frecuencia PWM aprox. 7.8 kHz con Fosc = 8 MHz y PR2 = 255.
 */

#include <xc.h>
#include "config.h"
#include "pwm.h"

/**
 * @brief  Inicializa el modulo PWM en RC2 usando CCP1 y Timer2.
 *
 * Pasos de configuracion (segun datasheet PIC18F4550, seccion 15):
 *  1. Poner RC2 como salida (TRISC2 = 0).
 *  2. Cargar PR2 con el valor del periodo deseado.
 *     Frecuencia = Fosc / (4 x (PR2+1) x prescaler)
 *               = 8 MHz / (4 x 256 x 1) aprox. 7812 Hz
 *  3. Configurar CCP1CON en modo PWM (bits CCP1M3:CCP1M0 = 1100).
 *  4. Activar Timer2 con prescaler 1:1 (T2CON = 0b00000100).
 */
void init_PWM(void) {
    TRISCbits.TRISC2 = 0;    /* RC2 como salida (pin CCP1) */
    PR2       = 255;          /* Periodo del PWM: (255+1) ciclos de Timer2 */
    CCP1CON   = 0b00001100;   /* Modo PWM activo en CCP1 */
    T2CON     = 0b00000100;   /* Timer2 ON, prescaler 1:1, postscaler 1:1 */
}

/**
 * @brief  Establece el ciclo de trabajo del PWM (10 bits).
 *
 * El PIC18F4550 usa 10 bits para el duty cycle distribuidos en:
 *   - Bits [9:2] -> CCPR1L (8 bits mas significativos).
 *   - Bits [1:0] -> DC1B en CCP1CON (2 bits menos significativos).
 *
 * Ejemplo:
 *   duty = 512  (50%)  -> CCPR1L = 128, DC1B = 00
 *   duty = 1023 (100%) -> CCPR1L = 255, DC1B = 11
 *
 * @param  duty  Valor entre 0 (apagado) y 1023 (maxima potencia).
 */
void set_PWM_duty(int duty) {
    /* Limitar el valor al rango valido de 10 bits */
    if (duty > 1023) duty = 1023;

    /* Escribir los 8 bits altos en CCPR1L */
    CCPR1L = (unsigned char)(duty >> 2);

    /* Escribir los 2 bits bajos en los campos DC1B del registro CCP1CON */
    CCP1CONbits.DC1B = duty & 0x03;
}