#ifndef BRAZO
#define BRAZO

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra);
int brazo_reposo();
int brazo_soldar();

#endif