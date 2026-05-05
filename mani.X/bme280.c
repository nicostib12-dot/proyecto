#include "bme280.h"
#include "i12c.h"

static unsigned int  dig_T1;
static int           dig_T2, dig_T3;

// Humedad
static unsigned char dig_H1;
static int           dig_H2;
static unsigned char dig_H3;
static int           dig_H4, dig_H5;
static signed char   dig_H6;

// t_fine: variable intermedia requerida para compensar humedad
static long          t_fine;

static void BME280_WriteReg(unsigned char reg, unsigned char val) {
    I2C_Start();
    I2C_Write(BME280_ADDR);     // Direccion + escritura
    I2C_Write(reg);             // Registro destino
    I2C_Write(val);             // Valor a escribir
    I2C_Stop();
    __delay_ms(5);
}

// Lee un byte de un registro del BME280
static unsigned char BME280_ReadReg(unsigned char reg) {
    unsigned char dato;

    // Primero escribimos el registro que queremos leer
    I2C_Start();
    I2C_Write(BME280_ADDR);     // Direccion + escritura
    I2C_Write(reg);             // Registro a leer
    I2C_Stop();
    __delay_us(50);

    // Luego leemos el dato
    I2C_Start();
    I2C_Write(BME280_ADDR | 0x01); // Direccion + lectura
    // Habilitar recepcion I2C
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    dato = SSPBUF;
    // NACK para terminar lectura
    SSPCON2bits.ACKDT = 1;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    I2C_Stop();

    return dato;
}

// Lee 2 bytes seguidos (little-endian) ? retorna int de 16 bits
static int BME280_ReadInt16(unsigned char reg) {
    unsigned char lsb, msb;
    lsb = BME280_ReadReg(reg);
    msb = BME280_ReadReg(reg + 1);
    return (int)((msb << 8) | lsb);
}

// Lee 2 bytes sin signo
static unsigned int BME280_ReadUInt16(unsigned char reg) {
    unsigned char lsb, msb;
    lsb = BME280_ReadReg(reg);
    msb = BME280_ReadReg(reg + 1);
    return (unsigned int)((msb << 8) | lsb);
}

static void BME280_LeerCalibracion(void) {
    unsigned char e5, e4, e6;

    // Calibracion temperatura (registros 0x88 al 0x8D)
    dig_T1 = BME280_ReadUInt16(0x88);
    dig_T2 = BME280_ReadInt16(0x8A);
    dig_T3 = BME280_ReadInt16(0x8C);

    // Calibracion humedad
    dig_H1 = BME280_ReadReg(0xA1);
    dig_H2 = BME280_ReadInt16(0xE1);
    dig_H3 = BME280_ReadReg(0xE3);

    // H4 y H5 comparten un byte (registro 0xE5) ? cálculo especial
    e4 = BME280_ReadReg(0xE4);
    e5 = BME280_ReadReg(0xE5);
    e6 = BME280_ReadReg(0xE6);

    dig_H4 = (int)(((int)e4 << 4) | (e5 & 0x0F));
    dig_H5 = (int)(((int)e6 << 4) | (e5 >> 4));
    dig_H6 = (signed char)BME280_ReadReg(0xE7);
}

static void BME280_CalcularTFine(void) {
    unsigned char msb, lsb, xlsb;
    long adc_T;
    long var1, var2;

    // Leer 3 bytes de temperatura cruda (registros 0xFA, 0xFB, 0xFC)
    msb  = BME280_ReadReg(0xFA);
    lsb  = BME280_ReadReg(0xFB);
    xlsb = BME280_ReadReg(0xFC);

    // Combinar en valor de 20 bits
    adc_T = ((long)msb << 12) | ((long)lsb << 4) | (xlsb >> 4);

    // Compensacion temperatura segun datasheet BME280
    var1 = ((((adc_T >> 3) - ((long)dig_T1 << 1))) *
             ((long)dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((long)dig_T1)) *
              ((adc_T >> 4) - ((long)dig_T1))) >> 12) *
             ((long)dig_T3)) >> 14;

    // t_fine es la variable intermedia global
    t_fine = var1 + var2;
}

// ?????????????????????????????????????????????????????
// INICIALIZACION del BME280
// Retorna 1 si el sensor responde, 0 si hay error
// ?????????????????????????????????????????????????????
unsigned char BME280_Init(void) {
    unsigned char chip_id;
    __delay_ms(100);  // Espera arranque del sensor

    // Verificar que el sensor responde ? debe leer 0x60
    chip_id = BME280_ReadReg(BME280_REG_ID);
    if(chip_id != 0x60) {
        return 0;  // Sensor no encontrado o mal conectado
    }

    // Reset del sensor
    BME280_WriteReg(BME280_REG_RESET, 0xB6);
    __delay_ms(100);

    // Leer coeficientes de calibracion
    BME280_LeerCalibracion();

    // Configurar humedad: oversampling x1 (precision normal)
    // IMPORTANTE: este registro debe escribirse ANTES del ctrl_meas
    BME280_WriteReg(BME280_REG_CTRL_HUM, 0x01);

    BME280_WriteReg(BME280_REG_CTRL_MSR, 0x23);

    // Standby 1000ms entre mediciones, filtro off
    BME280_WriteReg(BME280_REG_CONFIG, 0xA0);

    __delay_ms(200);  // Espera primera medicion

    return 1;  // Sensor OK
}

float BME280_ReadHumidity(void) {
    unsigned char msb, lsb;
    long adc_H;
    long x1;
    float humedad;

    // Paso 1: Actualizar t_fine con temperatura actual
    BME280_CalcularTFine();

    // Paso 2: Leer 2 bytes de humedad cruda (0xFD y 0xFE)
    msb   = BME280_ReadReg(0xFD);
    lsb   = BME280_ReadReg(0xFE);
    adc_H = ((long)msb << 8) | lsb;

    // Paso 3: Compensacion segun formula del datasheet BME280
    x1 = t_fine - 76800L;

    x1 = (((((adc_H << 14) -
             ((long)dig_H4 << 20) -
             ((long)dig_H5 * x1)) +
            16384L) >> 15) *
           (((((((x1 * (long)dig_H6) >> 10) *
               (((x1 * (long)dig_H3) >> 11) + 32768L)) >> 10) +
              2097152L) * (long)dig_H2 + 8192L) >> 14));

    x1 = x1 - (((((x1 >> 15) * (x1 >> 15)) >> 7) *
                 (long)dig_H1) >> 4);

    // Limitar al rango valido
    if(x1 < 0)          x1 = 0;
    if(x1 > 419430400L) x1 = 419430400L;

    // Convertir a float %RH
    humedad = (float)(x1 >> 12) / 1024.0;

    // Limitar rango fisico real
    if(humedad < 0.0)   humedad = 0.0;
    if(humedad > 100.0) humedad = 100.0;

    return humedad;
}