#ifdef PANTALLA
#define PANTALLA

void patnalla_init();
void pantalla_setPorcentaje(int porcentaje, int startCaracter = 16);
void pantalla_limpiar();
void pantalla_print(char[] mens, int fila = 0, bool centrado = false);
void pantalla_print_en(char[] mens, int fila, int columna);

#endif