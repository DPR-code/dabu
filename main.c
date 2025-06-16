#include "Tarea.h"
#include "TareaDAO.h"
#include "db.h"
#include <complex.h>
#include <ctype.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int saberComando(const char *cmd);
void limpiarBuffer();
void printTarea(struct Tarea tarea);

enum comandos {

  CMD_HELP,
  CMD_HELPES,
  CMD_UPDATE,
  CMD_ADD,
  CMD_ADDIN,
  CMD_EDIT,
  CMD_SHOW,
  CMD_DELETE,
  CMD_CHECK,
  CMD_D

};

#define ARRAYLENGHT(x) (sizeof(x) / sizeof(x[0]))

#define VERSION 0.0

#define PORHACER "PorHacer"
#define HECHAS "Hechas"
#define SINHACER "SinHacer"

int main(int argc, char *argv[]) {

  initDb();

  if (argc > 1) {

    int limite = argc < 3 ? argc : 3;

    for (int i = 1; i < limite; i++) {

      for (int j = 0; j < (int)strlen(argv[i]); j++) {

        argv[i][j] = toupper((unsigned char)argv[i][j]);
      }
    }
  }

  int cmd = -1;

  if (argc < 2) {

    cmd = -2;

  } else {

    cmd = saberComando(argv[1]);
  }

  switch (cmd) {

  case -1:

    printf("not valid comand\n");

    break;

  case -2:

    printf("dabu: Version %.2f\n", VERSION);
    printf("Type dabu Help for help\n");

    break;

  case CMD_HELP:

    printf("dabu: Daniel's Personal Task Manager\n");
    printf("------------------------------------\n");
    printf("dabu Help: help\n");
    printf("dabu HelpEs: help in spanish\n");
    printf("dabu Update: update database for today\n");
    printf("dabu Add: questions will be asked to add a new task\n");
    printf("dabu Add name YYYY MM DD: Add a task for the selected "
           "day of the selected month and the selected year\n");
    printf("dabu Add name MM DD: Add a task for the selected day "
           "of the selected month of this year\n");
    printf("dabu Add name DD: Add a task for the selected day of this month "
           "and year\n");
    printf("dabu AddIN i: add within i days\n");
    printf("dabu Edit i: edit the task with id i\n");

    printf("dabu Show tasks, success, fail: shows the corresponding table\n");
    printf("dabu Delete i: delete the task with id i\n");
    printf("dabu Check i: mark the task with id i as completed\n");

    break;

  case CMD_HELPES:

    printf("dabu: Gestor Personal de Tareas de Daniel\n");
    printf("------------------------------------------\n");
    printf("dabu Help: ayuda\n");
    printf("dabu HelpEs: ayuda en español\n");
    printf("dabu Update: actualizar la base de datos para hoy\n");
    printf("dabu Add: se harán preguntas para añadir una nueva tarea\n");
    printf("dabu Add name YYYY MM DD: añadir una tarea para el día, mes y año "
           "seleccionados\n");
    printf("dabu Add name MM DD: añadir una tarea para el día y mes "
           "seleccionados de este año\n");
    printf("dabu Add name DD: añadir una tarea para el día seleccionado de "
           "este mes y año\n");
    printf("dabu AddIN i: añadir dentro de i días\n");
    printf("dabu Edit i: editar la tarea con id i\n");

    printf(
        "dabu Show tasks, success, fail: muestra la tabla correspondiente\n");
    printf("dabu Delete i: eliminar la tarea con id i\n");
    printf("dabu Check i: marcar la tarea con id i como completada\n");

    break;

  case CMD_UPDATE:

    time_t hoy = time(NULL);

    int cuantas = cuantasTareas(PORHACER);

    struct Tarea *tareas = listTareas(PORHACER, cuantas);

    for (int i = 0; i < cuantas; i++) {

      int anno, mes, dia;

      sscanf(tareas[i].dia, "%d-%d-%d", &anno, &mes, &dia);

      struct tm fecha_tm = {0};
      fecha_tm.tm_year = anno - 1900;
      fecha_tm.tm_mon = mes - 1;
      fecha_tm.tm_mday = dia;
      fecha_tm.tm_hour = 0;
      fecha_tm.tm_min = 0;
      fecha_tm.tm_sec = 0;

      time_t fecha_tarea = mktime(&fecha_tm);

      double diferencia = difftime(hoy, fecha_tarea);

      if (diferencia > 0) {

        updateTarea(tareas[i].id, tareas[i], SINHACER);
      }
    }

    liberarListarTareas(tareas, cuantas);

    break;

  case CMD_ADD:

    struct Tarea tarea;
    char nombre[25];
    char descripcion[200];
    char fecha[11];
    int anno = 0, mes = 0, dia = 0;
    char *endptr;

    if (argc < 3) {
      printf("Task name:\n");
      fgets(nombre, sizeof(nombre), stdin);
      tarea.nombre = nombre;
    } else {
      tarea.nombre = argv[1];
    }

    // Leer año
    if (argc >= 4) {
      anno = (int)strtol(argv[3], &endptr, 10);
      if (*endptr != '\0' || anno < 1971 || anno > 2999) {
        printf("year no valid\n");
        break;
      }
    } else {
      printf("Task year:\n");
      scanf("%d", &anno);
      limpiarBuffer();
      if (anno < 1971 || anno > 2999) {
        printf("year no valid\n");
        break;
      }
    }

    // Leer mes
    if (argc >= 5) {
      mes = (int)strtol(argv[4], &endptr, 10);
      if (*endptr != '\0' || mes < 1 || mes > 12) {
        printf("month no valid\n");
        break;
      }
    } else {
      printf("Task month:\n");
      scanf("%d", &mes);
      limpiarBuffer();
      if (mes < 1 || mes > 12) {
        printf("month no valid\n");
        break;
      }
    }

    // Leer día
    if (argc >= 6) {
      dia = (int)strtol(argv[5], &endptr, 10);
      if (*endptr != '\0' || dia < 1 || dia > 31) {
        printf("day no valid\n");
        break;
      }
    } else {
      printf("Task day:\n");
      scanf("%d", &dia);
      limpiarBuffer();
      if (dia < 1 || dia > 31) {
        printf("day no valid\n");
        break;
      }
    }

    // Leer descripción
    printf("Task description:\n");
    fgets(descripcion, sizeof(descripcion), stdin);

    // Formatear fecha
    snprintf(fecha, sizeof(fecha), "%04d-%02d-%02d", anno, mes, dia);

    // Llenar estructura
    tarea.mensaje = descripcion;
    tarea.dia = fecha;

    insertTarea(tarea, PORHACER);

    break;

  case CMD_ADDIN: {

    time_t t = time(NULL);
    struct tm fecha_tm = *localtime(&t);

    char *endptr;
    int dia = (int)strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || dia < 1 || dia > 31) {
      printf("vaule no valid\n");
      break;
    }

    fecha_tm.tm_mday = fecha_tm.tm_mday + dia;
    fecha_tm.tm_year = fecha_tm.tm_year + 1900;

    mktime(&fecha_tm);

    char fecha[11];
    snprintf(fecha, sizeof(fecha), "%04d-%02d-%02d", fecha_tm.tm_year,
             fecha_tm.tm_mon, fecha_tm.tm_mday);

    char nombre[25];
    char descripcion[200];

    printf("Task name:\n");
    fgets(nombre, sizeof(nombre), stdin);

    printf("Task description:\n");
    fgets(descripcion, sizeof(descripcion), stdin);

    struct Tarea tarea;

    tarea.nombre = nombre;
    tarea.mensaje = descripcion;
    tarea.dia = fecha;

    insertTarea(tarea, PORHACER);

    break;
  }

  case CMD_EDIT: {

    int id;

    if (argc >= 3) {

      char *endptr;
      id = (int)strtol(argv[2], &endptr, 10);
      if (*endptr != '\0') {
        printf("id no valid\n");
        break;
      }

    } else {

      printf("Insert ID\n");
      scanf("%d", &id);
      limpiarBuffer();
    }

    char nombre[25];
    printf("New task name:\n");
    fgets(nombre, sizeof(nombre), stdin);

    char descripcion[200];
    printf("New Task description:\n");
    fgets(descripcion, sizeof(descripcion), stdin);

    int anno, mes, dia;

    printf("Task year:\n");
    scanf("%d", &anno);
    limpiarBuffer();

    if (anno < 1971 || anno > 2999) {
      printf("year no valid\n");
      break;
    }

    printf("Task month:\n");
    scanf("%d", &mes);
    limpiarBuffer();

    if (mes < 1 || mes > 12) {
      printf("month no valid\n");
      break;
    }

    printf("Task day:\n");
    scanf("%d", &dia);
    limpiarBuffer();

    if (dia < 1 || dia > 31) {
      printf("day no valid\n");
      break;
    }

    char *tipo = NULL;
    int tipoId;
    printf("Insert type:\n1: tasks\n2: success\n3: fail\n");
    scanf("%d", &tipoId);
    limpiarBuffer();

    switch (tipoId) {
    case 1:
      tipo = PORHACER;
      break;
    case 2:
      tipo = HECHAS;
      break;
    case 3:
      tipo = SINHACER;
      break;
    default:
      printf("Invalid type\n");
      break;
    }

    if (!tipo)
      break;

    char fecha[11];
    snprintf(fecha, sizeof(fecha), "%04d-%02d-%02d", anno, mes, dia);

    struct Tarea tarea;
    tarea.nombre = nombre;
    tarea.mensaje = descripcion;
    tarea.dia = fecha;

    int resultado = updateTarea(id, tarea, tipo);

    if (resultado == -2) {

      printf("no task updated");
    }

    break;
  }
  case CMD_SHOW: {

    char *tabla = NULL;

    if (argc == 3) {

      for (int i = 0; i < (int)strlen(argv[2]); i++) {
        argv[2][i] = toupper((unsigned char)argv[2][i]);
      }

      if (strcmp(argv[2], "TASKS") == 0) {
        tabla = PORHACER;
      }

      if (strcmp(argv[2], "SUCCESS") == 0) {
        tabla = HECHAS;
      }

      if (strcmp(argv[2], "FAIL") == 0) {
        tabla = SINHACER;
      }
    } else {

      tabla = PORHACER;
    }

    int cantidad = cuantasTareas(tabla);

    struct Tarea *tareas = listTareas(tabla, cantidad);

    for (int i = 0; i < cantidad; i++) {

      printTarea(tareas[i]);
    }

    liberarListarTareas(tareas, cantidad);

    break;
  }

  case CMD_DELETE: {

    int id;

    if (argc >= 3) {

      char *endptr;
      id = (int)strtol(argv[2], &endptr, 10);
      if (*endptr != '\0') {
        printf("id no valid\n");
        break;
      }

    } else {

      printf("Insert ID\n");
      scanf("%d", &id);
      limpiarBuffer();
    }

    deleteTarea(id);

    break;
  }

  case CMD_CHECK: {

    int id;

    if (argc >= 3) {

      char *endptr;
      id = (int)strtol(argv[2], &endptr, 10);
      if (*endptr != '\0') {
        printf("id no valid\n");
        break;
      }

    } else {

      printf("Insert ID\n");
      scanf("%d", &id);
      limpiarBuffer();
    }

    struct Tarea tarea = listUnaTarea(id);

    updateTarea(id, tarea, HECHAS);

    break;
  }

  case CMD_D:

    printf("⠠⠵\n");

    break;
  }

  return 0;
}

int saberComando(const char *cmd) {
  if (strcmp(cmd, "HELP") == 0)
    return CMD_HELP;
  if (strcmp(cmd, "HELPES") == 0)
    return CMD_HELPES;
  if (strcmp(cmd, "UPDATE") == 0)
    return CMD_UPDATE;
  if (strcmp(cmd, "ADD") == 0)
    return CMD_ADD;
  if (strcmp(cmd, "ADDIN") == 0)
    return CMD_ADDIN;
  if (strcmp(cmd, "EDIT") == 0)
    return CMD_EDIT;
  if (strcmp(cmd, "SHOW") == 0)
    return CMD_SHOW;
  if (strcmp(cmd, "DELETE") == 0)
    return CMD_DELETE;
  if (strcmp(cmd, "CHECK") == 0)
    return CMD_CHECK;
  if (strcmp(cmd, "D") == 0)
    return CMD_D;
  return -1;
}

void limpiarBuffer() {
  while (getchar() != '\n')
    ;
}

void printTarea(struct Tarea tarea) {

  printf("id: %d - %s\n %s\n", tarea.id, tarea.nombre, tarea.mensaje);
}
