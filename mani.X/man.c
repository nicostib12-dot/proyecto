#include <xc.h>
#include "i12c.h"
#include "ssd1306.h"
#include "lm35.h"
#include "mq135.h"
#include "bme280.h"
#include "bh1750.h"

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
#define MI_R0       10.0

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

void humStr(float num, char *buf) {
    unsigned char i, entero, decimal;
    for(i = 0; i < 10; i++) buf[i] = ' ';
    if(num < 0.0)   num = 0.0;
    if(num > 100.0) num = 100.0;
    entero  = (unsigned char)num;
    decimal = (unsigned char)((num - (float)entero) * 10.0);
    buf[0] = (char)((entero / 10) + '0');
    buf[1] = (char)((entero % 10) + '0');
    buf[2] = '.';
    buf[3] = (char)(decimal + '0');
    buf[4] = '%';
    buf[5] = ' ';
    buf[6] = '\0';
}

void luxStr(float lux, char *buf) {
    unsigned int val;
    unsigned char i;
    for(i = 0; i < 10; i++) buf[i] = ' ';
    if(lux < 0.0)    lux = 0.0;
    if(lux > 9999.0) lux = 9999.0;
    val    = (unsigned int)lux;
    buf[0] = (char)((val / 1000)       + '0');
    buf[1] = (char)(((val % 1000)/100) + '0');
    buf[2] = (char)(((val % 100)/10)   + '0');
    buf[3] = (char)((val % 10)         + '0');
    buf[4] = ' ';
    buf[5] = 'l';
    buf[6] = 'x';
    buf[7] = ' ';
    buf[8] = '\0';
}

void main(void) {
    OSCCON = 0x72;
    while(!OSCCONbits.IOFS);
    __delay_ms(200);
    CMCON = 0x07;

    ADC_Init();
    MQ135_Init();
    unsigned char bme_ok = BME280_Init();
    BH1750_Init();
    I2C_Init();
    OLED_Init();

    // ?? Etiquetas fijas en pantalla ??
    OLED_Print(0, 10, "Hum: ");
    OLED_Print(2, 10, "Aire:");
    OLED_Print(4, 10, "Temp:");
    OLED_Print(6, 10, "Luz: ");

    float        temp, adc_mq, rs, ppm, hum, lux;
    char         bufTemp[10], bufPPM[8];
    char         bufHum[10],  bufLux[10];

    while(1) {
        // ?? Fila 0: Humedad BME280 ??
        if(bme_ok) {
            hum = BME280_ReadHumidity();
            humStr(hum, bufHum);
            OLED_Print(0, 46, "        ");
            OLED_Print(0, 46, bufHum);
        } else {
            OLED_Print(0, 46, "ERROR   ");
        }

        // ?? Fila 2: Calidad aire MQ135 ??
        adc_mq = MQ135_LeerADC();
        rs     = MQ135_calcularRs(adc_mq);
        ppm    = MQ135_calcularPPM(rs, MI_R0);
        ppmStr((unsigned int)ppm, bufPPM);
        OLED_Print(2, 46, "        ");
        OLED_Print(2, 46, bufPPM);
        OLED_Print(2, 82, "pm");

        // ?? Fila 4: Temperatura LM35 ??
        temp = leerTemperatura();
        floatStr(temp, bufTemp, 55);
        OLED_Print(4, 46, "        ");
        OLED_Print(4, 46, bufTemp);
        OLED_Print(4, 82, "C");

        // ?? Fila 6: Luz BH1750 ??
        lux = BH1750_ReadLux();
        luxStr(lux, bufLux);
        OLED_Print(6, 46, "        ");
        OLED_Print(6, 46, bufLux);

        __delay_ms(1000);
    }
}