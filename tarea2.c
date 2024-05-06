#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char id[100];
  char title[100];
  List *genres;
  char director[300];
  List *directores;
  float rating;
  int year;
} Film;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Películas");
  puts("========================================");

  puts("1) Cargar Películas");
  puts("2) Buscar por id");
  puts("3) Buscar por director");
  puts("4) Buscar por género");
  puts("5) Buscar por década");
  puts("6) Buscar por rango de calificaciones");
  puts("7) Buscar por década y género");
  puts("8) Salir");
}

void borrarComillas(char *str) { 

    int len = strlen(str); //Obtenemos la longitud de la cadena

    if (str[0] == '"') { //SI el primer carácter es una comilla:

        memmove(str, str + 1, len); //Movemos el inicio de la cadena a un espacio a la derecha

        len--; //Bajamos en 1 el tamaño de la cadena

    }

    if (len > 0 && str[len - 1] == '"') { //Si el último carácter es una comilla

        str[len - 1] = '\0'; //Lo cambiamos al carácter nulo para que ahí termine la cadena.

    }

}

void rellenarDirectores(Map * pelis_bydirector, Film *peli){
  char *director = list_first(peli->directores);
  while (director != NULL) {
    strcpy(peli->director, director);

    if (map_search(pelis_bydirector, director) == NULL){
      List *listaDirectores = list_create();
      map_insert(pelis_bydirector, director, listaDirectores);
      list_pushFront(listaDirectores, peli);
    } 
    else{
      MapPair *Pair = map_search(pelis_bydirector, director);
      list_pushBack(Pair->value, peli);
    }
    director = list_next(peli->directores);
  }
}

void rellenarGenero(Map *pelis_bygenre, Film *peli){
    char *genero = list_first(peli->genres);
    while (genero != NULL) {
        if (map_search(pelis_bygenre, genero) == NULL){
            List *listaPeliculas = list_create();
            map_insert(pelis_bygenre, genero, listaPeliculas);
            list_pushFront(listaPeliculas, peli);
        } 
        else{
            MapPair *Pair = map_search(pelis_bygenre, genero);
            list_pushBack(Pair->value, peli);
        }
        genero = list_next(peli->genres);
    }
}

char *espacioInicial(char *str) {

    while (isspace((unsigned char)*str)) str++; 

    return str; 

}
/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}


void cargar_peliculas(Map *pelis_byid, Map *pelis_bydirector, Map *pelis_bygenre){
  FILE *archivo = fopen("data/Top1500.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); 
    return;
  }

  char **campos;
  campos = leer_linea_csv(archivo, ',');

  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Film *peli = (Film *)malloc(sizeof(Film));    
    // Inicializar la lista de géneros y directores
    peli->genres = list_create();
    peli->directores = list_create();

    strcpy(peli->id, campos[1]);
    strcpy(peli->title, campos[5]);

    // Procesar los géneros
    borrarComillas(campos[11]);
    char *tokGeneros = strtok(campos[11], ",");
    while (tokGeneros != NULL){
      char *limpiarTok = espacioInicial(tokGeneros);
      list_pushBack(peli->genres, strdup(limpiarTok));
      tokGeneros = strtok(NULL, ",");
    }

    // Procesar los directores
    borrarComillas(campos[14]);
    char *tokDirector = strtok(campos[14], ",");
    while (tokDirector != NULL){
      char *limpiarTokDos = espacioInicial(tokDirector);
      list_pushBack(peli->directores, strdup(limpiarTokDos));
      tokDirector = strtok(NULL, ",");
    }

    peli->year = atoi(campos[10]); 
    peli->rating = atof(campos[8]);

    map_insert(pelis_byid, peli->id, peli);
    rellenarDirectores(pelis_bydirector, peli); 
    rellenarGenero(pelis_bygenre, peli);
  }
  fclose(archivo); 

  MapPair *pair = map_first(pelis_byid);
  while (pair != NULL) {
    Film *peli = pair->value;
    printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title,
           peli->director, peli->year);
    pair = map_next(pelis_byid); 
  }
}

void buscar_por_id(Map *pelis_byid) {
  char id[10]; 
  printf("Ingrese el id de la película: ");
  scanf("%s", id); 
  MapPair *pair = map_search(pelis_byid, id);
  if (pair != NULL) {
    Film *peli =
        pair->value;
    printf("Título: %s, Año: %d\n", peli->title, peli->year);
  } else {
    printf("La película con id %s no existe\n", id);
  }
}

void buscar_por_director(Map * pelis_bydirector) {
  getchar();
  char director[30];
  printf("Ingrese el director de la película: ");
  getchar();
  scanf("%[^\n]s", director);
  MapPair *pair = map_search(pelis_bydirector, director);
  if (pair) {
    Film *peli = (Film *)list_first(pair->value);
    printf("Películas del director %s:\n", director);
    while (peli != NULL) {
      printf("Título: %s, Año: %d, Director: ",peli->title, peli->year); 
      char *director = (char *)list_first(peli->directores);
      printf("%s", director);
      director= (char*)list_next(peli->directores);
      while (director != NULL){
        printf(", %s", director);
        director = (char*)list_next(peli->directores);
      }
      printf("\n");
      peli = (Film *)list_next(pair->value);
    }
  }
  else{
    printf("No se encontraron películas del director %s\n", director);
  }
}

void buscar_por_genero(Map *pelis_bygenre) {
  //misma lógica que buscar por director
  getchar(); 
  char genre[20];
  printf("Ingrese el género de la película: ");
  scanf(" %19[^\n]s", genre);

  MapPair *pair = map_search(pelis_bygenre, genre);

  if (pair) {
      List *peliculas = pair->value;
      Film *peli = (Film *)list_first(peliculas);

      printf("Películas del género %s:\n", genre);
      while (peli != NULL) {
          printf("Título: %s, Año: %d, Director: %s, Rating: %.1f\n",
                 peli->title, peli->year, peli->director, peli->rating);
          peli = (Film *)list_next(peliculas);
      }
  } else {
      printf("No se encontraron películas del género %s\n", genre);
  }
}

int main() {
  char opcion;
  Map *pelis_byid = map_create(is_equal_str);
  Map *pelis_bygenre = map_create(is_equal_str);
  Map *pelis_bydirector = map_create(is_equal_str);
  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_peliculas(pelis_byid, pelis_bydirector, pelis_bygenre);
      break;
    case '2':
      buscar_por_id(pelis_byid);
      break;
    case '3':
      buscar_por_director(pelis_bydirector);
      break;
    case '4':
      buscar_por_genero(pelis_bygenre);
      break;
    case '5':
      break;
    case '6':
      break;
    case '7':
      break;
    default:
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}
