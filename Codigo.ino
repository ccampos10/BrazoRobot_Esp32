#include <brazo.h>
#include <pantalla.h>
#include <ESP32Servo.h>
#include <Stepper.h>

// #define DISTANCIA_DETECCION 0
const int stepsPerRevolution = 2048;

const int ANGULO_RETENCION = 1;
const int ANGULO_LIBERACION = 1;

Servo retenedor;

// motores brazo
const int pin_motor_base = 17;
const int pin_motor_antebrazo = 5;
const int pin_motor_brazo = 18;
const int pin_motor_garra = 19;

// sensor de proximidad
const int pin_prox_echo = 34;
const int pin_prox_triger = 35;

// coneccion serial
// pin 21 sda
// pin 22 scl

const int pin_motor_retenedor = 23;
const int pin_temperatrua = 16;
const int pin_boton = 4;

bool etapa_1 = true; // etapa inicial
bool etapa_2 = false;
bool etapa_3 = false;
bool etapa_4 = false;
bool etapa_5 = false;

const int IN1  = 5;
const int IN2  = 18;
const int IN3  = 19;
const int IN4  = 21;

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

bool boton_esta_on(){
  return digitalRead(pin_boton) == 1;
}

void cinta_detener(){
  myStepper.step(0);
}
void cinta_iniciar(){
  myStepper.step(stepsPerRevolution);
}

void retenedor_liberar(){
  retenedor.attach(ANGULO_LIBERACION);
}
void retenedor_retener(){
  retenedor.attach(ANGULO_RETENCION);
}

bool objeto_proximo(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  
  double duracion = pulseIn(echo_pin, HIGH);
  double distancia = duracion * 0.0343 / 2.0;

  return distancia <= DISTANCIA_DETECCION;
}

void setup() {
  Serial.begin(115200);
  
  myStepper.setSpeed(2);
  brazo_init(pin_motor_base, pin_motor_antebrazo, pin_motor_brazo, pin_motor_garra, 7.7, 7.7);

  // boton setter
  pinMode(pin_boton, INPUT_PULLDOWN);

  // ultrasonido setter
  pinMode(pin_prox_triger, OUTPUT);
  pinMode(pin_prox_echo, INPUT);

  // retenedor setter
  servo.attach(pin_motor_retenedor, 500, 2500);
}

void loop() {

  // ver como programar un motor a pasos

  // proceso paralelo a las etapas
  if (!etapa_1 && boton_esta_on()){
    cinta_detener();
    retenedor_liberar();
    if (brazo_reposo()!=0) { Serial.println("Ocurrio un error"); }
    
    Serial.println("Proceso detenido abruptamente.");
    etapa_1 = true;
    etapa_2 = false;
    etapa_3 = false;
    etapa_4 = false;
    etapa_5 = false;
  }

  // etapas
  if (etapa_1) {
    if (boton_esta_on()){
      etapa_1 = false;
      etapa_2 = true;
    }
  }
  else if (etapa_2) {
    cinta_iniciar(); // comprobar en la funcion si ya esta encendida y no volver a encender
    etapa_2 = false;
    etapa_3 = true;
  }
  else if (etapa_3) {
    if (objeto_proximo()) {
      cinta_detener();
      etapa_3 = false;
      etapa_4 = true;
    }
    
  }
  else if (etapa_4) {
    retenedor_retener();
    etapa_4 = false;
    etapa_5 = true;
  }
  else if (etapa_5) {
    brazo_soldar();
    if (brazo_soldar()!=0){ Serial.println("Ocurrio un error."); }
    retenedor_liberar();
    etapa_5 = false;
    etapa_2 = true;
  }
  else {
    Serial.println("Error, ninguna etapa encendida.");
    etapa_1=true;
  }
  delay(10);
}


