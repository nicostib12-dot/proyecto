/**
 * @file    oled.h
 * @brief   Interfaz pública del driver para la pantalla OLED SSD1306 (128×64).
 *
 * Controla una pantalla OLED monocromática de 128×64 píxeles con
 * controlador SSD1306 mediante I2C (dirección 0x3C → byte = 0x78).
 *
 * La pantalla se organiza en 8 páginas de 8 píxeles de alto cada una.
 * El driver incluye una fuente 5×7 píxeles con dígitos, letras mayúsculas
 * y algunos caracteres especiales (' ', ':', '%', '.').
 */

#ifndef OLED_H
#define OLED_H

/** @brief  Inicializa el controlador SSD1306 con la secuencia de comandos estándar. */
void OLED_Init(void);

/**
 * @brief  Envía un comando de control al SSD1306.
 * @param  cmd  Byte de comando (ver datasheet SSD1306).
 */
void OLED_Command(unsigned char cmd);

/**
 * @brief  Envía un byte de dato (píxel) al SSD1306.
 * @param  data  Byte de 8 píxeles verticales a escribir en la posición actual.
 */
void OLED_Data(unsigned char data);

/** @brief  Borra toda la pantalla (escribe 0x00 en las 8 páginas × 128 columnas). */
void OLED_Clear(void);

/**
 * @brief  Borra una sola línea (página) de la pantalla de forma rápida.
 * @param  linea  Número de página a borrar (0–7).
 */
void OLED_ClearLine(unsigned char linea);

/**
 * @brief  Posiciona el cursor de escritura en la pantalla.
 *
 * @param  col   Columna en píxeles (0 = izquierda). Internamente se suma un
 *               offset de hardware de 2 columnas.
 * @param  page  Página (fila de 8 px), rango 0–7.
 */
void OLED_SetCursor(unsigned char col, unsigned char page);

/**
 * @brief  Escribe un carácter de la fuente 5×7 en la posición actual.
 *
 * Caracteres soportados: espacio, '0'–'9', 'A'–'Z', ':', '%', '.'.
 * Cualquier otro carácter se muestra como espacio.
 *
 * @param  c  Carácter ASCII a dibujar.
 */
void OLED_WriteChar(char c);

/**
 * @brief  Escribe una cadena de texto en la posición actual del cursor.
 *
 * Recorre la cadena caracter por caracter llamando a OLED_WriteChar().
 *
 * @param  str  Puntero a la cadena de texto terminada en '\0'.
 */
void OLED_WriteString(char *str);

#endif /* OLED_H */
