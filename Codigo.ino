#include <ESP32Servo.h>

// motores brazo
const int pin_motor_base = -1;
const int pin_motor_antebrazo = -1;
const int pin_motor_brazo = -1;
const int pin_motor_garra = -1;

// sensor de proximidad
const int pin_prox_echo = -1;
const int pin_prox_triger = -1;

const int pin_motor_retenedor = -1;
const int pin_temperatrua = -1;
const int pin_boton = -1;

bool etapa_1 = true; // etapa inicial
bool etapa_2 = false;
bool etapa_3 = false;
bool etapa_4 = false;
bool etapa_5 = false;


void setup() {
  Serial.begin(115200);
}

void loop() {

  // proceso paralelo a las etapas
  if (!etapa_1 && boton_esta_on()){
    cinta_detener();
    retenedor_liberar();
    brazo_reposo();
    
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
