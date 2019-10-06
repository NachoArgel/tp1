#include "hash.h"
#include "lista.h"
const int primos[] = {41,101,211,401,809,1601,3209,6421,12809,25603,50021,100003,200003,
400009,800011};



typedef campo campo_t{
	char* clave;
	void* valor;
};

campo_t* campo_crear(clave,valor){
	campo_t* campo = malloc(sizeof(campo_t));
	if(campo==NULL){
		return NULL;
	}
	campo->valor=valor;
	campo->clave=clave;
	return campo;
}

struct hash hash_t{
	lista_t** listas;
	size_t capacidad; 
	size_t cantidad;
	void (*hash_destruir_dato_t)(void *);
};

struct hash_iter hash_iter_t{
	hash_t* hash;
	campo_t* actual;
	campo_t* anterior;
};
	
void (*hash_destruir_dato_t)(void *);

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash==NULL){
		return NULL;
	}
	hash->listas = malloc(TAM*sizeof(lista_t*));	
	if(hash->listas = NULL){
		free(hash);
		return NULL;
	}
	hash->cantidad = 0;
	hash->capacidad = TAM;}
	hash->
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if(hash->cantidad >= 2*hash->capacidad){
		if(!hash_redimensionar(hash)){
			return false;
		}
	}		
	int pos=hashing(clave,hash);
	
	campo_t* campo = campo_crear(clave,dato);
	if (campo == NULL)return NULL;
	if (hash->listas[pos] == NULL){
		hash->listas[pos] = lista_crear());
		if(hash->listas[pos] == NULL)return NULL;
	}
	if(!lista_insertar_ultimo(hash->listas[pos],campo)){
		return NULL;
	}
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
	if((lista_esta_vacia(hash->listas[pos]))){
		return NULL;
	}
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	while(!lista_iter_al_final(iter) && lista_iter_ver_actual(iter)->dato->clave != clave){
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter))return NULL;
	hash->cantidad --;
	return lista_iter_borrar(iter);
	
}

void *hash_obtener(const hash_t *hash, const char *clave){
	int pos = hashing(clave,hash);
	
	if((lista_esta_vacia(hash->listas[pos]))){
		return NULL;
	}
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	while(!lista_iter_al_final(iter) && lista_iter_ver_actual(iter)->dato->clave != clave){
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter))return NULL;
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

bool hash_redimensionar(hash){
	
	if (hash->capacidad <= 800011){
		sizeof dim = 0
		for (i=0 ; i<15; i++){
			if (primos[i]==capacidad;
			dim = i+1;
		}
		lista_t** listas_nuevo = malloc(dim*sizeof(lista_t*))
	}else{
		lista_t** listas_nuevo = malloc(2*sizeof(lista_t*))
	}

}

hash_iter_t *hash_iter_crear(const hash_t *hash);
bool hash_iter_avanzar(hash_iter_t *iter);
const char *hash_iter_ver_actual(const hash_iter_t *iter);
bool hash_iter_al_final(const hash_iter_t *iter);
void hash_iter_destruir(hash_iter_t* iter);
