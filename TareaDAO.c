#include "Tarea.h"
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

sqlite3 *db = NULL;

void openDb() {
  if (db == NULL) {
    if (sqlite3_open("DB.db", &db) != SQLITE_OK) {
      fprintf(stderr, "Error al abrir la base de datos: %s\n",
              sqlite3_errmsg(db));
      exit(1);
    }
  }
}

int cuantasTareas(char *tipo) {

  openDb();

  const char *sql = "SELECT COUNT(*) FROM Tareas WHERE estado = ?";

  sqlite3_stmt *stmt;
  int cantidad = 0;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error en prepare: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(stmt, 1, tipo, -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    cantidad = sqlite3_column_int(stmt, 0);
  } else {
    fprintf(stderr, "Error al ejecutar COUNT: %s\n", sqlite3_errmsg(db));
  }

  sqlite3_finalize(stmt);
  return cantidad;
}

struct Tarea *listTareas(char *tipo, int cantidad) {
  openDb();

  char *sql = "SELECT * FROM Tareas WHERE estado = ? ORDER BY dia";

  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error en prepare: %s\n", sqlite3_errmsg(db));
    return NULL;
  }

  sqlite3_bind_text(stmt, 1, tipo, -1, SQLITE_STATIC);

  struct Tarea *tareas = malloc(sizeof(struct Tarea) * (cantidad));

  int i = 0;

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    tareas[i].id = sqlite3_column_int(stmt, 0);

    const char *nombre = (const char *)sqlite3_column_text(stmt, 1);
    tareas[i].nombre = strdup(nombre ? nombre : "");

    const char *mensaje = (const char *)sqlite3_column_text(stmt, 2);
    tareas[i].mensaje = strdup(mensaje ? mensaje : "");

    const char *dia = (const char *)sqlite3_column_text(stmt, 3);
    tareas[i].dia = strdup(dia ? dia : "");

    i++;
  }

  sqlite3_finalize(stmt);
  return tareas;
}

struct Tarea listUnaTarea(int id) {
  openDb();  // Asegurarse de abrir la DB si no está abierta

  const char *sql = "SELECT * FROM Tareas WHERE id = ?";

  sqlite3_stmt *stmt;

  struct Tarea tarea = {
      .id = -1,
      .nombre = NULL,
      .mensaje = NULL,
      .dia = NULL
  };

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error en prepare: %s\n", sqlite3_errmsg(db));
    return tarea;  
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    tarea.id = sqlite3_column_int(stmt, 0);

    const char *nombre = (const char *)sqlite3_column_text(stmt, 1);
    tarea.nombre = strdup(nombre ? nombre : "");

    const char *mensaje = (const char *)sqlite3_column_text(stmt, 2);
    tarea.mensaje = strdup(mensaje ? mensaje : "");

    const char *dia = (const char *)sqlite3_column_text(stmt, 3);
    tarea.dia = strdup(dia ? dia : "");
  }

  sqlite3_finalize(stmt);
  return tarea;
}


void liberarListarTareas(struct Tarea *tareas, int cantidad) {
  if (tareas == NULL)
    return;

  for (int i = 0; i < cantidad; i++) {
    free(tareas[i].nombre);
    free(tareas[i].mensaje);
    free(tareas[i].dia);
  }

  free(tareas);
}

int insertTarea(struct Tarea tarea, char *tipo) {

  openDb();

  char *sql = "INSERT INTO Tareas (nombre, descripcion, dia, estado) VALUES "
              "(?, ?, ?, ?)";

  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error en prepare: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(stmt, 1, tarea.nombre, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, tarea.mensaje, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, tarea.dia, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, tipo, -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Error al insertar tarea: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return -1;
  }

  sqlite3_finalize(stmt);
  return 0;
}

int updateTarea(int id, struct Tarea tarea, char *tipo) {

  openDb();

  char *sql = "UPDATE Tareas SET nombre = ?, descripcion = ?, dia = ?, estado "
              "= ? WHERE id = ?";

  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error en prepare: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(stmt, 1, tarea.nombre, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, tarea.mensaje, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, tarea.dia, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, tipo, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 5, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Error al insertar tarea: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return -1;
  }

  int changes = sqlite3_changes(db);

  sqlite3_finalize(stmt);

  if (changes == 0) {
  fprintf(stderr, "No se encontró ninguna tarea con id %d.\n", id);
  return -2;  
}
  return 0;
}

int deleteTarea(int id) {

  openDb();

  char *sql = "DELETE FROM Tareas WHERE id = ?;";

  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error en prepare: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Error al insertar tarea: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return -1;
  }

  sqlite3_finalize(stmt);
  return 0;
}