#include <xc.h>
#include "i12c.h"
#include "ssd1306.h"
#include "lm35.h"
#include "mq135.h"

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

// Tu R0 calibrado ? cambia este valor después de calibrar
#define MI_R0   10.0

// Convierte float a string "XX.X"
void floatStr(float num, char *buf, unsigned char maxVal) {
    unsigned char i, entero, decimal;
    for(i = 0; i < 10; i++) buf[i] = ' ';
    if(num < 0.0)           num = 0.0;
    if(num > (float)maxVal) num = (float)maxVal;
    entero  = (unsigned char)num;
    decimal = (unsigned char)((num - (float)entero) * 10.0);
    buf[0] = (char)((entero / 10) + '0');
    buf[1] = (char)((entero % 10) + '0');
    buf[2] = '.';
    buf[3] = (char)(decimal + '0');
    buf[4] = ' ';
    buf[5] = '\0';
}

// Convierte ppm (entero) a string
void ppmStr(unsigned int ppm, char *buf) {
    unsigned char i;
    for(i = 0; i < 8; i++) buf[i] = ' ';
    buf[0] = (char)((ppm / 1000)       + '0');
    buf[1] = (char)(((ppm % 1000)/100) + '0');
    buf[2] = (char)(((ppm % 100)/10)   + '0');
    buf[3] = (char)((ppm % 10)         + '0');
    buf[4] = ' ';
    buf[5] = '\0';
}

void main(void) {
    OSCCON = 0x72;
    while(!OSCCONbits.IOFS);
    __delay_ms(200);

    CMCON = 0x07;

    // Inicializar todo
    ADC_Init();      // LM35 en AN0
    MQ135_Init();    // MQ-135 en AN1 (incluye preheat 3s)
    I2C_Init();
    OLED_Init();

    // Pantalla fija
    OLED_Print(0, 10, "* INVERNADERO *");
    OLED_Print(2, 10, "Temp:");
    OLED_Print(4, 10, "Aire:");
    OLED_Print(6, 10, "Cal:");

    float temp, adc_mq, rs, ppm;
    char  bufTemp[10];
    char  bufPPM[8];
    char *calidad;
    float r0_real = MQ135_calibrarR0();
    char bufR0[10];
    floatStr(r0_real, bufR0, 99);
    OLED_Print(4, 10, "R0:");
    OLED_Print(4, 30, bufR0);
    while(1);  
    while(1) {
        // ?? Leer temperatura (AN0) ??
        temp = leerTemperatura();
        floatStr(temp, bufTemp, 55);
        OLED_Print(2, 46, "       ");
        OLED_Print(2, 46, bufTemp);
        OLED_Print(2, 82, "C");

        // ?? Leer MQ-135 (AN1) ??
        adc_mq = MQ135_LeerADC();
        rs     = MQ135_calcularRs(adc_mq);
        ppm    = MQ135_calcularPPM(rs, MI_R0);

        ppmStr((unsigned int)ppm, bufPPM);
        OLED_Print(4, 46, "       ");
        OLED_Print(4, 46, bufPPM);
        OLED_Print(4, 82, "ppm");

                if(ppm < 450.0)       calidad = "BUENO  ";  // Aire limpio
         else if(ppm < 700.0)  calidad = "NORMAL ";  // Algo de gas
         else if(ppm < 1500.0) calidad = "MALO   ";  // Gas detectado
         else                  calidad = "PELIGRO";        

        OLED_Print(6, 46, "       ");
        OLED_Print(6, 46, calidad);

        __delay_ms(500);
    }
}