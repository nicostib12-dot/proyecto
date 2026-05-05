#ifndef MQ135_H
#define MQ135_H

#include <xc.h>
#define _XTAL_FREQ 8000000UL

// ?? Constantes del sensor ??????????????????????????
// RL: resistencia de carga del módulo (medir con multímetro)
// La mayoría de módulos chinos traen 1k?
#define RL_VALUE        1.0     // k?

// R0: resistencia en aire limpio (debes calibrarlo tú)
// Valor típico del datasheet para aire limpio: Rs/R0 = 3.6
// Empieza con este valor y ajusta según calibración
#define R0_DEFAULT      10.0    // k? ? ajustar después de calibrar

// Constantes curva CO2 del datasheet MQ-135
// La curva es: ppm = A * (Rs/R0)^B
// Para CO2 aproximado: A=116.6020682, B=-2.769034857
#define CURVE_A         116.6020682
#define CURVE_B         -2.769034857

// Muestras para promedio
#define MQ_MUESTRAS     10

void  MQ135_Init(void);
float MQ135_LeerADC(void);
float MQ135_calcularRs(float adc);
float MQ135_calcularPPM(float rs, float r0);
float MQ135_calibrarR0(void);

#endif