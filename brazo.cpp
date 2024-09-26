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

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra, float antebrazo_largo_E, float brazo_largo_E){
  servo_base.attach(pin_motor_base);
  servo_brazo.attach(pin_motor_brazo);
  servo_antebrazo.attach(pin_motor_antebrazo);
  servo_garra.attach(pin_motor_garra);
  Serial.println(pin_motor_brazo);
  antebrazo_largo = (float)antebrazo_largo_E;
  brazo_largo = (float)brazo_largo_E;
  seteado = true;
}

void brazo_set_cero(){
  servo_base.write(0);
  servo_brazo.write(0);
  servo_antebrazo.write(0);
  servo_garra.write(0);
}

int brazo_set_pos(float x, float y, float z){
  if (!seteado) { return -1; }
  if (x<0 || y<0 || z<0) { return -1; }
  // eje x y eje y en el piso, el eje z es positivo hacia arriba

  float angulo_base = atan(y/x)*180.0/M_PI;
  // float x_recal = x;
  // float z_recal = z;
  float x_recal = sqrt(pow(x,2)+pow(y,2)); // recalcular la pos x;
  float z_recal = z;
  if (sqrt(pow(x_recal,2)+pow(z,2)) > antebrazo_largo+brazo_largo) {
    x_recal = x_recal/(sqrt(pow(x_recal,2)+pow(z,2)))*(float)(antebrazo_largo+brazo_largo);
    z_recal = z_recal/(sqrt(pow(x_recal,2)+pow(z,2)))*(float)(antebrazo_largo+brazo_largo);
  }

  // Serial.println(b);
  // Serial.println(c);
  // Serial.println();

  float d = (pow(brazo_largo,2)-pow(antebrazo_largo,2)-pow(x_recal,2)-pow(z_recal,2))/(-2.0);
  float a = pow(z_recal,2)-pow(x_recal,2);
  float b = 2.0*x_recal*d;
  float c = pow(z_recal,2)*pow(antebrazo_largo,2)-pow(d,2);
  float x1 = (-b + sqrt(round(pow(b,2) - 4.0*a*c)))/(2.0*a);
  float x2 = (-b - sqrt(round(pow(b,2) - 4.0*a*c)))/(2.0*a);

  Serial.println(round((pow(b,2) - 4.0*a*c)));
  Serial.println(d);
  Serial.println(a);
  Serial.println(b);
  Serial.println(c);
  Serial.println();
  Serial.println(x1);
  Serial.println(x2);

  int angulo_antebrazo;
  int angulo_brazo;
  float y1;
  if(d == z_recal*sqrt(pow(antebrazo_largo,2)-pow(x1,2))+x1*x_recal){
    Serial.println("1");
    y1 = sqrt(pow(antebrazo_largo,2)-pow(x1,2));
    angulo_antebrazo = atan(y1/x1)*180.0/M_PI;
    angulo_brazo = atan((y1-z_recal)/(x_recal-x1))*180.0/M_PI;
    
  }
  else {
    Serial.println("2");
    y1 = sqrt(pow(antebrazo_largo,2)-pow(x2,2));
    angulo_antebrazo = atan(y1/x_recal)*180.0/M_PI;
    angulo_brazo = atan((z_recal-y1)/(x_recal-x2))*180.0/M_PI;
  }

  Serial.println(angulo_base);
  Serial.println(angulo_antebrazo);
  Serial.println(angulo_brazo);
  if (angulo_base<0 || angulo_base >180) { return -1; }
  if (angulo_antebrazo<0 || angulo_antebrazo >180) { return -1; }
  if (angulo_brazo<0 || angulo_brazo >180) { return -1; }

  servo_base.write(angulo_base);
  servo_antebrazo.write(angulo_antebrazo);
  servo_brazo.write(angulo_brazo);

  return 0;
}
// inutil
int brazo_reposo(){
  if (!seteado) { return -1; }
  return brazo_set_pos(-1,-1,-1);
}

int brazo_soldar(){
  if (!seteado) { return -1; }
  int puntos[4][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}}; // no recuerdo el orden

  for (int i = 0; i<4; i++ ){
    if (brazo_set_pos(puntos[i][0], puntos[i][1], puntos[i][2]) != 0) {return -1;}
  }
  return 0;
}