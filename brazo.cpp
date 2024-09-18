#include <cmath>
#include <brazo.h>
#include <ESP32Servo.h>

static Servo servo_base;
static Servo servo_antebrazo;
static Servo servo_brazo;
static Servo servo_garra;
static float antebrazo_largo = -1;
static float brazo_largo = -1;
static bool seteado = false;

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra, int antebrazo_largo_E, int brazo_largo_E){
  servo_base.attach(pin_motor_base);
  servo_antebrazo.attach(pin_motor_antebrazo);
  servo_brazo.attach(pin_motor_brazo);
  servo_garra.attach(pin_motor_garra);
  antebrazo_largo = (float)antebrazo_largo_E;
  brazo_largo = (float)brazo_largo_E;
  seteado = true;
}

static int brazo_set_pos(float x, float y, float z){
  if (!seteado) { return -1; }
  if (x<0 || y<0 || z<0) { return -1; }
  // eje x y eje y en el piso, el eje z es positivo hacia arriba

  int angulo_base = atan(y/x);
  float x_recal = sqrt(x^2+y^2); // recalcular la pos x;
  float z_recal = z;
  if (sqrt(x_recal^2+z^2) > antebrazo_largo+brazo_largo) {
    x_recal = x_recal/(sqrt(x_recal^2+z^2))*(float)(antebrazo_largo+brazo_largo);
    z_recal = z_recal/(sqrt(x_recal^2+z^2))*(float)(antebrazo_largo+brazo_largo);
  }

  float d = (brazo_largo^2-antebrazo_largo^2-x_recal^2-z_recal^2)/(-2.0);
  float a = z_recal^2-x_recal^2;
  float b = 2.0*x_recal*d;
  float c = z_recal^2*antebrazo_largo^2-d^2;
  float x1 = (-b + sqrt(b^2 - 4.0*a*c))/(2.0*a);
  float x2 = (-b - sqrt(b^2 - 4.0*a*c))/(2.0*a);

  int angulo_antebrazo;
  int angulo_brazo;
  float y1;
  if(d == z_recal*sqrt(antebrazo_largo^2-x1^2)+x1*x_recal){
    y1 = sqrt(antebrazo_largo^2-x1^2);
    angulo_antebrazo = atan(y1/x_recal);
    angulo_brazo = atan((z_recal-y1)/(x_recal-x1));
  }
  else {
    y1 = sqrt(antebrazo_largo^2-x2^2);
    angulo_antebrazo = atan(y1/x_recal);
    angulo_brazo = atan((z_recal-y1)/(x_recal-x2));
  }

  if (angulo_base<0 || angulo_base >180) { return -1; }
  if (angulo_antebrazo<0 || angulo_antebrazo >180) { return -1; }
  if (angulo_brazo<0 || angulo_brazo >180) { return -1; }

  servo_base.write(angulo_base);
  servo_antebrazo.write(angulo_antebrazo);
  servo_brazo.write(angulo_brazo);

  return 0;
}

int brazo_reposo(){
  if (!seteado) { return -1; }
  return brazo_set_pos(-1,-1,-1);
}

int brazo_soldar(){
  if (!seteado) { return -1; }
  int puntos[3][4] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}}; // no recuerdo el orden

  for (int i = 0; i<4; i++ ){
    if (brazo_set_pos(puntos[i][0], puntos[i][1], puntos[i][2]) != 0) {return -1;}
  }
  return 0;
}