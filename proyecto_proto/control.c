#include "config.h"
#include "pwm.h"
#include "control.h"

static int contador_aire = 0;

// Control de temperatura con histeresis
// Setpoint: 22C, histeresis +-2C
// Calentador ON si T < 20, OFF si T > 24
void control_temperatura(float T) {
    static int calentador_on = 0;
    if (!calentador_on && T < 20.0) {
        CALENTADOR = 1;
        calentador_on = 1;
    } else if (calentador_on && T > 24.0) {
        CALENTADOR = 0;
        calentador_on = 0;
    }
    // Entre 20 y 24: mantener estado (histeresis)
}

// Control de iluminacion con histeresis
// Setpoint: 500 lux, histeresis +-50 lux
// LED ON si lux < 450, OFF si lux > 550
void control_luz(float lux) {
    static int led_on = 0;
    if(!led_on && lux < 450.0) { LED_TIRA = 1; led_on = 1; }
    else if(led_on && lux > 550.0) { LED_TIRA = 0; led_on = 0; }
}

// Control de calidad de aire
// Umbral: 300 ADC
// Alarma si supera umbral por mas de 10 ciclos (~10 segundos)
void control_aire(int aire) {
    if (aire > 300) {
        contador_aire++;
        set_PWM_duty(1023);   // Ventilacion maxima
        if (contador_aire > 10) {
            ALARMA = 1;       // Activar alarma tras 10 segundos
        }
    } else {
        contador_aire = 0;
        ALARMA = 0;
        // Ventilacion vuelve a control por humedad
    }
}

// Control de ventilacion por humedad
// Setpoint: 50% HR, histeresis +-10%
// Ventilacion baja si HR < 40%, alta si HR > 60%
void control_humedad(float H) {
    if (H < 40.0) {
        set_PWM_duty(200);    // Ventilacion baja — 20%
    } else if (H > 60.0) {
        set_PWM_duty(900);    // Ventilacion alta — 88%
    }
    // Entre 40 y 60%: mantener estado (histeresis)
}
