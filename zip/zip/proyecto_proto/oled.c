/**
 * @file    oled.c
 * @brief   Driver para pantalla OLED SSD1306 de 128×64 píxeles vía I2C.
 *
 * Implementa:
 *  - Fuente de mapa de bits 5×7 para dígitos y letras mayúsculas.
 *  - Funciones de inicialización, borrado y escritura de texto.
 *  - Acceso directo al bus I2C para minimizar overhead en transferencias.
 *
 * Cada byte de datos representa 8 píxeles verticales (columna de 8 px).
 * Una página = 8 filas de píxeles. La pantalla tiene 8 páginas × 128 columnas.
 *
 * Dirección I2C del SSD1306: 0x3C (7 bits) -> 0x78 como byte de dirección I2C.
 * Offset de hardware: las columnas visibles empiezan en la columna 2 del chip.
 */

#include <xc.h>
#include "i2c.h"
#include "oled.h"
#include "config.h"

/** Dirección I2C del SSD1306 ya desplazada (0x3C << 1 = 0x78) */
#define OLED_ADDR 0x78

/* =========================================================
 *  FUENTE DE MAPA DE BITS 5×7
 *
 *  Cada entrada es un arreglo de 5 bytes. Cada byte representa
 *  una columna de 8 píxeles (bit 0 = fila superior).
 *
 *  Índices:
 *    0      -> ' ' (espacio)
 *    1?10   -> '0'?'9'
 *    11?36  -> 'A'?'Z'
 *    37     -> ':'
 *    38     -> '%'
 *    39     -> '.'
 * ========================================================= */
const unsigned char font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* [0]  ' ' espacio           */
    {0x3E,0x51,0x49,0x45,0x3E}, /* [1]  '0'                   */
    {0x00,0x42,0x7F,0x40,0x00}, /* [2]  '1'                   */
    {0x42,0x61,0x51,0x49,0x46}, /* [3]  '2'                   */
    {0x21,0x41,0x45,0x4B,0x31}, /* [4]  '3'                   */
    {0x18,0x14,0x12,0x7F,0x10}, /* [5]  '4'                   */
    {0x27,0x45,0x45,0x45,0x39}, /* [6]  '5'                   */
    {0x3C,0x4A,0x49,0x49,0x30}, /* [7]  '6'                   */
    {0x01,0x71,0x09,0x05,0x03}, /* [8]  '7'                   */
    {0x36,0x49,0x49,0x49,0x36}, /* [9]  '8'                   */
    {0x06,0x49,0x49,0x29,0x1E}, /* [10] '9'                   */
    {0x7E,0x11,0x11,0x11,0x7E}, /* [11] 'A'                   */
    {0x7F,0x49,0x49,0x49,0x36}, /* [12] 'B'                   */
    {0x3E,0x41,0x41,0x41,0x22}, /* [13] 'C'                   */
    {0x7F,0x41,0x41,0x22,0x1C}, /* [14] 'D'                   */
    {0x7F,0x49,0x49,0x49,0x41}, /* [15] 'E'                   */
    {0x7F,0x09,0x09,0x09,0x01}, /* [16] 'F'                   */
    {0x3E,0x41,0x49,0x49,0x7A}, /* [17] 'G'                   */
    {0x7F,0x08,0x08,0x08,0x7F}, /* [18] 'H'                   */
    {0x00,0x41,0x7F,0x41,0x00}, /* [19] 'I'                   */
    {0x20,0x40,0x41,0x3F,0x01}, /* [20] 'J'                   */
    {0x7F,0x08,0x14,0x22,0x41}, /* [21] 'K'                   */
    {0x7F,0x40,0x40,0x40,0x40}, /* [22] 'L'                   */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* [23] 'M'                   */
    {0x7F,0x04,0x08,0x10,0x7F}, /* [24] 'N'                   */
    {0x3E,0x41,0x41,0x41,0x3E}, /* [25] 'O'                   */
    {0x7F,0x09,0x09,0x09,0x06}, /* [26] 'P'                   */
    {0x3E,0x41,0x51,0x21,0x5E}, /* [27] 'Q'                   */
    {0x7F,0x09,0x19,0x29,0x46}, /* [28] 'R'                   */
    {0x46,0x49,0x49,0x49,0x31}, /* [29] 'S'                   */
    {0x01,0x01,0x7F,0x01,0x01}, /* [30] 'T'                   */
    {0x3F,0x40,0x40,0x40,0x3F}, /* [31] 'U'                   */
    {0x1F,0x20,0x40,0x20,0x1F}, /* [32] 'V'                   */
    {0x3F,0x40,0x38,0x40,0x3F}, /* [33] 'W'                   */
    {0x63,0x14,0x08,0x14,0x63}, /* [34] 'X'                   */
    {0x07,0x08,0x70,0x08,0x07}, /* [35] 'Y'                   */
    {0x61,0x51,0x49,0x45,0x43}, /* [36] 'Z'                   */
    {0x00,0x00,0x5F,0x00,0x00}, /* [37] ':'                   */
    {0x00,0x06,0x09,0x09,0x06}, /* [38] '%'                   */
    {0x00,0x00,0x08,0x00,0x00}, /* [39] '.'                   */
};

/* =========================================================
 *  PRIMITIVAS I2C PARA EL SSD1306
 * ========================================================= */

/**
 * @brief  Envía un byte de comando al controlador SSD1306.
 *
 * El byte de control 0x00 indica al SSD1306 que el siguiente
 * byte es un comando de configuración.
 *
 * @param  cmd  Código de comando (ver datasheet SSD1306).
 */
void OLED_Command(unsigned char cmd) {
    I2C_Start();
    I2C_Write(OLED_ADDR);
    I2C_Write(0x00);  /* Byte de control: modo comando */
    I2C_Write(cmd);
    I2C_Stop();
}

/**
 * @brief  Envía un byte de dato (píxeles) al SSD1306.
 *
 * El byte de control 0x40 indica que el siguiente byte es
 * un dato a escribir en la memoria de pantalla (GDDRAM).
 *
 * @param  data  Byte de 8 píxeles verticales.
 */
void OLED_Data(unsigned char data) {
    I2C_Start();
    I2C_Write(OLED_ADDR);
    I2C_Write(0x40);  /* Byte de control: modo dato */
    I2C_Write(data);
    I2C_Stop();
}

/* =========================================================
 *  INICIALIZACIÓN
 * ========================================================= */

/**
 * @brief  Inicializa el controlador SSD1306.
 *
 * Secuencia de comandos estándar para configurar la pantalla:
 *   0xAE       -> Apagar display (necesario antes de configurar)
 *   0xD5,0xF0  -> Frecuencia de reloj y divisor de oscilador
 *   0xA8,0x3F  -> Modo multiplex 64 (64 líneas = altura de la pantalla)
 *   0xD3,0x00  -> Offset de desplazamiento de display = 0
 *   0x40       -> Dirección de inicio de línea = 0
 *   0x8D,0x14  -> Habilitar carga de bomba (VCC interno)
 *   0x20,0x02  -> Modo de direccionamiento de página
 *   0xA1       -> Mapeo de segmentos: columna 127 -> SEG0
 *   0xC8       -> Escaneo vertical invertido (COM63 -> COM0)
 *   0xDA,0x12  -> Configuración de pines COM: alternado
 *   0x81,0xFF  -> Contraste máximo
 *   0xD9,0xF1  -> Período de pre-carga
 *   0xDB,0x40  -> Nivel de VCOMH
 *   0xA4       -> Salida de RAM activa (no "all ON")
 *   0xA6       -> Modo de visualización normal (no invertido)
 *   0xAF       -> Encender display
 */