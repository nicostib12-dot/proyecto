#include "ssd1306.h"
#include "i12c.h"
#include "font5x8.h"

static void OLED_Cmd(unsigned char cmd) {
    I2C_Start();
    I2C_Write(OLED_ADDR);
    I2C_Write(OLED_CMD);
    I2C_Write(cmd);
    I2C_Stop();
}

static void OLED_Data(unsigned char data) {
    I2C_Start();
    I2C_Write(OLED_ADDR);
    I2C_Write(OLED_DATA);
    I2C_Write(data);
    I2C_Stop();
}

void OLED_Init(void) {
    __delay_ms(200);          // Espera estabilización alimentación

    OLED_Cmd(0xAE);           // Display OFF
    OLED_Cmd(0xD5); OLED_Cmd(0x80); // Frecuencia de reloj
    OLED_Cmd(0xA8); OLED_Cmd(0x3F); // Multiplex 1/64
    OLED_Cmd(0xD3); OLED_Cmd(0x00); // Offset display = 0
    OLED_Cmd(0x40);                  // Línea de inicio = 0
    OLED_Cmd(0x8D); OLED_Cmd(0x14); // Charge pump habilitado
    OLED_Cmd(0x20); OLED_Cmd(0x00); // Modo direccionamiento horizontal
    OLED_Cmd(0xA1);                  // Segment re-map (espejo horizontal)
    OLED_Cmd(0xC8);                  // COM scan invertido (espejo vertical)
    OLED_Cmd(0xDA); OLED_Cmd(0x12); // Configuración pines COM
    OLED_Cmd(0x81); OLED_Cmd(0xFF); // Contraste máximo
    OLED_Cmd(0xD9); OLED_Cmd(0xF1); // Pre-charge period
    OLED_Cmd(0xDB); OLED_Cmd(0x40); // VCOMH deselect level
    OLED_Cmd(0xA4);                  // Sigue RAM (no all-on)
    OLED_Cmd(0xA6);                  // Display normal (no invertido)
    OLED_Cmd(0xAF);                  // Display ON

    OLED_Clear();
}

void OLED_Clear(void) {
    unsigned char page, col;
    for(page = 0; page < OLED_PAGES; page++) {
        OLED_SetCursor(page, 0);
        for(col = 0; col < OLED_WIDTH; col++) {
            OLED_Data(0x00);
        }
    }
}

void OLED_SetCursor(unsigned char page, unsigned char col) {
    OLED_Cmd(0xB0 + page);
    OLED_Cmd(0x00 + (col & 0x0F));   // Nibble bajo columna
    OLED_Cmd(0x10 + (col >> 4));     // Nibble alto columna
}

void OLED_SendChar(char c) {
    unsigned char i;
    if(c < 32 || c > 126) c = 32;   // Carácter inválido ? espacio
    for(i = 0; i < 5; i++) {
        OLED_Data(font5x8[c - 32][i]);
    }
    OLED_Data(0x00);  // Espacio separador entre caracteres
}

void OLED_Print(unsigned char page, unsigned char col, const char *str) {
    OLED_SetCursor(page, col);
    while(*str) {
        OLED_SendChar(*str++);
    }
}