#include "control.h"

static unsigned char estado_calentador  = 0;
static unsigned char estado_ventilador  = 0;
static unsigned char estado_led         = 0;
static unsigned char estado_alarma      = 0;

static unsigned char vent_por_humedad   = 0;
static unsigned char vent_por_aire      = 0;

void Control_Init(void) {
    // TRISD: 0 = salida, 1 = entrada
    TRISDbits.TRISD0 = 0;   // Ventilador
    TRISDbits.TRISD1 = 0;   // Calentador
    TRISDbits.TRISD2 = 0;   // Tira LED
    TRISDbits.TRISD3 = 0;   // Alarma

    // Apagar todos al inicio ? estado seguro
    VENTILADOR = 0;
    CALENTADOR = 0;
    TIRA_LED   = 0;
    ALARMA     = 0;

    estado_calentador = 0;
    estado_ventilador = 0;
    estado_led        = 0;
    estado_alarma     = 0;
}

void Control_Temperatura(float temp) {
    if(estado_calentador == 0) {
        // Calentador apagado ? encender si baja de T_REF
        if(temp < TEMP_REF) {
            CALENTADOR = 1;
            estado_calentador = 1;
        }
    } else {
        // Calentador encendido ? apagar si supera T_REF + HIST
        if(temp > (TEMP_REF + TEMP_HISTERESIS)) {
            CALENTADOR = 0;
            estado_calentador = 0;
        }
    }
}

void Control_Humedad(float hum) {
    if(hum > HUM_ALTA) {
        vent_por_humedad = 1;
    } else if(hum < HUM_BAJA) {
        vent_por_humedad = 0;
    }
 
}

void Control_Aire(float ppm) {
    if(ppm > AIRE_MALO) {
        vent_por_aire = 1;
    } else if(ppm < AIRE_BUENO) {
        vent_por_aire = 0;
    }

    // Alarma si el aire es peligroso
    if(ppm > AIRE_PELIGRO) {
        ALARMA = 1;
        estado_alarma = 1;
    } else {
        ALARMA = 0;
        estado_alarma = 0;
    }
}

void Control_Luz(float lux) {
    if(estado_led == 0) {
        if(lux < LUX_BAJA) {
            TIRA_LED = 1;
            estado_led = 1;
        }
    } else {
        if(lux > LUX_SUFICIENTE) {
            TIRA_LED = 0;
            estado_led = 0;
        }
    }
}

void Control_Ejecutar(float temp, float hum, float ppm, float lux) {
    Control_Temperatura(temp);
    Control_Humedad(hum);
    Control_Aire(ppm);
    Control_Luz(lux);

    if(vent_por_humedad || vent_por_aire) {
        VENTILADOR = 1;
        estado_ventilador = 1;
    } else {
        VENTILADOR = 0;
        estado_ventilador = 0;
    }
}

unsigned char Control_GetVentilador(void)  { return estado_ventilador; }
unsigned char Control_GetCalentador(void)  { return estado_calentador; }
unsigned char Control_GetLed(void)         { return estado_led;        }
unsigned char Control_GetAlarma(void)      { return estado_alarma;     }
