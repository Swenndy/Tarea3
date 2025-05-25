#include <stdio.h>
#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxescenarios 17

typedef struct
{
  char *nombre;
  int valor;
  int peso;
} item;

typedef struct
{
  int arriba;
  int abajo;
  int izquierda;
  int derecha;
} direcciones;

typedef struct
{
  char *nombre;
  int id;
  char *descripcion;
  List *items;
  direcciones enlaces;
  int es_final;
} escenario;

typedef struct
{
  float tiempo_restante;
  int puntajeTotal;
  int pesoTotal;
  List *items;
  escenario *escenario_actual;
} player;

escenario *totalEscenarios[maxescenarios];

item *crearItem(char *nombre, int valor, int peso)
{
  item *nuevo_item = malloc(sizeof(item));
  nuevo_item->nombre = strdup(nombre);
  nuevo_item->valor = valor;
  nuevo_item->peso = peso;
  return nuevo_item;
}
void leer_escenarios()
{

  FILE *archivo = fopen("graphquest.csv", "r");
  if (archivo == NULL)
  {
    perror(
        "Error al abrir el archivo");
    return;
  }

  char **campos;
  fgets((char[256]){0}, 256, archivo);

  while ((campos = leer_linea_csv(archivo, ',')) != NULL)
  {
    int id = atoi(campos[0]);
    escenario *nuevo_escenario = malloc(sizeof(escenario));
    nuevo_escenario->id = id;
    nuevo_escenario->nombre = strdup(campos[1]);
    nuevo_escenario->descripcion = strdup(campos[2]);
    nuevo_escenario->items = list_create();
    nuevo_escenario->es_final = (strcmp(campos[8], "Si") == 0) ? 1 : 0;

    List *items = split_string(campos[3], ";");
    for (char *item_str = list_first(items); item_str != NULL; item_str = list_next(items))
    {
      List *valores = split_string(item_str, ",");
      char *nombre_item = strdup(list_first(valores));
      int valor = atoi(list_next(valores));
      int peso = atoi(list_next(valores));
      item *nuevo_item = crearItem(nombre_item, valor, peso);
      list_pushBack(nuevo_escenario->items, nuevo_item);
      list_clean(valores);
      free(valores);
    }
    list_clean(items);
    free(items);
    nuevo_escenario->enlaces.arriba = atoi(campos[4]);
    nuevo_escenario->enlaces.abajo = atoi(campos[5]);
    nuevo_escenario->enlaces.izquierda = atoi(campos[6]);
    nuevo_escenario->enlaces.derecha = atoi(campos[7]);
    totalEscenarios[id] = nuevo_escenario;
  }
  fclose(archivo);
  printf("Escenarios leidos correctamente.\n");
  limpiarPantalla();
  printf("Bienvenido a GraphQuest!\n");
  printf("Tu mision es recoger items y llegar al escenario final.\n");
  printf("Recuerda que el tiempo es limitado y debes gestionar tu inventario.\n");
  printf("Buena suerte!\n");
}
void recogerItems(player *jugador)
{
  printf("Items disponibles en el escenario '%s':\n", jugador->escenario_actual->nombre);
  List *items = jugador->escenario_actual->items;

  // Verifica si la lista está vacía
  if (list_first(items) == NULL)
  {
    printf("No hay items en esta sala.\n");
    return;
  }

  for (item *i = list_first(items); i != NULL; i = list_next(items))
  {
    printf("Nombre: %s, Valor: %d, Peso: %d\n", i->nombre, i->valor, i->peso);
  }

  printf("Seleccione el nombre del item a recoger: ");
  char nombre_item[50];
  // Limpia el buffer de entrada antes de fgets
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
  fgets(nombre_item, sizeof(nombre_item), stdin);
  // Elimina el salto de línea al final, si existe
  nombre_item[strcspn(nombre_item, "\n")] = 0;

  for (item *i = list_first(items); i != NULL; i = list_next(items))
  {
    if (strcmp(i->nombre, nombre_item) == 0)
    {
      list_pushBack(jugador->items, i);
      jugador->puntajeTotal += i->valor;
      jugador->pesoTotal += i->peso;
      jugador->tiempo_restante -= 1; // Descuenta 1 de tiempo
      list_popCurrent(items);        // Elimina el item del escenario
      printf("Item '%s' recogido.\n", i->nombre);
      return;
    }
  }
  printf("Item '%s' no encontrado en el escenario.\n", nombre_item);
}
void descartarItems(player *jugador)
{
  printf("Items en tu inventario:\n");
  for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items))
  {
    printf("Nombre: %s, Valor: %d, Peso: %d\n", i->nombre, i->valor, i->peso);
  }

  printf("Seleccione el nombre del item a descartar: ");
  char nombre_item[50];
  // Limpia el buffer de entrada antes de fgets
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
  fgets(nombre_item, sizeof(nombre_item), stdin);
  // Elimina el salto de línea al final, si existe
  nombre_item[strcspn(nombre_item, "\n")] = 0;

  for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items))
  {
    if (strcmp(i->nombre, nombre_item) == 0)
    {
      jugador->puntajeTotal -= i->valor;
      jugador->pesoTotal -= i->peso;
      jugador->tiempo_restante -= 1; // Descuenta 1 de tiempo
      printf("Item '%s' descartado.\n", i->nombre);
      list_popCurrent(jugador->items);
      free(i->nombre);
      free(i);
      return;
    }
  }
  printf("Item '%s' no encontrado en tu inventario.\n", nombre_item);
}
void movimientosDescenarios(player *jugador)
{
  limpiarPantalla();
  printf("Movimientos disponibles desde el escenario '%s':\n", jugador->escenario_actual->nombre);
  printf("1. Arriba: %d\n", jugador->escenario_actual->enlaces.arriba);
  printf("2. Abajo: %d\n", jugador->escenario_actual->enlaces.abajo);
  printf("3. Izquierda: %d\n", jugador->escenario_actual->enlaces.izquierda);
  printf("4. Derecha: %d\n", jugador->escenario_actual->enlaces.derecha);

  int opcion;
  printf("Seleccione una direccion (1-4): ");
  scanf("%d", &opcion);

  int nuevo_id = -1;
  switch (opcion)
  {
  case 1:
    nuevo_id = jugador->escenario_actual->enlaces.arriba;
    break;
  case 2:
    nuevo_id = jugador->escenario_actual->enlaces.abajo;
    break;
  case 3:
    nuevo_id = jugador->escenario_actual->enlaces.izquierda;
    break;
  case 4:
    nuevo_id = jugador->escenario_actual->enlaces.derecha;
    break;
  default:
    printf("Opcion invalida.\n");
    return;
  }

  if (nuevo_id >= 0 && nuevo_id < maxescenarios && totalEscenarios[nuevo_id] != NULL)
  {
    jugador->escenario_actual = totalEscenarios[nuevo_id];
    // Descuenta tiempo según la fórmula
    float tiempo_usado = (jugador->pesoTotal + 1) / 10.0;
    jugador->tiempo_restante -= tiempo_usado;
    printf("Te has movido al escenario '%s'. Tiempo usado: %.2f\n", jugador->escenario_actual->nombre, tiempo_usado);

    // Si es final, mostrar inventario y puntaje
    if (jugador->escenario_actual->es_final)
    {
      printf("\n!Has llegado al escenario final!\n");
      printf("Inventario final:\n");
      for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items))
      {
        printf("  - %s (valor: %d, peso: %d)\n", i->nombre, i->valor, i->peso);
      }
      printf("Puntaje final: %d\n", jugador->puntajeTotal);
      fflush(stdout);
      getchar(); // Espera que el usuario presione ENTER
      exit(0);
    }
  }
  else
  {
    printf("Movimiento invalido, no hay un escenario en esa direccion.\n");
  }
}

void mostrarEstadoActual(player *jugador)
{
  printf("\n=== Escenario Actual ===\n");
  printf("Escenario: %s\n", jugador->escenario_actual->nombre);
  printf("Descripcion: %s\n", jugador->escenario_actual->descripcion);
  printf("Tiempo restante: %.2f\n", jugador->tiempo_restante);
  printf("Puntaje total: %d\n", jugador->puntajeTotal);
  printf("Peso total: %d\n", jugador->pesoTotal);
  printf("Inventario:\n");
  for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items))
  {
    printf("  - %s (valor: %d, peso: %d)\n", i->nombre, i->valor, i->peso);
  }
}
void reiniciarPartida(player *jugador)
{
  jugador->tiempo_restante = 10;
  jugador->puntajeTotal = 0;
  jugador->pesoTotal = 0;
  list_clean(jugador->items);
  jugador->escenario_actual = totalEscenarios[1]; // <-- Entrada principal
  printf("Partida reiniciada. Buena suerte!\n");
}

int main()
{
  leer_escenarios();

  player jugador;
  jugador.tiempo_restante = 10;
  jugador.puntajeTotal = 0;
  jugador.pesoTotal = 0;
  jugador.items = list_create();
  jugador.escenario_actual = totalEscenarios[1]; // <-- Entrada principal

  while (jugador.tiempo_restante > 0)
  {
    mostrarEstadoActual(&jugador);
    printf("Seleccione una accion:\n");
    printf("1. Recoger items\n");
    printf("2. Descartar items\n");
    printf("3. Moverse a otro escenario\n");
    printf("4. Reiniciar Partida\n");
    printf("5. Salir del juego\n");

    int opcion;
    scanf("%d", &opcion);

    switch (opcion)
    {
    case 1:
      limpiarPantalla();
      recogerItems(&jugador);
      break;
    case 2:
      limpiarPantalla();
      descartarItems(&jugador);
      break;
    case 3:
      movimientosDescenarios(&jugador);
      break;
    case 4:
      limpiarPantalla();
      reiniciarPartida(&jugador);
      break;
    case 5:
      printf("Saliendo del juego...\n");
      return 0;
    default:
      printf("Opcion invalida. Intente nuevamente.\n");
    }
  }

  printf("Tiempo agotado. !Has perdido!\n");
  printf("Puntaje total: %d\n", jugador.puntajeTotal);
  return 0;
}
