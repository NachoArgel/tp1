#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>

//DEFINE TAM 32
const int primos[] = {41,101,211,401,809,1601,3209,6421,12809,25603,50021,100003,200003,
400009,800011};



typedef struct campo{
	char* clave;
	void* valor;
}campo_t;

campo_t* campo_crear(const char* clave,void* valor){
	campo_t* campo = malloc(sizeof(campo_t));
	if(campo==NULL){
		return NULL;
	}
	campo->valor=valor;
	campo->clave=clave;
	return campo;
}

struct hash{
	lista_t** listas;
	size_t capacidad; 
	size_t cantidad;
	void (*hash_destruir_dato_t)(void *);
};

struct hash_iter{
    size_t pos;
    size_t iterados;
    lista_iter_t* iter_lista;
    const hash_t* hash;
};
	
void (*hash_destruir_dato_t)(void *);

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash==NULL){
		return NULL;
	}
	hash->listas = malloc(primos[0]*sizeof(lista_t*));	
	if(hash->listas = NULL){
		free(hash);
		return NULL;
	}
	hash->cantidad = 0;
	hash->capacidad = primos[0];//hash->capacidad = TAM;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	
	if(hash->cantidad >= 2*hash->capacidad){
		if(!hash_redimensionar(hash)){
			return false;
		}
	}		
	int pos=hashing(clave,hash);
	
	campo_t* campo = campo_crear(clave,dato);
	if (campo == NULL){
		return false;
	}
	if (hash->listas[pos] == NULL){
		hash->listas[pos] = lista_crear();
		if(hash->listas[pos] == NULL){
			return false;
		}
	}
	//Chequeo que no exista la clave que recibi, y de existir le actualizo el dato
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	if(!iter){
		free(campo);
		return false;
	}
	//Creo un booleano para saber si ya inserte o no el valor
	bool insertado = false;
	while(!lista_iter_al_final(iter) && lista_iter_ver_actual(iter)->clave != clave){
		if(lista_iter_ver_actual(iter)->clave == clave){
			if(!lista_iter_insertar(iter,campo))return false;
			insertado = true;
		}
			lista_iter_avanzar(iter);
	}
	if(!insertado){			
		if(!lista_iter_insertar(iter,campo)){
			return false;
		}
	}
	lista_iter_destruir(iter);
	hash->cantidad++;	
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	
	if(hash->cantidad <= (hash->capacidad)/4){
		if(!hash_redimensionar(hash)){
			return NULL;
		}
	}
	int pos = hashing(clave,hash);
	if(lista_esta_vacia(hash->listas[pos])){
		return NULL;
	}
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	while(!lista_iter_al_final(iter) && lista_iter_ver_actual(iter)->clave != clave){
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter)){
		return NULL;
	}
	hash->cantidad--;
	return lista_iter_borrar(iter);
}

void *hash_obtener(const hash_t *hash, const char *clave){
	int pos = hashing(clave,hash);
	
	if(lista_esta_vacia(hash->listas[pos])){
		return NULL;
	}
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	while(!lista_iter_al_final(iter) && lista_iter_ver_actual(iter)->clave != clave){
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter)){
		return NULL;
		}
	return lista_iter_ver_actual(iter);
}		

bool hash_pertenece(const hash_t *hash, const char *clave){
	return (hash_obtener(hash,clave)!= NULL);
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){

}

bool hash_redimensionar(hash_t* hash,  ){
	
	//Falta el caso en el que la redimension sea hacia abajo, habria que recibir
	//por parámetro si quiero achicar o agrandar la tabla
	
	if (hash->capacidad <= 800011){
		size_t dim = 0;
		for (int i=0 ; i<15; i++){
			if (primos[i]==hash->capacidad);
			dim = i+1;
		}
		lista_t** listas_nuevo = malloc(primos[dim]*sizeof(lista_t*));
		if(!listas_nuevo)return false;
	}else{
		lista_t** listas_nuevo = malloc(2*sizeof(lista_t*));
		if(!listas_nuevo)return false;
	}
	//Copio todos los datos del hash viejo que ya no tiene lugar en listas_nuevo, 
	//siempre verificando que si algo no se puede hacer se destruya y libere toda
	//la memoria en uso
	hash_iter_t* hash_iter = hash_iter_crear(hash);
	if(!hash_iter){
		free(listas_nuevo);
		return false;
	}
	
	size_t i = 0;
	while(!hash_iter_al_final(hash_iter)){
		if(hash->listas[i]!=NULL){
			i++;
		}else{
			lista_iter_t* lista_iter = lista_iter_crear(hash->listas[i]);
			if(!lista_iter){
				free (listas_nuevo);
				hash_iter_destruir(hash_iter);
				return false;
			}
			while(!lista_iter_al_final(lista_iter)){
				
				campo_t* campo = lista_iter_borrar(lista_iter);
				int pos = hashing(campo->clave,hash);
				if(!lista_insertar_ultimo(listas_nuevo[pos],campo))return false;
				lista_iter_avanzar(lista_iter);
				hash_iter_avanzar(hash_iter);
			
			}
			lista_destruir(hash->listas[i],hash->hash_destruir_dato_t);
			lista_iter_destruir(lista_iter);
			i++;
		}
	}
	hash_iter_destruir(hash_iter);
	hash->listas = listas_nuevo;
	return true;
}

/****************************************************************
 *                PRIMITIVAS DEL ITERADOR                       *
 ****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
	
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t);
	if(!hash_iter)return NULL;
	
	hash_iter->hash = hash;
	hash_iter->iterados = 0;
	
	size_t i = 0;
	while(lista_esta_vacia(hash->listas[i])&& i < hash_iter->hash->capacidad){
		i++;
	}
	hash_iter->pos = i;
	hash_iter->iter_lista = lista_iter_crear(hash->listas[i]);
	if(!hash_iter->iter_lista)return NULL;
	
	return hash_iter;
}	
	
bool hash_iter_avanzar(hash_iter_t *iter){
	
	if(hash_iter_al_final(iter))return false;
	
	iter->iterados++;
	
	if(lista_iter_al_final(iter->iter_lista){
		lista_iter_destruir(iter->iter_lista);
		iter->pos++;
		while(iter->hash->listas[iter->pos]==NULL){//while(lista_esta_vacia(iter->hash->listas[iter->pos])){
			iter->pos++;
		}
		iter->iter_lista = lista_iter_crear(iter->hash->listas[iter->pos]);
		if(!iter->iter_lista)return false;
	}else{
		if(!lista_iter_avanzar(iter->iter_lista)) return false;
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	return lista_iter_ver_actual(iter->iter_lista)->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return iter->iterados==iter->hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
	lista_iter_destruir(iter->iter_lista);
}
