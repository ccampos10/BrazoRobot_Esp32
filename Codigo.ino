/*
 *****************************************************************************************************************
| ██████╗ ██████╗  █████╗ ███████╗ ██████╗     ███████╗ ██████╗ ██╗     ██████╗  █████╗ ██████╗  ██████╗ ██████╗  |
| ██╔══██╗██╔══██╗██╔══██╗╚══███╔╝██╔═══██╗    ██╔════╝██╔═══██╗██║     ██╔══██╗██╔══██╗██╔══██╗██╔═══██╗██╔══██╗ |
| ██████╔╝██████╔╝███████║  ███╔╝ ██║   ██║    ███████╗██║   ██║██║     ██║  ██║███████║██║  ██║██║   ██║██████╔╝ |
| ██╔══██╗██╔══██╗██╔══██║ ███╔╝  ██║   ██║    ╚════██║██║   ██║██║     ██║  ██║██╔══██║██║  ██║██║   ██║██╔══██╗ |
| ██████╔╝██║  ██║██║  ██║███████╗╚██████╔╝    ███████║╚██████╔╝███████╗██████╔╝██║  ██║██████╔╝╚██████╔╝██║  ██║ |
| ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝     ╚══════╝ ╚═════╝ ╚══════╝╚═════╝ ╚═╝  ╚═╝╚═════╝  ╚═════╝ ╚═╝  ╚═╝ |
 *****************************************************************************************************************

  -----------------------------
 |                             |
 |     Informacion general     |
 |                             |
  -----------------------------

//  Proyecto:     Control de manipuladores roboticos de 4 GDL
//  Integrantes:  Angel Barraza
//                David Ramirez
//                Guillermo Campos
//                Jazmin Ruz
//                Martin Ferreira
//                Tamariki Ruiz
//                Vicente Ramirez

//  Asignarua:    Fundamentos de IoT
//  Seccion:      02
//  Profesores:   Tulio Leon
//                Vicente Salinas

// Descripcion:
// El precente codigo cumple el proposito de controlar un brazo robotico de 4 grados de libertad (4 GDL), con el proposito
// de automatizar el proceso de soldadura de piezas metalicas en una linea de produccion. El brazo se posiciona en cordenadas
// predefinidas para realizar la soldadura. Ademas se utiliza la plataforma arduino cloud para monitorear el proceso de soldadura
// 
// El proceso de soldadura del brazo se divide en 5 etapas:
//    * Etapa 1: Inicio
//        En esta etapa se espera a que se inicie el proceso de soldadura.
//    * Etapa 2: Cinta transportadora.
//        En esta etapa se inicia el movimiento de la cinta transportadora que trae los objetos, de
//        otras partes de la linea de produccion, hacia el brazo soldador.
//    * Etapa 3: Deteccion
//        Esta es la etapa en donde se detecta el paso de un objeto por la cinta transportadora, deteniendola
//        para iniciar el proceso de soldadura.
//    * Etapa 4: Retencion
//        En esta etapa se retiene el objeto, para que este no se mueva durante la soldadura, garantizando
//        una soldadura optima y de calidad.
//    * Etapa 5: Soldadura
//        Esta es la etapa en donde se realiza la soldadura del objeto, realizando las rotaciones
//        de cada motor de forma individual para poder realizar el movimiento requerido. Se registra
//        la temperatura de la soldadura, se cuenta un objeto mas como soldado, se contabiliza un ciclo mas
//        como terminado, y se envian los datos a la nube





  -----------------------------
 |                             |
 |   Diagrama de conecciones   |
 |                             |
  -----------------------------

//                                        |==================|
//                                        |                  | --> Pin 23: Motor retenedor
//                                        |                  | --> Pin 22: Coneccion I2C, reloj
//                                        |                  |
//                                        |                  |
//                                        |                  | --> Pin 21: Coneccion I2C, datos
//          Pin 32: IN4 Motor a pasos <-- |                  | --> Pin 19: IN3 Motor a pasos
//  Pin 33: Motor antebrazo del brazo <-- |     ---------    | --> Pin 18: IN2 Motor a pasos
//      Pin 25: Motor brazo del brazo <-- |    |  Esp32  |   | --> Pin 5: IN1 Motor a pasos
//      Pin 26: Motor garra del brazo <-- |     ---------    | --> Pin 17: Motor base del brazo
//                                        |                  | --> Pin 16: Sensor de temparatura analoga
//                                        |                  | --> Pin 4: Boton de encendido
//        Pin 12: Echo de ultrasonido <-- |                  |
//      Pin 13: Triger de ultrasonido <-- |                  |
//                             Tierra <-- |                  | --> Tierra
//                             5 Volt <-- |                  | --> 3.3 Volt
//                                        |==================|



*/

/* 
  -----------------------------
 |                             |
 | variables de arduino cloud  |
 |                             |
  -----------------------------

  float temperatura; Regristra la temperatrua actual de la soldadura
  int ciclos; // Registra el numero de ciclos del proceso
  int etapa; // Registra la etapa actual del proceso
  int productos_Soldados; // Contabiliza los productos soldados
  bool estaEncendido; // Determina si el proceso esta iniciado o no
*/

// Libreria de arduino cloud
#include "thingProperties.h" // Libreria de arduino cloud

#include "brazo.h" // Codigo propio
#include "pantalla.h" // Codigo propio
#include <ESP32Servo.h> // Libraria para el control de servomotores
#include <Stepper.h> // Libreria para el control de motores a pasos

//  -----------------------------
// |                             |
// |     Variables generales     |
// |                             |
//  -----------------------------

// Valore predeterminados
const int DISTANCIA_DETECCION = 6; // distancia en la que se detecta el paso del objeto
const int stepsPerRevolution = 2048; //
const int ANGULO_RETENCION = 1; // angulo del retenedor
const int ANGULO_LIBERACION = 1; // angulo del retenedor
const float TEMPERATURA_LIMITE = 25;

// motores brazo
const int pin_motor_base = 16; // 17
const int pin_motor_antebrazo = 33;
const int pin_motor_brazo = 25;
const int pin_motor_garra = 26;

// sensor de proximidad
const int pin_prox_echo = 12;
const int pin_prox_triger = 13;

// pines del motor a pasos
const int IN1 = 5;
const int IN2 = 18;
const int IN3 = 19;
const int IN4 = 32;

// coneccion serial (pines por defecto)
// pin 21 sda
// pin 22 scl

const int pin_motor_retenedor = 23;
const int pin_temperatrua = 17; // 16
const int pin_boton = 4;

// Variable de las etapas
bool etapa_1 = true; // etapa inicial
bool etapa_2 = false;
bool etapa_3 = false;
bool etapa_4 = false;
bool etapa_5 = false;

// Banderas
bool mostradoEnPantalla = false;
bool cintaEstaIniciada = false;
bool objetoYaSoldado = false;

// Instancias
Servo retenedor; // instancia del servomotor usado como retenedor
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); // instancia del motor a pasos usado en la cinta




//  -----------------------------
// |                             |
// |     Funciones generales     |
// |                             |
//  -----------------------------

bool boton_esta_on(){
  // Comprobar el estado del boton
  return digitalRead(pin_boton) == 1;
}

void cinta_loop() {
  // Mueve la cinta siempre que esta este encendida
  if (cintaEstaIniciada) {
    myStepper.step(1);
  }
}

void retenedor_liberar(){
  // Abre el retenedor
  retenedor.write(ANGULO_LIBERACION);
}
void retenedor_retener(){
  // Cierra el retenedor
  retenedor.write(ANGULO_RETENCION);
}

bool objeto_proximo(){
  // Comprueba si hay algun objeto proximo

  // Emision del ultrasonido
  digitalWrite(pin_prox_triger, LOW);
  delayMicroseconds(2);
  digitalWrite(pin_prox_triger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_prox_triger, LOW);
  
  // Calculo de la distancia
  double duracion = pulseIn(pin_prox_echo, HIGH);
  double distancia = duracion * 0.0343 / 2.0;

  return distancia <= DISTANCIA_DETECCION;
}

float medir_temperatura() {
  // Funcion encargada de medir la temperatura del porta electrodo del soldador

  float readVal = analogRead(pin_temperatrua); // mide el nivel
  float volt = ((float)readVal * 3.3 / 4095.0)*1000.0; // multiplica por el voltaje por nivel. por 1000 para transformar a mili voltios
  float temp = (float)map(volt,500,510,0,100)/100.0; // transformar el voltaje a temp. multiplico por 100 para considerar los 2 primeros decimales
  Serial.println(readVal);
  Serial.println(volt);
  Serial.println(temp);
  return temp;
}





void setup() {
  Serial.begin(115200);
  
  // ------------------------------
  // Configuracion de arduino cloud
  // ------------------------------

  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();



  
  // ------------------------------
  //   Configuraciones generales
  // ------------------------------

  myStepper.setSpeed(4); // 
  brazo_init(pin_motor_base, pin_motor_antebrazo, pin_motor_brazo, pin_motor_garra, 7.7, 7.7); // Iniciar la configuracion del brazo
  pantalla_init(); // iniciar la comunicacion con la pantalla

  pinMode(pin_boton, INPUT_PULLDOWN); // Configurar el boton de inicio
  pinMode(pin_temperatrua, INPUT); // Configurar el pin de temperatura

  // Configurar los pines del ultrasonido
  pinMode(pin_prox_triger, OUTPUT);
  pinMode(pin_prox_echo, INPUT);

  // Configurar el retenedor
  retenedor.attach(pin_motor_retenedor, 500, 2500);
  temperatura = 18.337;
}

void loop() {
  ArduinoCloud.update(); // Actualizacion del arduino cloud

  cinta_loop(); // Ciclo de la cinta
  
  if (etapa_1) {
    /*
      ---------------------------------------
     |                                       |
     |         Primera etapa: Inicio         |
     |                                       |
      ---------------------------------------

      Descripcion:
      Esta es la etapa de espera antes de que se inicie el ciclo de produccion
    */

    etapa=1; // Establece la etapa actual para arduino cloud
    if (!mostradoEnPantalla) {
      // Muestra en la pantalla LCD la etapa una unica ves
      pantalla_print("Etapa 1",8);
      mostradoEnPantalla=true;
    }
    brazo_reposo();
    if (boton_esta_on() || estaEncendido){
      // si el boton a sido precionado, o se inicio el proceso a traves de arduino cloud
      // se inicia el proceso
      etapa_1 = false; // cambio de etapas
      etapa_2 = true;
      mostradoEnPantalla=false;
    }
  }
  else if (etapa_2) {
    /*
      ---------------------------------------
     |                                       |
     |   Segunda etapa: Inicio de la cinta   |
     |                                       |
      ---------------------------------------
      
      Descripcion:
      En esta etapa se inicia el movimiento de la cinta transportadora
    */

    etapa=2; // Establece la etapa actual para arduino cloud
    if (!mostradoEnPantalla) {
      // Muestra en la pantalla LCD la etapa una unica ves
      pantalla_print("Etapa 2",8);
      mostradoEnPantalla=true;
    }
    cintaEstaIniciada = true;
    etapa_2 = false; // cambio de etapas
    etapa_3 = true;
    mostradoEnPantalla=false;
  }
  else if (etapa_3) {
    /*
      ---------------------------------------
     |                                       |
     |  Tercera etapa: Deteccion de objetos  |
     |                                       |
      ---------------------------------------
      
      Descripcion:
      En esta etapa se detecta el paso de un objeto por la cinta transportadora para realizar la soldadura
    */

    etapa=3; // Establece la etapa actual para arduino cloud
    if (!mostradoEnPantalla) {
      // Muestra en la pantalla LCD la etapa una unica ves
      pantalla_print("Etapa 3",8);
      mostradoEnPantalla=true;
    }
    if (objeto_proximo() && !objetoYaSoldado) {
      // Comprueba si paso un objeto por la cinta transportadora
      cintaEstaIniciada = false; // Apaga la cinta de transporte
      etapa_3 = false; // cambio de etapas
      etapa_4 = true;
      mostradoEnPantalla=false;
      Serial.println(objetoYaSoldado);
    }
    //else if (!objeto_proximo() && objetoYaSoldado) {
    //  objetoYaSoldado=false;
    //  Serial.println(objetoYaSoldado);
    //}
  }
  else if (etapa_4) {
    /*
      ---------------------------------------
     |                                       |
     |        Cuarta etapa: Retencion        |
     |                                       |
      ---------------------------------------
      
      Descripcion:
      En esta etapa se retiene el objeto a soldar, para que este no se mueva de su posicion
    */

    etapa=4; // Establece la etapa actual para arduino cloud
    if (!mostradoEnPantalla) {
      // Muestra en la pantalla LCD la etapa una unica ves
      pantalla_print("Etapa 3",8);
      mostradoEnPantalla=true;
    }
    retenedor_retener(); // Retencion del objeto
    delay(100);
    etapa_4 = false; // cambio de etapas
    etapa_5 = true;
    mostradoEnPantalla=false;
  }
  else if (etapa_5) {
    /*
      ---------------------------------------
     |                                       |
     |        Quinta etapa: Soldadura        |
     |                                       |
      ---------------------------------------

      Descripcion:
      En esta etapa se raliza la soldadura del objeto, para luego volver a la etapa 2 y continuar con el ciclo
    */

    etapa=5; // Establece la etapa actual para arduino cloud
    if (!mostradoEnPantalla) {
      // Muestra en la pantalla LCD la etapa una unica ves
      pantalla_print("Etapa 5",8);
      mostradoEnPantalla=true;
    }
    if (medir_temperatura() >= TEMPERATURA_LIMITE){
      delay(10);
      return;
    }
    if (brazo_soldar()!=0){ Serial.println("Ocurrio un error."); } // Realiza la soldadura
    temperatura = medir_temperatura();
    
    retenedor_liberar(); // Libera el objeto
    productos_Soldados++; // Registra el objeto como soldado para arduino cloud
    delay(100);
    etapa_5 = false; // Cambio de etapas
    etapa_2 = true; // Reinicio a la etapa 2
    mostradoEnPantalla=false;
    ciclos++; // Registra los ciclos completados para arduino cloud
    //objetoYaSoldado = true;
  }
  delay(10);
}

/*
  Since EstaEncendido is READ_WRITE variable, onEstaEncendidoChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onEstaEncendidoChange()  {
  // Add your code here to act upon EstaEncendido change
}


