#include <xc.h>
#include "i12c.h"
#include "ssd1306.h"
#include "lm35.h"          // ? AGREGAR

#pragma config FOSC   = INTOSC_HS
#pragma config CPUDIV = OSC1_PLL2
#pragma config PLLDIV = 1
#pragma config WDT    = OFF
#pragma config PWRT   = ON
#pragma config BOR    = OFF
#pragma config LVP    = OFF
#pragma config MCLRE  = ON
#pragma config PBADEN = OFF
#pragma config DEBUG  = OFF

#define _XTAL_FREQ 8000000UL

// Convierte temperatura float a string "XX.X C"
void tempToStr(float num, char *buf) {
    unsigned char i, entero, decimal;
    for(i = 0; i < 10; i++) buf[i] = ' ';
    if(num < 0.0)  num = 0.0;
    if(num > 55.0) num = 55.0;
    entero  = (unsigned char)num;
    decimal = (unsigned char)((num - (float)entero) * 10.0);
    buf[0] = (char)((entero / 10) + '0');
    buf[1] = (char)((entero % 10) + '0');
    buf[2] = '.';
    buf[3] = (char)(decimal + '0');
    buf[4] = ' ';
    buf[5] = 'C';
    buf[6] = ' ';
    buf[7] = '\0';
}

void main(void) {
    OSCCON = 0x72;
    while(!OSCCONbits.IOFS);
    CMCON = 0x07;

    ADC_Init();            // ? AGREGAR antes que I2C
    I2C_Init();
    OLED_Init();

    // Pantalla fija ? titulos
    OLED_Print(0, 15, "* INVERNADERO *");
    OLED_Print(2, 10, "Temp:");
    OLED_Print(6, 10, "Estado:");

    float temp;
    char  buf[10];
    char *estado;

    while(1) {
        // Leer temperatura                  ? AGREGAR
        temp = leerTemperatura();
        tempToStr(temp, buf);

        // Mostrar en OLED
        OLED_Print(2, 46, "        ");
        OLED_Print(2, 46, buf);

        // Estado segun rango
        if(temp < 15.0)      estado = "  FRIO  ";
        else if(temp > 30.0) estado = "  CALOR ";
        else                 estado = " NORMAL ";

        OLED_Print(6, 52, "        ");
        OLED_Print(6, 52, estado);

        __delay_ms(500);
    }
}