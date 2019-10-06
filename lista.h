#ifndef LISTA_H
#define LISTA_H
#include <stdbool.h>
#include <stddef.h>
/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

//Si la lista esta vacia devuelve true, sino false.
//Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

//Inserta un elemento al principio de la lista.
//Si no se pudo insertar devuelve false.
//Pre: la lista fue creada.
//Post: se agregó un nuevo elemento a la lista, valor se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

//Inserta un elemento al final de la lista.
//Si no se pudo insertar devuelve false.
//Pre: la lista fue creada.
//Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Borra el elemento al principio de la lista.
//Pre: la lista fue creada.
//Post: se elimino un elemento a la lista en la primera posicion.Devuelve el valor.
void *lista_borrar_primero(lista_t *lista);

//Devuelve el valor en la primera posicion de la lista.
//Pre: la lista fue creada.
void *lista_ver_primero(const lista_t *lista);

//Devuelve el valor en la ultima posicion de la lista.
//Pre: la lista fue creada.
void *lista_ver_ultimo(const lista_t* lista);

//Devuelve el largo de la lista
//Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

//Recorre la lista y aplica la funcion visitar a cada dato.
//Si visitar en algun momento devuelve NULL, se corta la iteracion.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

//Crea un iterador.
//Pre: la lista fue creada.
// Post: devuelve un iterador en la primer posicion de la lista.
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza una posicion el iterador.
//Pre: el iterador fue creado.
bool lista_iter_avanzar(lista_iter_t *iter);

//Devuelve el elemento al que apunta el iterador.
//En caso de estar en el final devuelve NULL.
//Pre: el iterador fue creado.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve true si el iterador esta al final.(apuntando
//al siguiente del ultimo).
//Pre: el iterador fue creado.
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador.
//Pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un elemento en la posicion a la que apunta
//el iterador.Devuelve true si se inserto y false en 
//caso de error.
//Pre: el iterador fue creado.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Inserta un elemento en la posicion a la que apunta
//el iterador.Devuelve true si se inserto y false en 
//caso de error.
//Pre: el iterador fue creado.
void *lista_iter_borrar(lista_iter_t *iter);


void pruebas_lista_alumno(void);

#endif // COLA_H