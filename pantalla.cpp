/*
  -----------------------------
 |                             |
 |     Informacion general     |
 |                             |
  -----------------------------

// Descripcion:
// El precente codigo se encarga de configurar y controlar la pantalla lcd, entregando las siguientes funciones
// para poder interactuar con la pantalla:
//    * patnalla_init()
//        Funcion encargada de la configuracion inicial de la pantalla
//    * pantalla_setPorcentaje()
//        Esta funcion muestra una barra de progreso en la pantalla
//    * pantalla_limpiar()
//        Esta funcion elimina todos los elementos precentes actualmente en la pantalla, letras, simbolos, etc.
//    * pantalla_print()
//        Esta funcion muestra un mensaje en la pantalla
//    * pantalla_print_en(char* mens, int fila, int columna);
//        Esta funcion muestra un mensaje el la pantalla en una posicion determinada
*/


// #include <stddef.h>
// #include <stdint.h>
#include <LiquidCrystal_I2C.h> // Libraria para el control de la pantalla LCD

// Instancia de la pantalla lcd
static LiquidCrystal_I2C lcd(0x27, 16, 2); // direccion de esclabo 0x27, 16 columnas, 2 filas

// Caracteres personalizados de pocentaje
uint8_t barras[5][8] = {{0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10}, // 1/5
//                      █░░░░
                      {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}, // 2/5
//                      ██░░░
                      {0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c}, // 3/5
//                      ███░░
                      {0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e}, // 4/5
//                      ████░
                      {0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f}}; // 5/5
//                      █████

void pantalla_init(){
  // Configuracion inicial de la pantalla lcd
  lcd.init();
  lcd.backlight();

  // Crea los caracteres personalizados
  lcd.createChar(0, barras[0]);
  lcd.createChar(1, barras[1]);
  lcd.createChar(2, barras[2]);
  lcd.createChar(3, barras[3]);
  lcd.createChar(4, barras[4]);
}

void pantalla_setPorcentaje(int porcentaje, int caracteres) {
  // Mostrar una barra de porcentaje en la pantalla lcd
  float porcentaje_columna = 100.0/((float)caracteres*5.0);
  int col_on = porcentaje/porcentaje_columna;
  lcd.setCursor(16-caracteres, 1);

  for (int i = 0; i<col_on/5; i++) {
    lcd.write(4);
  }
  if (col_on%5 != 0){
    lcd.write(col_on%5-1);
  }
}

void pantalla_limpiar(){
  // Limpiar todos los elementos en pantalla
  lcd.clear();
}


void pantalla_print(char* mens, size_t mens_len, int fila, bool centrado) {
  // Mostrar un mensaje por pantalla de maximo 16 carracteres.
  
  if (mens_len > 16) {return;}
  if (centrado) { lcd.setCursor((16-mens_len)/2, fila); }
  else { lcd.setCursor(0, fila); }
  lcd.setCursor(0, fila);
  lcd.print(mens);
}

void pantalla_print_en(char* mens, int fila, int columna) {
  // Mostrar un mensaje por pantalla en una posicion espesifica
  lcd.setCursor(columna, fila);
  lcd.print(mens);
}