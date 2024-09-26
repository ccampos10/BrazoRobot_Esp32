#include <cmath>
#include <brazo.h>
#include <ESP32Servo.h>

const int SERVO_BASE_CERO_CORRECCION = 0;
const int SERVO_ANTEBRAZO_CERO_CORRECCION = 20;
const int SERVO_BRAZO_CERO_CORRECCION = 140; // 55
const int SERVO_GARRA_CERO_CORRECCION = 0;

const int SERVO_BASE_EXTENDIDO_CORRECCION = 180;
const int SERVO_ANTEBRAZO_EXTENDIDO_CORRECCION = 150;
const int SERVO_BRAZO_EXTENDIDO_CORRECCION = 55; // 140
const int SERVO_GARRA_EXTENDIDO_CORRECCION = 0;

const int SERVO_GARRA_CERRADO = 30;

void brazo_pinza_cerrar() {
  servo_garra.write(SERVO_GARRA_CERRADO);
}

Servo servo_base;
Servo servo_antebrazo;
Servo servo_brazo;
Servo servo_garra;
static float antebrazo_largo = -1;
static float brazo_largo = -1;
static bool seteado = false;

float roundDos(float numero) {
  return round(numero * 10000) / 10000;
}

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra, float antebrazo_largo_E, float brazo_largo_E){
  servo_base.attach(pin_motor_base,500,2500);
  servo_antebrazo.attach(pin_motor_antebrazo,500,2500);
  servo_brazo.attach(pin_motor_brazo,500,2500);
  servo_garra.attach(pin_motor_garra,500,2500);
  antebrazo_largo = (float)antebrazo_largo_E;
  brazo_largo = (float)brazo_largo_E;
  seteado = true;
}

void brazo_set_cero(){
  servo_base.write(90);
  servo_antebrazo.write(20);
  servo_brazo.write(90);
  servo_garra.write(0);
}

void mover_base(int angulo) {
  servo_base.write(angulo);
}

void mover_antebrazo(int angulo){
  angulo = map(angulo, 0, 180, SERVO_ANTEBRAZO_CERO_CORRECCION, SERVO_ANTEBRAZO_EXTENDIDO_CORRECCION);
  int angulo_var = servo_antebrazo.read() - angulo;
  // if (angulo_var<0) { angulo_var = angulo_var*-1; }
  int angulo2 = servo_brazo.read()-angulo_var;
  angulo2 = map(angulo2, 0, 180, SERVO_BRAZO_CERO_CORRECCION, SERVO_BRAZO_EXTENDIDO_CORRECCION);

  servo_antebrazo.write(angulo);
  Serial.println(angulo);
  Serial.println(angulo2);
  delay(50);
  servo_brazo.write(angulo2);
}

void mover_brazo(int angulo){
  servo_brazo.write(map(angulo, 0, 180, SERVO_BRAZO_CERO_CORRECCION, SERVO_BRAZO_EXTENDIDO_CORRECCION));
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
  if (sqrt(pow(x_recal,2)+pow(z,2)) > antebrazo_largo+brazo_largo) { // el recal deveria ser si el eje 'z' y 'x' salen del espacio de movimientos
    x_recal = x_recal/(sqrt(pow(x_recal,2)+pow(z,2)))*(float)(antebrazo_largo+brazo_largo);
    z_recal = z_recal/(sqrt(pow(x_recal,2)+pow(z,2)))*(float)(antebrazo_largo+brazo_largo);
  }

  Serial.println();
  Serial.println(x_recal);
  Serial.println(z_recal);
  Serial.println();

  float d = (pow(brazo_largo,2)-pow(antebrazo_largo,2)-pow(x_recal,2)-pow(z_recal,2))/(-2.0);
  float a = pow(z_recal,2)-pow(x_recal,2);
  float b = 2.0*x_recal*d;
  float c = pow(z_recal,2)*pow(antebrazo_largo,2)-pow(d,2);
  float x1 = (-b + sqrt(roundDos(pow(b,2) - 4.0*a*c)))/(2.0*a);
  float x2 = (-b - sqrt(roundDos(pow(b,2) - 4.0*a*c)))/(2.0*a);

  Serial.println(roundDos((pow(b,2) - 4.0*a*c)));
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

  Serial.println();
  Serial.println(d);
  Serial.println(z_recal*sqrt(pow(antebrazo_largo,2)-pow(x1,2))+x1*x_recal);
  Serial.println();
  
  if(roundDos(d) == roundDos(z_recal*sqrt(pow(antebrazo_largo,2)-pow(x1,2))+x1*x_recal)){
    Serial.println("1");
    y1 = sqrt(pow(antebrazo_largo,2)-pow(x1,2));
    Serial.println(y1);
    angulo_antebrazo = atan(y1/x1)*180.0/M_PI;
    angulo_brazo = atan((y1-z_recal)/(x_recal-x1))*180.0/M_PI;
  }

  else {
    Serial.println("2");
    y1 = sqrt(pow(antebrazo_largo,2)-pow(x2,2));
    Serial.println(y1);
    angulo_antebrazo = atan(y1/x_recal)*180.0/M_PI;
    angulo_brazo = atan((y1-z_recal)/(x_recal-x2))*180.0/M_PI;
  }

  Serial.println(angulo_base);
  Serial.println(angulo_antebrazo);
  Serial.println(angulo_brazo);
  if (angulo_base<0 || angulo_base >180) { return -1; }
  if (angulo_antebrazo<0 || angulo_antebrazo >180) { return -1; }
  if (angulo_brazo<0 || angulo_brazo >180) { return -1; }

  servo_base.write(angulo_base);
  servo_antebrazo.write(angulo_antebrazo);
  servo_brazo.write(SERVO_BRAZO_EXTENDIDO_CORRECCION-angulo_brazo);

  return 0;
}

// inutil
int brazo_reposo(){
  if (!seteado) { return -1; }
  return brazo_set_pos(0.001,7,0.0001);
}

int brazo_soldar(){
  if (!seteado) { return -1; }
  int puntos[4][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}}; // no recuerdo el orden

  for (int i = 0; i<4; i++ ){
    if (brazo_set_pos(puntos[i][0], puntos[i][1], puntos[i][2]) != 0) {return -1;}
  }
  return 0;
}