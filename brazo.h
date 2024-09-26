#ifndef BRAZO
#define BRAZO

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra, float antebrazo_largo_E, float brazo_largo_E);
void brazo_set_cero();
int brazo_set_pos(float x, float y, float z);
int brazo_reposo();
int brazo_soldar();
void mover_antebrazo(int angulo);
void mover_base(int angulo);
void mover_brazo(int angulo);
void brazo_pinza_cerrar();

#endif