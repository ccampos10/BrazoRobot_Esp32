#include <brazo.h>
// #include <pantalla.h>
#include <ESP32Servo.h>
#include <Stepper.h>

// #define DISTANCIA_DETECCION 0
const int DISTANCIA_DETECCION = 16;
const int stepsPerRevolution = 2048;

const int ANGULO_RETENCION = 1;
const int ANGULO_LIBERACION = 1;

Servo retenedor;

// motores brazo
const int pin_motor_base = 17;
const int pin_motor_antebrazo = 33;
const int pin_motor_brazo = 25;
const int pin_motor_garra = 26;

// sensor de proximidad
const int pin_prox_echo = 12;
const int pin_prox_triger = 13;

// // coneccion serial
// // pin 21 sda
// // pin 22 scl

const int pin_motor_retenedor = 23;
const int pin_temperatrua = 16;
const int pin_boton = 4;

bool etapa_1 = true; // etapa inicial
bool etapa_2 = false;
bool etapa_3 = false;
bool etapa_4 = false;
bool etapa_5 = false;

// const int IN1  = 33;
// const int IN2  = 25; // cambiar
// const int IN3  = 26; // cambiar
// const int IN4  = 27; // cambiar
const int IN1 = 5;
const int IN2 = 18;
const int IN3 = 19;
const int IN4 = 21;

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

bool boton_esta_on(){
  return digitalRead(pin_boton) == 1;
}

void cinta_detener(){
  myStepper.step(0);
}
void cinta_iniciar(){
  myStepper.step(1);
}

void retenedor_liberar(){
  retenedor.attach(ANGULO_LIBERACION);
}
void retenedor_retener(){
  retenedor.attach(ANGULO_RETENCION);
}

bool objeto_proximo(){
  digitalWrite(pin_prox_triger, LOW);
  delayMicroseconds(2);
  digitalWrite(pin_prox_triger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_prox_triger, LOW);
  
  double duracion = pulseIn(pin_prox_echo, HIGH);
  double distancia = duracion * 0.0343 / 2.0;
  // Serial.println(distancia);

  return distancia <= DISTANCIA_DETECCION;
}

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  
  myStepper.setSpeed(4);
  Serial.println("1");
  brazo_init(pin_motor_base, pin_motor_antebrazo, pin_motor_brazo, pin_motor_garra, 7.7, 7.7);
  Serial.println("2");

  // boton setter
  pinMode(pin_boton, INPUT_PULLDOWN);
  Serial.println("3");

  // ultrasonido setter
  pinMode(pin_prox_triger, OUTPUT);
  Serial.println("4");
  pinMode(pin_prox_echo, INPUT);
  Serial.println("5");

  // retenedor setter
  retenedor.attach(pin_motor_retenedor, 500, 2500);
  Serial.println("start 2");
}

void loop() {

  // ver como programar un motor a pasos

  // proceso paralelo a las etapas
  // if (!etapa_1 && boton_esta_on()){
  //   cinta_detener();
  //   retenedor_liberar();
  //   if (brazo_reposo()!=0) { Serial.println("Ocurrio un error"); }
    
  //   Serial.println("Proceso detenido abruptamente.");
  //   etapa_1 = true;
  //   etapa_2 = false;
  //   etapa_3 = false;
  //   etapa_4 = false;
  //   etapa_5 = false;
  // }

  // etapas
  if (etapa_1) {
    brazo_reposo();
    Serial.println("hola");
    if (boton_esta_on()){
      Serial.println("Boton");
      etapa_1 = false;
      etapa_2 = true;
    }
  }
  else if (etapa_2) {
    Serial.println("et 2");
    cinta_iniciar(); // comprobar en la funcion si ya esta encendida y no volver a encender
    etapa_2 = false;
    etapa_3 = true;
  }
  else if (etapa_3) {
    cinta_iniciar();
    if (objeto_proximo()) {
      Serial.println("et 3");
      cinta_detener();
      etapa_3 = false;
      etapa_4 = true;
    }
  }
  else if (etapa_4) {
    Serial.println("et 4");
    retenedor_retener();
    delay(100);
    etapa_4 = false;
    etapa_5 = true;
  }
  else if (etapa_5) {
    if (brazo_soldar()!=0){ Serial.println("Ocurrio un error."); }
    retenedor_liberar();
    delay(100);
    etapa_5 = false;
    etapa_2 = true;
    Serial.println("et 5");
  }
  else {
    Serial.println("Error, ninguna etapa encendida.");
    etapa_1=true;
  }
  delay(10);
}


