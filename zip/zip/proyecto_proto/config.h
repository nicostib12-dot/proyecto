/**
 * @file    config.h
 * @brief   Configuracion global del proyecto.
 *
 * Define la frecuencia del oscilador, los bits de configuracion del
 * microcontrolador PIC18F4550 y los alias de los pines de salida
 * que controlan los actuadores fisicos del sistema.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>

/* =========================================================
 *  FRECUENCIA DEL OSCILADOR
 *  Requerida por las macros __delay_ms() y __delay_us()
 *  del compilador XC8.
 * ========================================================= */
#define _XTAL_FREQ 8000000   /**< Oscilador interno a 8 MHz */

/* =========================================================
 *  BITS DE CONFIGURACION DEL PIC18F4550
 *  Se graban en la memoria de programa durante la programacion
 *  y no pueden cambiarse en tiempo de ejecucion.
 * ========================================================= */
#pragma config FOSC   = INTOSCIO_EC  /**< Usa el oscilador interno */
#pragma config WDT    = OFF          /**< Watchdog Timer desactivado */
#pragma config LVP    = OFF          /**< Programacion de bajo voltaje desactivada */
#pragma config PBADEN = OFF          /**< PORTB pines digitales por defecto */
#pragma config MCLRE  = ON           /**< Pin MCLR habilitado como reset */

/* =========================================================
 *  ALIAS DE PINES - ACTUADORES (PORTD)
 *
 *  Se usan macros para dar nombres descriptivos a los pines
 *  de salida, lo que hace el codigo de control mas legible.
 * ========================================================= */
#define CALENTADOR  LATDbits.LATD0   /**< RD0 -> Resistencia de nicromo (calefactor) */
#define LED_TIRA    LATDbits.LATD2   /**< RD2 -> Tira LED de iluminacion artificial */
#define ALARMA      LATDbits.LATD3   /**< RD3 -> Buzzer / LED de alarma */

#endif /* CONFIG_H */