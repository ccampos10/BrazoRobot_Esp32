/*
  -----------------------------
 |                             |
 |     Informacion general     |
 |                             |
  -----------------------------

// Descripcion:
// El precente codigo se encarga unicamente del control del brazo robotico, habilitando las siguientes funciones
// para realizar los movimientos y configurar el brazo
//   * brazo_init()
//       Funcion encargada de iniciar los componentes necesarios para el correcto funcionamiento del brazo
//   * brazo_reposo()
//       Esta funcion mueve el brazo a un estado de reposo, evitando la interferencia de este con los objetos que
//       pasan por la cinta transportadora
//   * brazo_soldar()
//       Esta funcion se encarga de hacer los movimientos necesarios para soldar la piesa metalica
//   * brazo_pinza_cerrar()
//       Esta funcion simplemente se usa para que el brazo sostenga el elemento soldador


//                     __________
//                     //      \\
//                   //    /\    \\
//   Brazo <──┐    //    // (  °  )
//            │  //    //   / / \ \
//             //    //   / /     \ \
//           //    //     | |     | | ──────> Garra
//         //    //        \ \ _ / /
//         (  °  )           \ | /
//         \\    \\           ¯ ¯
//           \\    \\
//             \\    \\
//               \\    \\ ──────> Antebrazo
//                 \\    \\
//                   \\    \\
//                    (  °  )        ┌─> Base
//                   //     \\       │
//                  ||       || ─────┘
//       =================================================

*/



// #include "brazo.h"
#include <ESP32Servo.h> // Libraria para el control de servomotores

//  -----------------------------
// |                             |
// |     Variables generales     |
// |                             |
//  -----------------------------

// Variables de correccion de los angulos fisicos
// Establece el angulo minimo de cada motor segun limitaciones fisicas
const int SERVO_BASE_CERO_CORRECCION = 0;
const int SERVO_ANTEBRAZO_CERO_CORRECCION = 20;
const int SERVO_BRAZO_CERO_CORRECCION = 140;
const int SERVO_GARRA_CERO_CORRECCION = 0;

// Establece el angulo maximo de cada motor segun limitaciones fisicas
const int SERVO_BASE_EXTENDIDO_CORRECCION = 180;
const int SERVO_ANTEBRAZO_EXTENDIDO_CORRECCION = 150;
const int SERVO_BRAZO_EXTENDIDO_CORRECCION = 55;
const int SERVO_GARRA_EXTENDIDO_CORRECCION = 0;

// Establece el angulo de la garra cuando esta cerrada
const int SERVO_GARRA_CERRADO = 30;

// Instancia de los servomotores
Servo servo_base;
Servo servo_antebrazo;
Servo servo_brazo;
Servo servo_garra;
static float antebrazo_largo = -1; // largo del antebrazo
static float brazo_largo = -1; // largo del brazo
static bool seteado = false; // Bandera que indica si el brazo ha sido iniciado





void brazo_pinza_cerrar() {
  // Cierra la pinza para sostener el soldador
  servo_garra.write(SERVO_GARRA_CERRADO);
}

void brazo_init(int pin_motor_base, int pin_motor_antebrazo, int pin_motor_brazo, int pin_motor_garra, float antebrazo_largo_E, float brazo_largo_E){
  // Funcion que configura los elementos necesarios para el correcto funcionamiento del brazo
  servo_base.attach(pin_motor_base,500,2500); // Establece el pin de la base
  servo_antebrazo.attach(pin_motor_antebrazo,500,2500); // Establece el pin del antebrazo
  servo_brazo.attach(pin_motor_brazo,500,2500); // Establece el pin del brazo
  servo_garra.attach(pin_motor_garra,500,2500); // Establece el pin de la garra
  brazo_pinza_cerrar();
  antebrazo_largo = (float)antebrazo_largo_E; // Establece el largo del antebrazo
  brazo_largo = (float)brazo_largo_E; // Establece el largo del brazo
  seteado = true; // Indica que el brazo ya a sido configurado correctamente
}

// void brazo_set_cero(){
//   servo_base.write(90);
//   servo_antebrazo.write(20);
//   servo_brazo.write(90);
//   servo_garra.write(0);
// }

// float roundDos(float numero) {
//   return round(numero * 10000) / 10000;
// }

void mover_base(int angulo) {
  // Mueve la base al angulo indicado
  servo_base.write(angulo);
}

void mover_antebrazo(int angulo){
  // Mueve el antebrazo al alngulo indicado
  // Se realiza una correccion del angulo del brazo, ya que por limitaciones fisicas
  // el angulo del antebrazo afecta al angulo del brazo

  angulo = map(angulo, 0, 180, SERVO_ANTEBRAZO_CERO_CORRECCION, SERVO_ANTEBRAZO_EXTENDIDO_CORRECCION);
  int angulo_var = servo_antebrazo.read() - angulo;
  // if (angulo_var<0) { angulo_var = angulo_var*-1; }
  int angulo2 = servo_brazo.read()-angulo_var;
  angulo2 = map(angulo2, 0, 180, SERVO_BRAZO_CERO_CORRECCION, SERVO_BRAZO_EXTENDIDO_CORRECCION);

  servo_antebrazo.write(angulo);
  // Serial.println(angulo);
  // Serial.println(angulo2);
  delay(50);
  servo_brazo.write(angulo2);
}

void mover_brazo(int angulo){
  int anguloActual;
  int anguloFinal = map(angulo, 0, 180, SERVO_BRAZO_CERO_CORRECCION, SERVO_BRAZO_EXTENDIDO_CORRECCION);
  for (anguloActual = servo_brazo.read(); anguloActual!=anguloFinal; anguloActual>anguloFinal? anguloActual-- : anguloActual++) {
    servo_brazo.write(anguloActual);
    delay(10);
  }
  // servo_brazo.write(map(angulo, 0, 180, SERVO_BRAZO_CERO_CORRECCION, SERVO_BRAZO_EXTENDIDO_CORRECCION));
}

int brazo_reposo(){
  // Deja el brazo en una posicion de reposo

  if (!seteado) { return -1; } // Devuelve el codigo de error -1 en el caso de que el brazo no a sido configurado
  mover_base(90);
  mover_brazo(90);
  mover_antebrazo(90);
  return 0;
}

int brazo_soldar(){
  // Realiza los movimientos prestablecidos para realizar la soldadura

  if (!seteado) { return -1; } // Devuelve el codigo de error -1 en el caso de que el brazo no a sido configurado
  Serial.println("sold");
  mover_antebrazo(30);
  delay(1000);
  mover_brazo(135);
  delay(1000);
  brazo_reposo();
  delay(1000);
  return 0;
}