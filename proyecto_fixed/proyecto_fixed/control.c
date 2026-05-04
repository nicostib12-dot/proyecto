#include "config.h"
#include "pwm.h"

static int contador_aire = 0;

void control_temperatura(float T)
{
static int calentador_on = 0;
if(!calentador_on && T < 20) { CALENTADOR = 1; calentador_on = 1; }
else if(calentador_on && T > 24) { CALENTADOR = 0; calentador_on = 0; }
}

void control_luz(int luz)
{
static int led_on = 0;
if(!led_on && luz < 450) { LED = 1; led_on = 1; }
else if(led_on && luz > 550) { LED = 0; led_on = 0; }
}

void control_aire(int aire)
{
    if(aire > 300)
    {
        contador_aire++;

        set_PWM_duty(1023); // ventilación máxima

        if(contador_aire > 10)
            ALARMA = 1;
    }
    else
    {
        contador_aire = 0;
        ALARMA = 0;

        set_PWM_duty(300); // ventilación normal
    }
}

void control_humedad(float H)
{
    if(H < 40)
    {
        set_PWM_duty(200);
    }
    else if(H > 60)
    {
        set_PWM_duty(900);
    }
    // entre 40 y 60 no cambia (histéresis)
}