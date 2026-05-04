#include <xc.h>
#include "adc.h"
#include "config.h"

void init_ADC() {

    ADCON0 = 0x01; // ADC ON, canal 0
    ADCON2 = 0xA9; // Justificado derecha, Tacq, Fosc/8
}

int leer_adc(int canal) {

    ADCON0bits.CHS = (unsigned char)canal; // seleccionar canal

    __delay_us(10); // tiempo de adquisición

    GO_nDONE = 1;
    while(GO_nDONE);

    return (ADRESH << 8) + ADRESL;
}
