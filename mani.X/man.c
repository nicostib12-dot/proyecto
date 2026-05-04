#include <xc.h>
#include "i12c.h"
#include "ssd1306.h"

// ============================================================
// CONFIG BITS ? PIC18F4550 con cristal externo 8MHz SIN PLL
// ============================================================
#pragma config FOSC   = INTOSC_HS  // Oscilador interno 8MHz
#pragma config CPUDIV = OSC1_PLL2  // Sin división de CPU
#pragma config PLLDIV = 1          // Sin PLL
#pragma config WDT    = OFF
#pragma config PWRT   = ON
#pragma config BOR    = OFF
#pragma config LVP    = OFF
#pragma config MCLRE  = ON
#pragma config PBADEN = OFF
#pragma config DEBUG  = OFF
#define _XTAL_FREQ 8000000UL     // 8MHz ? debe coincidir con i2c.h

void main(void) {
    // Configurar oscilador interno a 8MHz
    OSCCON = 0x72;       // IRCF=111 ? 8MHz, SCS=10 ? fuente interna
    while(!OSCCONbits.IOFS); // Espera que se estabilice

    ADCON1 = 0x0F;
    CMCON  = 0x07;

    I2C_Init();
    OLED_Init();

    OLED_Print(0, 10, "Hola Mundo!");
    OLED_Print(2, 10, "PIC18F4550");
    OLED_Print(4, 10, "Sin cristal!");

    while(1) { }

}