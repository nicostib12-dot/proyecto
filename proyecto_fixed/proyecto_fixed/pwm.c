#include <xc.h>
#include "config.h"
#include "pwm.h"

void init_PWM() {

    TRISCbits.TRISC2 = 0; // CCP1 como salida

    PR2 = 255; // periodo PWM

    CCP1CON = 0b00001100; // modo PWM

    T2CON = 0b00000100; // Timer2 ON
}

void set_PWM_duty(int duty) {

    if(duty > 1023) duty = 1023;

    CCPR1L = (unsigned char)(duty >> 2);
    CCP1CONbits.DC1B = duty & 0x03;
}
