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
#include "bh1750.h"

void setup(void) {
    OSCCON = 0x72;   // 8 MHz oscilador interno
    TRISA  = 0xFF;   // PORTA todo entradas (ADC)
    TRISB  = 0x00;   // PORTB todo salidas (I2C bit-banging RB0/RB1)
    TRISD  = 0x00;   // PORTD todo salidas (actuadores)
    TRISC  = 0x00;   // PORTC salidas (PWM en RC2)
    LATD   = 0x00;   // Apagar todos los actuadores al inicio
    // ADCON1 se configura dentro de init_ADC()
}

void main(void) {
    setup();
    __delay_ms(500);   // Esperar estabilizacion de alimentacion

    // Inicializar modulos
    I2C_Init();
    init_ADC();
    init_PWM();

    // Inicializar sensores I2C
    OLED_Init();
    OLED_Clear();
    BME280_Init();
    BH1750_Init();

    // Mensaje de arranque
    OLED_SetCursor(0, 0);
    OLED_WriteString("INICIANDO");
    __delay_ms(1000);
    OLED_Clear();

    // Bucle principal de control
    while(1) {
        // ── Lectura de sensores ──────────────────────
        float temp    = leer_temperatura();     // LM35  — AN0
        int   aire    = leer_aire();            // MQ135 — AN2
        float humedad = BME280_ReadHumidity();  // BME280 — I2C
        float lux     = BH1750_ReadLux();       // BH1750

        // ── Logica de control ────────────────────────
        control_temperatura(temp);
        control_luz(lux);
        control_aire(aire);
        control_humedad(humedad);

        // ── Mostrar en OLED ──────────────────────────
        char buffer[14];

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
        sprintf(buffer, "L %d", (int)lux);
        OLED_WriteString(buffer);

        __delay_ms(1000);   // Ciclo de 1 segundo
    }
}
