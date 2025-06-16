#ifndef TAREA_H
#define TAREA_H

#include <time.h>

// Definición de la estructura Tarea
struct Tarea {
  int id;
  char *nombre;
  char *mensaje;
  char *dia;
};

// Declaraciones de funciones para inicializar fechas
char *soloDia(int dia);
char *mesYDia(int mes, int dia);
char *AnnoMesYDia(int anno, int mes, int dia);

#endif // TAREA_H
