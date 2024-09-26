#include <stddef.h>
#include <stdint.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2); // direccion 0x27, 16 columnas, 2 filas

// caracteres de pocentaje
uint8_t barras[5][8] = {{0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10}, // 1/5
                      {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}, // 2/5
                      {0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c}, // 3/5
                      {0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e}, // 4/5
                      {0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f}}; // 5/5

// Iniciar la pantalla lcd
void patnalla_init(){
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, barras[0]);
  lcd.createChar(1, barras[1]);
  lcd.createChar(2, barras[2]);
  lcd.createChar(3, barras[3]);
  lcd.createChar(4, barras[4]);
}

// Mostrar una barra de porcentaje
void pantalla_setPorcentaje(int porcentaje, int caracteres) {
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

// Limpiar todos los elementos en pantalla
void pantalla_limpiar(){
  lcd.clear();
}


// Mostrar un mensaje por pantalla de maximo 16 carracteres.
void pantalla_print(char* mens, size_t mens_len, int fila, bool centrado) {
  // el mensaje no puede tener mas de 16 caracteres.
  // se puede poner mas de 16 caracteres, pero solo muestra 16.
  // y a partir de una cantidad de caracteres se bugea y los imprime en la fila de abajo
  if (mens_len > 16) {return;}
  if (centrado) { lcd.setCursor((16-mens_len)/2, fila); }
  else { lcd.setCursor(0, fila); }
  lcd.setCursor(0, fila);
  lcd.print(mens);
}

// Mostrar un mensaje por pantalla en una posicion espesifica
void pantalla_print_en(char* mens, int fila, int columna) {
  lcd.setCursor(columna, fila);
  lcd.print(mens);
}