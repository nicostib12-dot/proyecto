#ifndef SSD1306_H
#define SSD1306_H

#include <xc.h>

#define OLED_ADDR   0x78    // Dirección SSD1306: 0x3C << 1 = 0x78
#define OLED_CMD    0x00
#define OLED_DATA   0x40
#define OLED_WIDTH  128
#define OLED_PAGES  8

void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(unsigned char page, unsigned char col);
void OLED_SendChar(char c);
void OLED_Print(unsigned char page, unsigned char col, const char *str);

#endif