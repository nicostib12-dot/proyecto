#ifndef CONTROL_H
#define CONTROL_H

void control_temperatura(float T);
void control_luz(float lux);       // ahora recibe float lux reales del BH1750
void control_aire(int aire);
void control_humedad(float H);

#endif
