#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*NODO*/
typedef struct nodo{
    void* dato;
    struct nodo* sig;

}nodo_t;

nodo_t* nodo_crear(void* dato_asig) {
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if (nodo == NULL){
        return NULL;
    }
    nodo->dato = dato_asig;
    nodo->sig = NULL;
    return nodo;
}
void nodo_destruir(nodo_t* nodo){
    free(nodo);
}  

/*LISTA*/
struct lista {
    nodo_t* prim;
    nodo_t* ult;
    size_t largo;
};

lista_t* lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));

    if (lista == NULL){
        return NULL;
    }
    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;
    return lista;

}
bool lista_esta_vacia(const lista_t *lista){
    return (lista->largo == 0);

}
void insertar_lista_vacia(lista_t *lista, nodo_t* nodo){
    lista->prim = nodo;
    lista->ult = nodo;
    lista->largo ++;
}
bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* elem =nodo_crear(dato);
    if (elem == NULL)return false;
    if (lista_esta_vacia(lista)){
        insertar_lista_vacia(lista, elem);
        return true;
    }
    elem->sig = lista->prim;
    lista->prim = elem;
    lista->largo ++;
    return true;

}
bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* elem = nodo_crear(dato);
    if (elem == NULL)return false;
    if (lista_esta_vacia(lista)){
        insertar_lista_vacia(lista, elem);
        return true;
    }
    lista->ult->sig = elem;
    lista->ult = elem;
    lista->largo ++;
    return true;

}
void *lista_borrar_primero(lista_t *lista){
    if (lista_esta_vacia(lista))return NULL;
    nodo_t* sustraido = lista->prim;
    void* valor = lista->prim->dato;
    if (lista->prim == lista->ult){
        lista->ult = NULL;
    }
    lista->prim = lista->prim->sig;
    lista->largo --;
    nodo_destruir(sustraido);
    return valor;

}
void *lista_ver_primero(const lista_t *lista){
    if(lista_esta_vacia(lista))return NULL;
    return lista->prim->dato;

}
void *lista_ver_ultimo(const lista_t* lista){
    if(lista_esta_vacia(lista))return NULL;
    return lista->ult->dato;

}
size_t lista_largo(const lista_t *lista){
    return lista->largo;

}
void lista_destruir(lista_t *lista, void destruir_dato(void *)){
        nodo_t* act = lista->prim;
        while (act != NULL){
            nodo_t* siguiente = act->sig;
            if (destruir_dato != NULL){
                destruir_dato(act->dato);
            }
            nodo_destruir(act);
            act = siguiente;
        }
    free(lista); 
}

/*ITERADOR INTERNO*/

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
    nodo_t* act = lista->prim; 
    bool corte = true;
	while (act != NULL && corte) {
		if (visitar != NULL) {
			corte = visitar(act->dato, extra);
		}
		act = act->sig;
	}
}

/*ITERADOR EXTERNO*/
struct lista_iter {
    lista_t* lista_i;
    nodo_t* ant;
    nodo_t* act;

};

lista_iter_t* lista_iter_crear(lista_t *lista){
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));

    if (iter == NULL){
        return NULL;
    }
    iter->lista_i = lista;
    iter->ant = NULL;
    iter->act = lista->prim;
    return iter;

}
bool lista_iter_avanzar(lista_iter_t *iter){
    if (lista_iter_al_final(iter))return false;
    iter->ant = iter->act;
    iter->act = iter->act->sig;
    return true;
}
void *lista_iter_ver_actual(const lista_iter_t *iter){
    if (lista_iter_al_final(iter))return NULL;
    return iter->act->dato;

}
bool lista_iter_al_final(const lista_iter_t *iter){
    return (iter->act == NULL);

}
void lista_iter_destruir(lista_iter_t *iter){
    free(iter);

}
bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* nodo = nodo_crear(dato); 
    if (!nodo)return false;
    if (iter->ant != NULL){
        iter->ant->sig = nodo;
    }
    if (iter->ant == NULL){
        iter->lista_i->prim = nodo;
    }
    if (iter->act == NULL){
        iter->lista_i->ult = nodo;
    }
    nodo->sig = iter->act;
    iter->act = nodo;
    iter->lista_i->largo ++;
    return true;
}
void *lista_iter_borrar(lista_iter_t *iter){
    if (iter->act == NULL)return NULL;
    nodo_t* borrado = iter->act;
    if (iter->ant != NULL){
        iter->ant->sig = iter->act->sig;
    }
    if (iter->ant == NULL){
        iter->lista_i->prim = iter->act->sig;
    }
    if (iter->act->sig == NULL){
        iter->lista_i->ult = iter->ant;
    }
    void* valor = iter->act->dato;
    iter->act = iter->act->sig;
    iter->lista_i->largo --;
    free(borrado);
    return valor;
}
