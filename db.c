#include <sqlite3.h>
#include <stdio.h>

int initDb() {

  sqlite3 *db;
  char *errmsg = NULL;
  int rc;

  // Abrir o crear la base de datos
  rc = sqlite3_open("DB.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Error al abrir o crear la base de datos: %s\n",
            sqlite3_errmsg(db));
    sqlite3_close(db);
    return rc;
  }

  // SQL para crear las tablas
  const char *sql = "CREATE TABLE IF NOT EXISTS Tareas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "nombre TEXT NOT NULL,"
                    "descripcion TEXT,"
                    "dia DATE NOT NULL,"
                    " estado TEXT NOT NULL CHECK (estado IN ('PorHacer', "
                    "'Hechas', 'SinHacer')));";

  // Ejecutar el SQL
  rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Error al crear tablas: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    return rc;
  }

  sqlite3_close(db);
  return SQLITE_OK;
}