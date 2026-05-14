/**
 * @file    adc.h
 * @brief   Interfaz publica del modulo ADC (Convertidor Analogico-Digital).
 *
 * Expone las funciones necesarias para inicializar el ADC del PIC18F4550
 * y realizar lecturas en cualquiera de sus canales analogicos.
 */

#ifndef ADC_H
#define ADC_H

/**
 * @brief  Inicializa el modulo ADC del PIC18F4550.
 *
 * Configura los registros ADCON0, ADCON1 y ADCON2 para:
 *  - Habilitar AN0 y AN2 como entradas analogicas.
 *  - Fijar las referencias de tension a VDD/VSS.
 *  - Seleccionar justificacion derecha y tiempo de adquisicion adecuado.
 */
void init_ADC(void);

/**
 * @brief  Lee el valor digital de un canal analogico.
 *
 * @param  canal  Numero de canal analogico a leer (0 = AN0, 2 = AN2, etc.).
 * @return Resultado de 10 bits (0-1023) proporcional a la tension de entrada.
 */
int leer_adc(int canal);

#endif /* ADC_H */