/**
 * @file    bme280.c
 * @brief   Driver simplificado para el sensor BME280 (Bosch) via I2C.
 *
 * Implementa escritura y lectura de registros internos del BME280
 * y una funcion de lectura de humedad en modo forzado.
 *
 * Nota: No se aplican coeficientes de calibracion de fabrica.
 * La conversion es lineal y suficiente para uso academico.
 *
 * Direccion I2C: 0x76 (SDO = GND).
 */

#include <xc.h>
#include "i2c.h"
#include "bme280.h"
#include "config.h"

/** Direccion I2C del BME280 cuando el pin SDO esta conectado a GND */
#define BME280_ADDR 0x76

/* =========================================================
 *  FUNCIONES INTERNAS DE ACCESO A REGISTROS
 * ========================================================= */

/**
 * @brief  Escribe un byte en un registro interno del BME280.
 *
 * Protocolo I2C: START -> direccion+W -> registro -> dato -> STOP.
 *
 * @param  reg   Direccion del registro destino.
 * @param  data  Valor a escribir.
 */
void BME280_Write(unsigned char reg, unsigned char data) {
    I2C_Start();
    I2C_Write(BME280_ADDR << 1); /* Direccion + bit de escritura */
    I2C_Write(reg);              /* Registro a escribir */
    I2C_Write(data);             /* Dato */
    I2C_Stop();
}

/**
 * @brief  Lee un byte de un registro interno del BME280.
 *
 * Protocolo I2C (lectura con restart implicito via STOP + START):
 *  1. START -> direccion+W -> registro -> STOP  (apuntar al registro)
 *  2. START -> direccion+R -> leer byte -> STOP (leer el contenido)
 *
 * @param  reg  Direccion del registro a leer.
 * @return Byte leido del sensor.
 */
unsigned char BME280_Read(unsigned char reg) {
    unsigned char data;

    /* Fase 1: Indicar al sensor que registro queremos leer */
    I2C_Start();
    I2C_Write(BME280_ADDR << 1); /* Escritura para apuntar al registro */
    I2C_Write(reg);
    I2C_Stop();
    __delay_us(50);              /* Pausa de seguridad */

    /* Fase 2: Leer el contenido del registro */
    I2C_Start();
    I2C_Write((BME280_ADDR << 1) | 1); /* Lectura */
    data = I2C_Read(0);                /* NACK: unico byte a leer */
    I2C_Stop();
    __delay_us(50);

    return data;
}

/* =========================================================
 *  FUNCIONES PUBLICAS
 * ========================================================= */

/**
 * @brief  Inicializa el sensor BME280.
 *
 * Secuencia de inicializacion:
 *  1. Esperar 100 ms tras el encendido para que el sensor arranque.
 *  2. Registro 0xF2 (ctrl_hum): oversampling de humedad x1.
 *  3. Registro 0xF5 (config): tiempo de espera 1 s, filtro IIR off.
 *
 * Nota: ctrl_hum (0xF2) debe escribirse ANTES que ctrl_meas (0xF4)
 * para que el cambio tenga efecto (requerimiento del datasheet).
 */
void BME280_Init(void) {
    __delay_ms(100);                /* Esperar arranque del sensor */
    BME280_Write(0xF2, 0x01);      /* ctrl_hum: humedad oversampling x1 */
    BME280_Write(0xF5, 0xA0);      /* config: standby 1 s, filtro desactivado */
    __delay_ms(10);
}

/**
 * @brief  Lee la humedad relativa del BME280 en modo forzado.
 *
 * El modo forzado realiza una sola medicion y vuelve al estado
 * de reposo, reduciendo el consumo en sistemas con ciclos lentos.
 *
 * Proceso:
 *  1. Configurar oversampling de humedad x1 (0xF2).
 *  2. Activar modo forzado con oversampling de temperatura y presion x1
 *     escribiendo 0x25 en el registro 0xF4.
 *  3. Esperar 50 ms (tiempo tipico de conversion ~9 ms, 50 ms de margen).
 *  4. Leer los registros 0xFD (MSB) y 0xFE (LSB) de humedad.
 *  5. Convertir el valor de 16 bits a porcentaje (escala lineal):
 *       0x0000 -> 0%    |    0xFFFF -> 100%
 *       porcentaje = raw / 655.36
 *
 * @note  Sin calibracion de fabrica. Valido para proyectos academicos.
 *
 * @return Humedad relativa en % (0.0 - 100.0).
 */
float BME280_ReadHumidity(void) {
    unsigned char msb, lsb;
    unsigned int raw;

    /* Pasos 1-2: Configurar y disparar medicion en modo forzado */
    BME280_Write(0xF2, 0x01);  /* Humedad: oversampling x1 */
    BME280_Write(0xF4, 0x25);  /* Temperatura x1, presion x1, forced mode */

    /* Paso 3: Esperar conversion */
    __delay_ms(50);

    /* Paso 4: Leer los dos bytes del registro de humedad */
    msb = BME280_Read(0xFD);   /* hum_msb: bits [15:8] */
    lsb = BME280_Read(0xFE);   /* hum_lsb: bits [7:0]  */

    /* Combinar en valor de 16 bits */
    raw = ((unsigned int)msb << 8) | lsb;

    /* Paso 5: Escalar a porcentaje (0x0000=0%, 0xFFFF=100%) */
    return (float)raw / 655.36f;
}