/**
 * @file    adc.c
 * @brief   Implementacion del modulo ADC para el PIC18F4550.
 *
 * Utiliza el ADC interno de 10 bits del PIC18F4550 para leer senales
 * analogicas de los sensores conectados a los canales AN0 y AN2:
 *   - AN0: sensor de temperatura LM35
 *   - AN2: sensor de calidad de aire MQ-135
 */

#include <xc.h>
#include "adc.h"
#include "config.h"

/**
 * @brief  Inicializa el modulo ADC del PIC18F4550.
 *
 * Configura tres registros de control:
 *
 *  ADCON1 = 0x0D
 *    -> Bits PCFG3:PCFG0 = 0b1101
 *    -> AN0, AN1 y AN2 como analogicos; el resto como digitales.
 *    -> Referencias: VDD (positiva) y VSS (negativa).
 *
 *  ADCON2 = 0xAA
 *    -> Justificacion a la derecha (bit 7 = 1).
 *    -> Tiempo de adquisicion: 12 TAD (bits 5:3 = 101).
 *    -> Reloj de conversion: Fosc/32 (bits 2:0 = 010), adecuado para 8 MHz.
 *
 *  ADCON0 = 0x01
 *    -> Canal seleccionado: AN0 (bits CHS = 00000).
 *    -> Modulo ADC encendido (bit ADON = 1).
 *
 *  Al final se espera 50 us para que el capacitor de muestreo
 *  interno se cargue completamente antes de la primera lectura.
 */
void init_ADC(void) {
    /* Seleccion de pines analogicos y referencias de tension */
    ADCON1 = 0x0D;

    /* Tiempo de adquisicion y reloj de conversion */
    ADCON2 = 0xAA;

    /* Activar ADC en canal 0 */
    ADCON0 = 0x01;

    /* Esperar carga del capacitor de muestreo */
    __delay_us(50);
}

/**
 * @brief  Lee el valor digital de un canal analogico especifico.
 *
 * Pasos:
 *  1. Seleccionar el canal mediante los bits CHS de ADCON0.
 *  2. Esperar 20 us para la adquisicion (carga del capacitor de muestreo).
 *  3. Disparar la conversion poniendo GO_nDONE = 1.
 *  4. Esperar en bucle hasta que el hardware ponga GO_nDONE = 0 (fin).
 *  5. Leer el resultado: parte alta en ADRESH y parte baja en ADRESL.
 *
 * @param  canal  Canal analogico a leer (ej.: 0 = AN0, 2 = AN2).
 * @return Valor entero de 10 bits en el rango [0, 1023].
 */
int leer_adc(int canal) {
    /* Seleccionar el canal de entrada */
    ADCON0bits.CHS = (unsigned char)canal;

    /* Tiempo de adquisicion minimo antes de convertir */
    __delay_us(20);

    /* Iniciar conversion */
    GO_nDONE = 1;

    /* Esperar hasta que la conversion termine (hardware lo pone a 0) */
    while (GO_nDONE);

    /* Combinar los 8 bits altos y los 2 bits bajos del resultado */
    return ((int)(ADRESH << 8) | ADRESL);
}