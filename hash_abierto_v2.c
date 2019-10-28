#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#define achicar -1
#define agrandar 1
#define TAM 31
#define MIN 1/4
#define MAX 2


struct hash{
	lista_t** listas;
	size_t capacidad; 
	size_t cantidad;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter{
    size_t pos;
    size_t iterados;
    lista_iter_t* iter_lista;
    const hash_t* hash;
};

typedef struct campo{
	const char* clave;
	void* valor;
}campo_t;

char *copiar_clave(const char *clave)
{
    char *copia = malloc(strlen(clave)+1);
    if (copia == NULL){
        return NULL;
	}
    strcpy(copia, clave);
    return copia;
}

campo_t* campo_crear(const char* clave, void* valor){
	campo_t* campo = malloc(sizeof(campo_t));
	if(campo==NULL){
		return NULL;
	}
	char* copia = copiar_clave(clave);
	if (copia == NULL){
		free (campo);
		return NULL;
	}
	campo->valor = valor;
	campo->clave = copia;
	return campo;
}

void campo_destruir(campo_t *campo, hash_destruir_dato_t destruir_dato){
		if(destruir_dato){
			destruir_dato(campo->valor);
		}
		free((void*)campo->clave);
		free(campo);
}

/*****************************************************************
 *                   FUNCIONES AUXILIARES HASH                   *
 * ***************************************************************/

size_t hashing(const char *word, size_t len)
{       int sum = 0;
        for (int k = 0; k < strlen(word); k++){
            int res = 5*sum;
            sum = res + word[k];
		}
        return sum % len; 
}

void destruir_listas(lista_t** listas, size_t i, hash_t *hash){
	for(i = i ;i > 0 ; i--){
		while(!lista_esta_vacia(listas[i])){
			campo_t* campo = (campo_t*)lista_borrar_primero(listas[i]);
			campo_destruir(campo,hash->destruir_dato);
		}
		lista_destruir(listas[i],NULL);
	}
	lista_destruir(listas[i],NULL);
	free(listas);
}

bool hash_copiar(hash_t* hash, size_t TAM_NUEVO, lista_t** listas_nuevo){
	
	for( size_t lista_i = 0; lista_i < hash->capacidad ; lista_i++){
		lista_iter_t* lista_iter = lista_iter_crear(hash->listas[lista_i]);
		if(!lista_iter){
			destruir_listas(listas_nuevo,lista_i,hash);
			return false;
		}
		while(!lista_iter_al_final(lista_iter)){			
			campo_t* campo = lista_iter_ver_actual(lista_iter);
			size_t pos = hashing(campo->clave,TAM_NUEVO);
			if(!lista_insertar_ultimo(listas_nuevo[pos],campo)){
				destruir_listas(listas_nuevo,lista_i,hash);
				return false;
			}
			lista_iter_avanzar(lista_iter);
		}
		lista_destruir(hash->listas[lista_i],NULL);
		lista_iter_destruir(lista_iter);
	}
	free(hash->listas);
	hash->listas = listas_nuevo;
	hash->capacidad = TAM_NUEVO;
	return true;
}


bool hash_redimensionar(hash_t* hash, int redimension ){
	size_t TAM_NUEVO;
	
	if( redimension == achicar){
		if(hash->capacidad <= TAM)return false;
		TAM_NUEVO = hash->capacidad/2;
	}else{				
		TAM_NUEVO = hash->capacidad*2;
	}
	
	lista_t** listas_nuevo = malloc(TAM_NUEVO*sizeof(lista_t*));
	if ( listas_nuevo == NULL ){
		return false;
	}
	for(size_t i = 0; i<TAM_NUEVO ; i++){
		listas_nuevo[i] = lista_crear();
		if(!listas_nuevo[i]){
			for(i=i ; i > 0 ; i--){
				lista_destruir(listas_nuevo[i],NULL);
			}
			free(listas_nuevo);	
			return false;
		}
	}
	return hash_copiar(hash,TAM_NUEVO,listas_nuevo);
}

campo_t *hash_buscar(const hash_t *hash, const char *clave, lista_iter_t *iter){
	while(!lista_iter_al_final(iter)){
		if (strcmp(((campo_t*)lista_iter_ver_actual(iter))->clave, clave) == 0){
			return ((campo_t*)lista_iter_ver_actual(iter));	
		}
		lista_iter_avanzar(iter);
	}
	return NULL;	
}

/*****************************************************************
 *                         PRIMITIVAS HASH                       *
 * ***************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash==NULL){
		return NULL;
	}
	hash->listas = malloc(TAM*sizeof(lista_t*));	
	if(hash->listas == NULL){
		free(hash);
		return NULL;
	}
	for(int i = 0; i<TAM ; i++){
		hash->listas[i] = lista_crear();
		if(!hash->listas[i]){
			for(i=i ; i > 0 ; i--){
				lista_destruir(hash->listas[i],NULL);
			}
			free(hash);	
			return NULL;
		}
	}	
	hash->destruir_dato = destruir_dato;
	hash->cantidad = 0;
	hash->capacidad = TAM;
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	
	if(hash->cantidad == hash->capacidad*MAX){
		if(!hash_redimensionar(hash,agrandar)){
			return false;
		}
	}		
		
	campo_t* campo = campo_crear(clave,dato);
	if (campo == NULL)return false;
	
	size_t pos=hashing(clave,hash->capacidad);

	lista_iter_t *iter = lista_iter_crear(hash->listas[pos]);
	if(!iter){
		campo_destruir(campo,hash->destruir_dato);
		return NULL;
	}
	
	campo_t *viejo = hash_buscar(hash,clave,iter);
	if(viejo != NULL){
		campo_destruir(viejo,hash->destruir_dato);
		lista_iter_borrar(iter);
		hash->cantidad--;
	}
	lista_iter_insertar(iter,campo);
	lista_iter_destruir(iter);
		
	hash->cantidad++;	
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	
	if(hash->cantidad <= hash->capacidad*MIN){ 
		hash_redimensionar(hash,achicar);
	}
	size_t pos = hashing(clave,hash->capacidad);

	lista_iter_t *iter = lista_iter_crear(hash->listas[pos]);
	if(!iter)return NULL;
	
	campo_t *a_borrar = hash_buscar(hash,clave,iter);
	if(a_borrar == NULL){
		lista_iter_destruir(iter);
		return NULL;
	}
	void *dato = a_borrar->valor;
	lista_iter_borrar(iter);
	campo_destruir(a_borrar,NULL);	
	hash->cantidad--;
	
	lista_iter_destruir(iter);
	
	return dato;	
}

void *hash_obtener(const hash_t *hash, const char *clave){
	size_t pos = hashing(clave,hash->capacidad);
	
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	if(!iter)return NULL;
	
	campo_t *buscado = hash_buscar(hash,clave,iter);
	lista_iter_destruir(iter);
	
	if(!buscado)return NULL;
	return buscado->valor;	
}
		
//Es lo mismo que hash_obtener pero al poner return hash_obtener(hash,clave)
//habia un caso que no estaba contemplado
bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t pos = hashing(clave,hash->capacidad);	
	
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	if(!iter)return NULL;
	
	campo_t *buscado = hash_buscar(hash,clave,iter);
	lista_iter_destruir(iter);
	
	if(!buscado)return false;
	return true;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	destruir_listas(hash->listas,hash->capacidad-1,hash);
	free(hash);
}


/*****************************************************************
 *                FUNCIONES AUXILIARES ITERADOR                  *
 * ***************************************************************/
 
size_t buscar_prox_listas(hash_iter_t* hash_iter,size_t i){
	while(i < hash_iter->hash->capacidad-1 && lista_esta_vacia(hash_iter->hash->listas[i])){
		i++;
	}
	if(i >= hash_iter->hash->capacidad)return false;
	hash_iter->iter_lista = lista_iter_crear(hash_iter->hash->listas[i]);
	if(!hash_iter->iter_lista)return false;
	return i;
}

/****************************************************************
 *                PRIMITIVAS DEL ITERADOR                       *
 ****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
	
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if(!hash_iter)return NULL;
	
	hash_iter->hash = hash;
	hash_iter->iterados = 0;
	
	if(hash->cantidad==0){
		hash_iter->iter_lista = NULL;
	}else{
		size_t i = buscar_prox_listas(hash_iter,0);
		if (!i)return NULL;
		hash_iter->pos = i;
		if(!hash_iter->pos)return NULL;
	}
	return hash_iter;
}	

bool hash_iter_avanzar(hash_iter_t *iter){
	
	if(hash_iter_al_final(iter))return false;

	if(lista_iter_ver_actual(iter->iter_lista) == lista_ver_ultimo(iter->hash->listas[iter->pos])){
		lista_iter_destruir(iter->iter_lista);
		iter->pos++;
		iter->pos = buscar_prox_listas(iter,iter->pos);
		if(!iter->pos)return false;
	}else{
		if(!lista_iter_avanzar(iter->iter_lista)) return false;
	}
	iter->iterados++;
	return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	return ((campo_t*)lista_iter_ver_actual(iter->iter_lista))->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return iter->iterados==iter->hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->iter_lista);
	free(iter);
}
