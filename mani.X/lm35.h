#ifndef LM35_H
#define LM35_H

#include <xc.h>
#define _XTAL_FREQ 8000000UL  // 8MHz oscilador interno

// Numero de muestras para promedio (mas = mas estable, mas lento)
#define NUM_MUESTRAS 8

void        ADC_Init(void);
float       leerTemperatura(void);

#endif