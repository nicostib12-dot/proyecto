#include <xc.h>
#include "adc.h"
#include "config.h"

void init_ADC(void) {
    // Habilitar AN0 y AN2 como analógicos, resto digitales
    // PIC18F4550: ADCON1 bits PCFG3:PCFG0 = 0b1101 ? AN0,AN1,AN2 analógicos
    ADCON1 = 0x0D;  // Vref = VDD/VSS, AN0-AN2 analógicos

    // Justificado derecha, Tacq = 12 TAD, Fosc/32 para 8 MHz
    ADCON2 = 0xAA;

    ADCON0 = 0x01;  // ADC ON, canal 0
    __delay_us(50); // tiempo de carga del capacitor de muestreo
}

int leer_adc(int canal) {
    ADCON0bits.CHS = (unsigned char)canal;
    __delay_us(20);     // tiempo de adquisicion
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((int)(ADRESH << 8) | ADRESL);
}
