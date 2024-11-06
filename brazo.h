/*
Codigo de cabecera para brazo.cpp
*/


#ifndef BRAZO
#define BRAZO

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra, float antebrazo_largo_E, float brazo_largo_E);
int brazo_reposo();
int brazo_soldar();
void brazo_pinza_cerrar();

#endif