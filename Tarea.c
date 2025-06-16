#include <time.h>

struct Tarea {

  int id;

  char *nombre;

  char *mensaje;

  char *dia;
};

char *soloDia(int dia) {

  static char fecha[11];

  time_t t = time(NULL);

  struct tm tiempo = *localtime(&t); // <-

  tiempo.tm_mday = dia;

  mktime(&tiempo);

  strftime(fecha, sizeof(fecha), "%Y-%m-%d", &tiempo);

  return fecha;
}

char *mesYDia(int mes, int dia) {

  static char fecha[11];

  time_t t = time(NULL);

  struct tm tiempo = *localtime(&t); // <-

  tiempo.tm_mday = dia;

  tiempo.tm_mon = mes;

  mktime(&tiempo);

  strftime(fecha, sizeof(fecha), "%Y-%m-%d", &tiempo);

  return fecha;
}

char *AnnoMesYDia(int anno, int mes, int dia) {

  static char fecha[11];

  time_t t = time(NULL);

  struct tm tiempo = *localtime(&t); // <-

  tiempo.tm_mday = dia;

  tiempo.tm_mon = mes - 1;

  tiempo.tm_year = anno - 1900;

  mktime(&tiempo);

  strftime(fecha, sizeof(fecha), "%Y-%m-%d", &tiempo);

  return fecha;
}
