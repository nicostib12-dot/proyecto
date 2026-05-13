/**
 * @file    control.c
 * @brief   Logica de control de actuadores del sistema ambiental.
 *
 * Este modulo implementa el control de cuatro actuadores:
 *   1. Calentador (nicromo en RD0) -> control por histeresis de temperatura.
 *   2. Tira LED   (RD2)            -> control por histeresis de iluminancia.
 *   3. Alarma     (buzzer en RD3)  -> activada por mala calidad de aire.
 *   4. Ventilador (PWM en RC2)     -> velocidad segun aire y humedad,
 *                                     con rampa suave para proteger el motor.
 *
 * Todos los pines de actuador estan definidos en config.h.
 * El ventilador comparte el unico canal PWM disponible (RC2/CCP1),
 * por eso aire y humedad se unifican en una sola funcion: control_ventilador().
 */

#include "config.h"
#include "pwm.h"
#include "control.h"

/* Contador global de ciclos consecutivos con aire malo.
 * Se incrementa en control_aire() para retardar la activacion de la alarma. */
static int contador_aire = 0;

/* =========================================================
 *  CONTROL DE TEMPERATURA
 * ========================================================= */

/**
 * @brief  Controla el calentador usando histeresis de temperatura.
 *
 * La histeresis evita que el calentador se encienda y apague
 * repetidamente cuando la temperatura oscila alrededor de un umbral.
 *
 *   T < 24 gC -> ENCENDER calentador
 *   T > 30 gC -> APAGAR  calentador
 *   24 <= T <= 30 gC -> mantener el estado anterior
 *
 * @param  T  Temperatura actual en gC.
 */
void control_temperatura(float T) {
    static int calentador_on = 0; /* Estado interno del calentador */

    if (!calentador_on && T < 24.0f) {
        CALENTADOR = 1;           /* Encender si la temperatura es baja */
        calentador_on = 1;
    } else if (calentador_on && T > 30.0f) {
        CALENTADOR = 0;           /* Apagar si ya se alcanzo la temperatura */
        calentador_on = 0;
    }
    /* En zona intermedia (24-30 gC): no se modifica el actuador */
}

/* =========================================================
 *  CONTROL DE ILUMINACION
 * ========================================================= */

/**
 * @brief  Controla la tira LED usando histeresis de iluminancia.
 *
 *   lux < 450 -> ENCENDER tira LED (demasiado oscuro)
 *   lux > 550 -> APAGAR  tira LED (suficiente luz natural)
 *   450 <= lux <= 550 -> mantener el estado anterior
 *
 * @param  lux  Iluminancia actual en lux (del BH1750).
 */
void control_luz(float lux) {
    static int led_on = 0; /* Estado interno de la tira LED */

    if (!led_on && lux < 450.0f) {
        LED_TIRA = 1;       /* Encender tira si hay poca luz */
        led_on = 1;
    } else if (led_on && lux > 550.0f) {
        LED_TIRA = 0;       /* Apagar tira si hay suficiente luz */
        led_on = 0;
    }
}

/* =========================================================
 *  CONTROL DEL VENTILADOR (FUNCION INTERNA)
 * ========================================================= */

/**
 * @brief  Calcula y aplica la velocidad objetivo del ventilador.
 *
 * Unifica el control de calidad de aire y humedad en un solo PWM,
 * ya que ambos usan el mismo pin RC2.
 *
 * Jerarquia de prioridades:
 *  1. Aire malo (ADC > 700)  -> duty maximo (1023) para ventilar.
 *  2. Humedad baja (< 40 %) -> duty bajo   (200), circulacion suave.
 *  3. Humedad alta (> 45 %) -> duty alto   (900), evacua humedad rapido.
 *  4. Condicion normal       -> duty = 0, ventilador apagado.
 *
 * Rampa suave: en lugar de saltar directamente al duty objetivo,
 * el valor cambia en pasos de 50 por ciclo (~1 s/ciclo). Esto evita
 * el pico de corriente de arranque (inrush current) del motor.
 *
 * @param  aire  Valor ADC del MQ-135.
 * @param  H     Humedad relativa en %.
 */
static void actualizar_ventilador(int aire, float H) {
    static int duty_actual = 0; /* Duty cycle actual del PWM (persiste entre llamadas) */
    int duty_objetivo = 0;

    /* --- Determinar velocidad objetivo segun prioridad --- */
    if (aire > 700) {
        duty_objetivo = 1023;  /* Prioridad maxima: ventilar por aire malo */
    } else if (H < 40.0f) {
        duty_objetivo = 200;   /* Humedad baja: circulacion suave */
    } else if (H > 45.0f) {
        duty_objetivo = 900;   /* Humedad alta: extraccion rapida */
    } else {
        duty_objetivo = 0;     /* Condicion normal: apagado */
    }

    /* --- Rampa suave: acercar duty_actual al objetivo de a 50 por ciclo --- */
    if (duty_actual < duty_objetivo) {
        duty_actual += 50;
        if (duty_actual > duty_objetivo) duty_actual = duty_objetivo;
    } else if (duty_actual > duty_objetivo) {
        duty_actual -= 50;
        if (duty_actual < duty_objetivo) duty_actual = duty_objetivo;
    }

    /* --- Aplicar el duty cycle calculado al PWM --- */
    set_PWM_duty(duty_actual);
}

/* =========================================================
 *  CONTROL DE CALIDAD DE AIRE
 * ========================================================= */

/**
 * @brief  Activa la alarma si la calidad del aire es mala por varios ciclos.
 *
 * Para evitar falsas alarmas por picos momentaneos del sensor MQ-135,
 * se requiere que el valor supere 700 durante al menos 4 ciclos
 * consecutivos antes de activar el buzzer/LED.
 *
 * La alarma consiste en 200 pulsos de 500 us (tono de ~1 kHz por ~0.2 s).
 *
 * Si el aire mejora, el contador y la alarma se resetean.
 *
 * @param  aire  Valor ADC del MQ-135 (0-1023).
 */
void control_aire(int aire) {
    if (aire > 700) {
        contador_aire++;

        if (contador_aire > 3) {
            /* Generar tono de alarma: 200 pulsos cuadrados a ~1 kHz */
            unsigned char i;
            for (i = 0; i < 200; i++) {
                ALARMA = 1; __delay_us(500);
                ALARMA = 0; __delay_us(500);
            }
        }
    } else {
        /* Aire dentro del rango aceptable -> resetear contador y alarma */
        contador_aire = 0;
        ALARMA = 0;
    }
}

/* =========================================================
 *  STUB DE HUMEDAD (compatibilidad con main.c)
 * ========================================================= */

/**
 * @brief  Funcion vacia de compatibilidad para control_humedad().
 *
 * El control real del ventilador por humedad esta integrado en
 * actualizar_ventilador(), llamada desde control_ventilador().
 * Esta funcion existe solo para no modificar las firmas en control.h
 * ni las llamadas en main.c.
 */
void control_humedad(float H) {
    /* Sin implementacion: la logica esta en actualizar_ventilador() */
    (void)H; /* Suprimir advertencia de parametro no usado */
}

/* =========================================================
 *  PUNTO DE ENTRADA DEL CONTROL DEL VENTILADOR
 * ========================================================= */

/**
 * @brief  Punto de entrada publico para el control unificado del ventilador.
 *
 * Debe llamarse al final de cada ciclo en main.c, despues de haber
 * leido todos los sensores y ejecutado el resto de funciones de control.
 *
 * @param  aire  Valor ADC del MQ-135 (calidad de aire).
 * @param  H     Humedad relativa en %.
 */
void control_ventilador(int aire, float H) {
    actualizar_ventilador(aire, H);
}