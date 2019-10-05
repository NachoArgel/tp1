#inlcude "hash.h"
#DEFINE TAM 32


typedef campo campo_t{
	char* clave;
	void* valor;
};

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
	hash->capacidad = TAM;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if(hash->cantidad >= 2*hash->capacidad){
		if(!hash_redimensionar(hash)){
			return false;
		}
	}		
	int pos=hashing(clave,hash);
	
	campo_t* campo = malloc(sizeof(campo_t));
	if(campo==NULL){
		return NULL;
	}
	campo->valor=dato;
	campo->clave=clave;
	
	if (hash->listas[pos]==NULL){
		hash->listas[pos]=malloc(sizeof(lista_t));
		if(hash->listas[pos]==NULL){
			return NULL;
		}
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
	if((hash->listas[pos]==NULL)||(lista_esta_vacia(hash->listas[pos]))){
		return NULL;
	}
	if(lista_ver_primero(hash->listas[pos])->clave == clave){
		return lista_borrar_primero(hash->listas[pos])->valor;
	}
	cantidad--;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	int pos = hashing(clave,hash);
	
	if((hash->listas[pos]==NULL)||(lista_esta_vacia(hash->listas[pos]))){
		return NULL;
	}
	if(lista_ver_primero(hash->listas[pos])->clave == clave){
		return lista_ver_primero(hash->lista[pos])->valor;
	}
}	
	
	
	
bool hash_pertenece(const hash_t *hash, const char *clave);
size_t hash_cantidad(const hash_t *hash);
void hash_destruir(hash_t *hash);

bool hash_redimensionar(){

hash_iter_t *hash_iter_crear(const hash_t *hash);
bool hash_iter_avanzar(hash_iter_t *iter);
const char *hash_iter_ver_actual(const hash_iter_t *iter);
bool hash_iter_al_final(const hash_iter_t *iter);
void hash_iter_destruir(hash_iter_t* iter);
