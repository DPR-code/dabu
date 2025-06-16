#ifndef TAREA_DAO_H
#define TAREA_DAO_H

#include "Tarea.h"

void openDb(void);

int cuantasTareas(char *tipo);

struct Tarea *listTareas(char *tipo, int cantidad);

struct Tarea listUnaTarea(int id);

void liberarListarTareas(struct Tarea *tareas, int cantidad);

int insertTarea(struct Tarea tarea, char *tipo);

int updateTarea(int id, struct Tarea tarea, char *tipo);

int deleteTarea(int id);

#endif 
