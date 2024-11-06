/*
Codigo de cabecera para brazo.cpp
*/

#ifndef PANTALLA
#define PANTALLA

void pantalla_init();
void pantalla_setPorcentaje(int porcentaje, int startCaracter = 16);
void pantalla_limpiar();
void pantalla_print(char* mens, size_t mens_len, int fila = 0, bool centrado = false);
void pantalla_print_en(char* mens, int fila, int columna);

#endif