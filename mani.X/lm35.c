
#include "lm35.h"

void ADC_Init(void) {

    UCON   = 0x00;         // Apaga modulo USB completo
    UCFG   = 0x08;         // Deshabilita transceiver USB

    // 1b. Configurar RA0 como entrada (pin del LM35)
    TRISAbits.TRISA0 = 1;  // 1 = entrada, 0 = salida

    ADCON1 = 0x0E;
    ADCON2 = 0xBE;

    ADCON0 = 0x01;

    // 1f. Esperar estabilizacion del ADC antes de primera lectura
    __delay_ms(20);
}

static unsigned int leer_adc_raw(void) {
    // Tiempo de adquisicion: capacitor interno del ADC se carga
    __delay_us(25);

    // Iniciar conversion
    ADCON0bits.GO_nDONE = 1;

    while(ADCON0bits.GO_nDONE);

    return (unsigned int)(((unsigned int)ADRESH << 8) | ADRESL);
}

float leerTemperatura(void) {
    unsigned long suma = 0;
    unsigned char i;
    unsigned int  promedio;
    float voltaje_mV;
    float temperatura;

    for(i = 0; i < NUM_MUESTRAS; i++) {
        suma += leer_adc_raw();
        __delay_ms(2);  // Pequena pausa entre lecturas
    }

    // Calcular promedio
    promedio = (unsigned int)(suma / NUM_MUESTRAS);

   
    voltaje_mV = (float)promedio * 4.8828;

    
    temperatura = voltaje_mV / 10.0;

    return temperatura;
}