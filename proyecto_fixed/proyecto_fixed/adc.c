#include <xc.h>
#include "adc.h"
#include "config.h"

void init_ADC() {
    // AN0 (LM35), AN1 (no usado fisicamente), AN2 (MQ-135) como analogicos
    // ADCON1: AN0, AN1, AN2 analogicos â€” resto digitales
    ADCON1 = 0x09;  // 0b00001001  AN0, AN1, AN2 analogicos, Vref=VDD/VSS
    
    // ADCON2: justificado derecha, Tacq=4TAD, Fosc/32 (correcto para 8MHz)
    ADCON2 = 0xAA;  // ADFM=1, ACQT=010, ADCS=010
    ADCON0 = 0x01;  // ADC ON, canal 0 inicial
}

int leer_adc(int canal) {

    ADCON0bits.CHS = (unsigned char)canal; // seleccionar canal
    __delay_us(20); // tiempo de adquisición
    GO_nDONE = 1;
    while(GO_nDONE);
    return (ADRESH << 8) + ADRESL;
}
