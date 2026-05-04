#include <xc.h>
#include "adc.h"
#include "sensores.h"
#include "config.h"

float leer_temperatura()
{
    int adc = leer_adc(0);
    return (adc * 500.0) / 1023.0;
}
int leer_luz() {
    return leer_adc(1); // AN1
}
int leer_aire() {
    return leer_adc(2); // AN2
}
