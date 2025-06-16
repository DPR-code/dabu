# dabu
Dabu es un administrador de tareas de terminal básico y pequeño creado como prueba.

## Archivos principales

- `main.c`: donde reside algunas funciones realcionadas con comandos y la funcion main.
- `Tarea.c/h`: define la estructura de una tarea.
- `TareaDAO.c/h`: funciones para interactuar con la base de datos.
- `db.c/h`: inicialización y conexión a SQLite.

## Compilación

```bash
gcc main.c Tarea.c TareaDAO.c db.c -o dabu -lsqlite3
