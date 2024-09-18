#ifndef BRAZO
#define BRAZO

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra);
void brazo_reposo();
void brazo_soldar();

#endif