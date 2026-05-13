/**
 * @file    main.c
 * @brief   Programa principal del sistema de monitoreo ambiental.
 *
 * ---------------------------------------------------------
 *  DESCRIPCIÓN DEL SISTEMA
 * ---------------------------------------------------------
 * Microcontrolador : PIC18F4550 a 8 MHz (oscilador interno)
 *
 * Sensores:
 *   LM35   (AN0, ADC) -> Temperatura en °C
 *   MQ-135 (AN2, ADC) -> Calidad del aire (valor crudo ADC)
 *   BME280 (I2C)      -> Humedad relativa en %
 *   BH1750 (I2C)      -> Iluminancia en lux
 *
 * Actuadores (PORTD):
 *   RD0 -> Calentador (resistencia nicromo)
 *   RD2 -> Tira LED de iluminación artificial
 *   RD3 -> Buzzer / LED de alarma
 *   RC2 -> Ventilador (señal PWM mediante CCP1)
 *
 * Visualización:
 *   Pantalla OLED SSD1306 128×64 (I2C) -> muestra T, H, Aire, Luz
 *
 * I2C (bit-banging):
 *   RB0 = SCL  |  RB1 = SDA
 *
 * Ciclo principal: 1 segundo (retardo al final del bucle while).
 * ---------------------------------------------------------
 */

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

/* =========================================================
 *  CONFIGURACIÓN INICIAL DE HARDWARE
 * ========================================================= */

/**
 * @brief  Configura los registros TRIS, LAT y el oscilador del PIC.
 *
 * Detalle de puertos:
 *   OSCCON = 0x72 -> Oscilador interno a 8 MHz (IRCF = 111).
 *   TRISA  = 0xFF -> PORTA como entradas analógicas (para ADC).
 *   TRISB  = 0x00 -> PORTB como salidas (RB0=SCL, RB1=SDA, bit-banging I2C).
 *   TRISD  = 0x00 -> PORTD como salidas (actuadores: calentador, LED, alarma).
 *   TRISC  = 0x00 -> PORTC como salidas (RC2 = PWM del ventilador).
 *   LATD   = 0x00 -> Todos los actuadores apagados al inicio.
 *
 * Nota: ADCON1 se configura dentro de init_ADC(), no aquí.
 */
void setup(void) {
    OSCCON = 0x72;  /* Oscilador interno a 8 MHz */
    TRISA  = 0xFF;  /* PORTA: todo entradas (señales analógicas ADC) */
    TRISB  = 0x00;  /* PORTB: todo salidas (I2C bit-banging en RB0/RB1) */
    TRISD  = 0x00;  /* PORTD: todo salidas (actuadores) */
    TRISC  = 0x00;  /* PORTC: salidas (PWM en RC2) */
    LATD   = 0x00;  /* Apagar todos los actuadores al arrancar */
}

/* =========================================================
 *  PROGRAMA PRINCIPAL
 * ========================================================= */

/**
 * @brief  Función principal del firmware.
 *
 * Secuencia de ejecución:
 *  1. Configurar hardware (puertos, oscilador).
 *  2. Esperar 500 ms para estabilizar la alimentación.
 *  3. Inicializar módulos de comunicación y periféricos.
 *  4. Mostrar mensaje de bienvenida en la pantalla OLED.
 *  5. Entrar en el bucle principal de control (ciclo de ~1 s):
 *       a. Leer los cuatro sensores.
 *       b. Ejecutar la lógica de control de cada actuador.
 *       c. Actualizar la pantalla OLED con los valores actuales.
 *       d. Esperar 1 segundo antes del siguiente ciclo.
 */
void main(void) {

    /* -- Inicialización de hardware -- */
    setup();
    __delay_ms(500);   /* Estabilización de la fuente de alimentación */

    /* -- Inicialización de módulos de comunicación -- */
    I2C_Init();        /* Configurar bus I2C bit-banging (RB0/RB1) */
    init_ADC();        /* Configurar módulo ADC (AN0, AN2) */
    init_PWM();        /* Configurar PWM en RC2 para el ventilador */

    /* -- Inicialización de periféricos y sensores I2C -- */
    OLED_Init();       /* Configurar pantalla OLED SSD1306 */
    OLED_Clear();      /* Borrar cualquier contenido residual */
    BME280_Init();     /* Configurar sensor de humedad BME280 */
    BH1750_Init();     /* Encender sensor de luz BH1750 */

    /* -- Mensaje de arranque en OLED -- */
    OLED_SetCursor(0, 0);
    OLED_WriteString("INICIANDO");
    __delay_ms(1000);
    OLED_Clear();

    /* ================================================================
     *  BUCLE PRINCIPAL DE CONTROL
     *  Se repite cada ~1 segundo. Cada iteración:
     *    1) Lee todos los sensores.
     *    2) Ejecuta la lógica de control de actuadores.
     *    3) Refresca la pantalla OLED.
     * ================================================================ */
    while (1) {

        /* -- 1. Lectura de sensores -- */
        float temp    = leer_temperatura();    /* LM35   -> AN0  -> °C         */
        int   aire    = leer_aire();           /* MQ-135 -> AN2  -> ADC 0-1023 */
        float humedad = BME280_ReadHumidity(); /* BME280 -> I2C  -> %          */
        float lux     = BH1750_ReadLux();      /* BH1750 -> I2C  -> lux        */

        /* -- 2. Lógica de control de actuadores -- */
        control_temperatura(temp);             /* Calentador: histéresis 24-30 °C   */
        control_luz(lux);                      /* Tira LED:   histéresis 450-550 lux */
        control_aire(aire);                    /* Alarma:     ADC > 700 por 4 ciclos */
        control_ventilador(aire, humedad);     /* Ventilador: aire + humedad con rampa */

        /* -- 3. Actualización de la pantalla OLED -- */
        char buffer[14];                       /* Buffer temporal para sprintf */

        /* Línea 0: Temperatura */
        OLED_ClearLine(0);
        OLED_SetCursor(0, 0);
        sprintf(buffer, "T %d C", (int)temp);
        OLED_WriteString(buffer);

        /* Línea 2: Humedad */
        OLED_ClearLine(2);
        OLED_SetCursor(0, 2);
        sprintf(buffer, "H %d%%", (int)humedad);
        OLED_WriteString(buffer);

        /* Línea 4: Calidad de aire */
        OLED_ClearLine(4);
        OLED_SetCursor(0, 4);
        sprintf(buffer, "A %d", aire);
        OLED_WriteString(buffer);

        /* Línea 6: Iluminancia */
        OLED_ClearLine(6);
        OLED_SetCursor(0, 6);
        sprintf(buffer, "L %d", (int)lux);
        OLED_WriteString(buffer);

        /* -- 4. Esperar hasta el próximo ciclo -- */
        __delay_ms(1000);  /* Ciclo de actualización: 1 segundo */
    }
}