#include <xc.h>
#include "adc.h"
#include "sensores.h"
#include "config.h"

// Temperatura con LM35 — AN0
// LM35: 10mV/C, Vref=5V, ADC 10 bits
// Formula: temp = (adc * 500.0) / 1023.0
float leer_temperatura(void) {
    int adc = leer_adc(0);
    // LM35: 10mV/�C, Vref=5V, ADC 10 bits
    // Vout = adc * (5000mV / 1023)
    // Temp = Vout / 10mV
    return (adc * 4.8876) / 10.0;
}

// Calidad de aire con MQ-135 — AN2
// Retorna valor ADC crudo (0-1023)
// Umbral definido en control: > 300 = aire malo
int leer_aire(void) {
    return leer_adc(2);
}
