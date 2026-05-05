#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>

// Frecuencia del oscilador interno
#define _XTAL_FREQ 8000000

// CONFIG BITS
#pragma config FOSC   = INTOSCIO_EC  // Oscilador interno
#pragma config WDT    = OFF
#pragma config LVP    = OFF
#pragma config PBADEN = OFF
#pragma config MCLRE  = ON

// Pines actuadores
#define CALENTADOR  LATDbits.LATD0   // RD0 — nicromo
#define LED_TIRA    LATDbits.LATD2   // RD2 — tira LED
#define ALARMA      LATDbits.LATD3   // RD3 — buzzer/LED alarma

#endif
