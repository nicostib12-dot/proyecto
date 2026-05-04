#ifndef OLED_H
#define OLED_H

void OLED_Init(void);
void OLED_Command(unsigned char cmd);
void OLED_Data(unsigned char data);
void OLED_Clear(void);
void OLED_SetCursor(unsigned char x, unsigned char y);
void OLED_WriteChar(char c);
void OLED_WriteString(char *str);
void OLED_ClearLine(unsigned char linea);

#endif