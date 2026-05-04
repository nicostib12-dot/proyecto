#include <xc.h>
#include <stdio.h>
#include "config.h"
#include "adc.h"
#include "sensores.h"
#include "control.h"
#include "pwm.h"
#include "i2c.h"
#include "oled.h"
#include "bme280.h"

void setup(void) {
    OSCCON = 0x72;   // 8 MHz interno
    TRISA  = 0xFF;   // PORTA entradas (sensores ADC)
    TRISB  = 0x00;   // PORTB salidas (I2C bit-banging)
    TRISD  = 0x00;   // PORTD salidas (actuadores)
    LATD   = 0x00;   // Apagar actuadores
    ADCON1 = 0x0E;   // Solo AN0 analogico, resto digital
}

void main(void) {
    setup();
    __delay_ms(500);   // Esperar estabilizacion de alimentacion

    I2C_Init();
    OLED_Init();
    OLED_Clear();
    BME280_Init();
    init_ADC();
    init_PWM();

    while(1) {

        float temp    = leer_temperatura();
        int   luz     = leer_luz();
        int   aire    = leer_aire();
        float humedad = BME280_ReadHumidity();

   
        control_temperatura(temp);
        control_luz(luz);
        control_aire(aire);
        control_humedad(humedad);

        
        char buffer[12];

        OLED_ClearLine(0);
        OLED_SetCursor(0, 0);
        sprintf(buffer, "T %d C", (int)temp);
        OLED_WriteString(buffer);

        OLED_ClearLine(2);
        OLED_SetCursor(0, 2);
        sprintf(buffer, "H %d%%", (int)humedad);
        OLED_WriteString(buffer);

        OLED_ClearLine(4);
        OLED_SetCursor(0, 4);
        sprintf(buffer, "A %d", aire);
        OLED_WriteString(buffer);

        OLED_ClearLine(6);
        OLED_SetCursor(0, 6);
        sprintf(buffer, "L %d", luz);
        OLED_WriteString(buffer);

        __delay_ms(1000);
    }
}
