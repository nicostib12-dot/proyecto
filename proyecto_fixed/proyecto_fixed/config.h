#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>

// Frecuencia del oscilador interno
#define _XTAL_FREQ 8000000

// CONFIG BITS
#pragma config FOSC = INTOSCIO_EC  // Oscilador interno, pines como I/O
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config PBADEN = OFF
#pragma config MCLRE = ON   // ? agregar esto

// Pines
#define CALENTADOR LATDbits.LATD0
#define LED        LATDbits.LATD2
#define ALARMA     LATDbits.LATD3

#endif