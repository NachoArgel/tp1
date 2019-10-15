#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
//#define TAM 32
#define achicar -1
#define agrandar 1
const int primos[] = {41,101,211,401,809,1601,3209,6421,12809,25603,50021,100003,200003,
400009,800011};




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

size_t hashing(const char *cad, size_t largo){
   size_t valor;
   const char *c;

   for(c=cad,valor=0;*c;c++){
      valor+=(size_t)(*c);
	}
   return(valor%largo);
}

/*unsigned long hashing(const char *str){
	unsigned long hashing = 5381;
	int c = *str++;

	while (c == *str++){
		c = *str++;
		hashing = ((hashing << 5) + hashing) + c; // hashing * 33 + c 
	}
	return hashing;
 }*/

/*****************************************************************
 *                         PRIMITIVAS HASH                       *
 * ***************************************************************/


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(hash==NULL){
		return NULL;
	}
	hash->listas = malloc(primos[0]*sizeof(lista_t*));	
	if(hash->listas == NULL){
		free(hash);
		return NULL;
	}
	for(int i = 0; i<primos[0] ; i++){
		hash->listas[i] = lista_crear();
		//Si por alguna razon no puedo crear una lista para la tabla, tengo que 
		//liberar las que ya estan creadas
		if(!hash->listas[i]){
			for(i=i ; i > 0 ; i--){
				free(hash->listas[i]);
			}
			free(hash);	
			return NULL;
		}
	}	
	hash->destruir_dato = destruir_dato;
	hash->cantidad = 0;
	hash->capacidad = primos[0];//hash->capacidad = TAM;
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	
	if(hash->cantidad == 2*hash->capacidad){
		if(!hash_redimensionar(hash,agrandar)){
			return false;
		}
	}		
		
	campo_t* campo = campo_crear(clave,dato);
	if (campo == NULL)return false;
	
	size_t pos=hashing(clave,hash->capacidad);
	//printf("Pos asignada por funcionde hashing es %ld\n", pos);
	
	if(lista_esta_vacia(hash->listas[pos])){
		lista_insertar_ultimo(hash->listas[pos], campo);
		hash->cantidad++;
		return true;
	}	
	//Chequeo que no exista la clave que recibi, y de existir le actualizo el dato destruyendo el anterior
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	if(!iter){
		free(campo);
		return false;
	}
	//Creo un booleano para saber si ya inserte o no el valor
	bool insertado = false;
	while(!lista_iter_al_final(iter)){
		if (strcmp(((campo_t*)lista_iter_ver_actual(iter))->clave, clave) == 0){
			hash->cantidad --;
			campo_t* borrado = ((lista_iter_borrar(iter)));
			if(hash->destruir_dato){
				hash->destruir_dato(borrado->valor);
				insertado = true;
			}
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
		hash_redimensionar(hash,achicar);
	}
	size_t pos = hashing(clave,hash->capacidad);
	if(lista_esta_vacia(hash->listas[pos])){
		return NULL;
	}
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	while(!lista_iter_al_final(iter)&&((strcmp(((campo_t*)lista_iter_ver_actual(iter))->clave, clave) != 0))){
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter)){
		return NULL;
	}
	hash->cantidad--;
	return ((campo_t*)lista_iter_borrar(iter))->valor;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if(hash->cantidad==0)return NULL;
	
	size_t pos = hashing(clave,hash->capacidad);
	
	if(lista_esta_vacia(hash->listas[pos]))return NULL;
	
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	
	while(!lista_iter_al_final(iter)){
		if (strcmp(((campo_t*)lista_iter_ver_actual(iter))->clave, clave) == 0){
			void* valor = ((campo_t*)lista_iter_ver_actual(iter))->valor;
			lista_iter_destruir(iter);
			return valor;
		}
		lista_iter_avanzar(iter);
	}
	return NULL;
}		

bool hash_pertenece(const hash_t *hash, const char *clave){
	if(hash->cantidad==0)return false;	
	size_t pos = hashing(clave,hash->capacidad);	
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	while(!lista_iter_al_final(iter)){
		if (strcmp(((campo_t*)lista_iter_ver_actual(iter))->clave, clave) == 0){
			return true;
		}
		lista_iter_avanzar(iter);
	}
	return false;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for(size_t i = 0 ; i<hash->cantidad ; i++){
		/*while(!lista_esta_vacia(hash->listas[i])){
			campo_t* campo = (campo_t*)lista_borrar_primero(hash->listas[i]);
		//	hash->destruir_dato(campo->valor);
			free(campo);
		}*/
		lista_destruir(hash->listas[i],NULL);
	}
	free(hash);
}

/*
bool hash_redimensionar(hash_t* hash, size_t TAM_NUEVO ){//SIN PRIMOS
	
	if( TAM_NUEVO < TAM ){//Menor que el tamaño original
				return false;
	}
				
	lista_t** listas_nuevo = malloc(TAM_NUEVO*sizeof(lista_t*));
		
		if ( listas_nuevo == NULL ){
			return false;
		}else{
			return hash_copiar(hash,TAM_NUEVO,listas_nuevo);
		}
}*/

bool hash_copiar(hash_t* hash, size_t TAM_NUEVO, lista_t** listas_nuevo){
	
	size_t pos = 0;
	size_t i = 0;
	while(pos<=hash->cantidad){
		if (lista_esta_vacia(hash->listas[i])){
			pos++;
		}else{
			lista_iter_t* lista_iter = lista_iter_crear(hash->listas[i]);
			if(!lista_iter){
				free (listas_nuevo);
				return false;
			}
			while(!lista_iter_al_final(lista_iter)){//lista_esta_vacia
				
				campo_t* campo = lista_iter_ver_actual(lista_iter);
				size_t pos = hashing(campo->clave,TAM_NUEVO);
				if(!lista_insertar_ultimo(listas_nuevo[pos],campo))return false;
				lista_iter_avanzar(lista_iter);
				pos++;
			
			}
			lista_destruir(hash->listas[i],NULL);
			lista_iter_destruir(lista_iter);
		}
		
	}
	hash->listas = listas_nuevo;
	hash->capacidad = TAM_NUEVO;
	return true;
}
/*bool hash_copiar(hash_t* hash, size_t TAM_NUEVO, lista_t** listas_nuevo){
	
	hash_iter_t* hash_iter = hash_iter_crear(hash);
	if(!hash_iter){
		free(listas_nuevo);
		return false;
	}
	size_t i = 0;
	while(!hash_iter_al_final(hash_iter)){
		if (lista_esta_vacia(hash->listas[i])){
			hash_iter_avanzar(hash_iter);
		}else{
			lista_iter_t* lista_iter = lista_iter_crear(hash->listas[i]);
			if(!lista_iter){
				free (listas_nuevo);
				hash_iter_destruir(hash_iter);
				return false;
			}
			while(!lista_iter_al_final(lista_iter)){//lista_esta_vacia
				
				campo_t* campo = lista_iter_ver_actual(lista_iter);
				size_t pos = hashing(campo->clave,TAM_NUEVO);
				if(!lista_insertar_ultimo(listas_nuevo[pos],campo))return false;
				lista_iter_avanzar(lista_iter);
				hash_iter_avanzar(hash_iter);
			
			}
			lista_destruir(hash->listas[i],NULL);
			lista_iter_destruir(lista_iter);
		}
		
	}
	hash_iter_destruir(hash_iter);
	hash->listas = listas_nuevo;
	hash->capacidad = TAM_NUEVO;
	return true;
}*/

bool hash_redimensionar(hash_t* hash, int redimension ){//CON PRIMOS
	size_t dim = 0;
	if(redimension==achicar){
		if(hash->capacidad <= primos[0])return false;
		if (hash->capacidad <= 800011){
			for (int i=0 ; i<14; i++){
				if (primos[i]==hash->capacidad);
				dim = i-1;
			}
			lista_t** listas_nuevo = malloc(primos[dim]*sizeof(lista_t*));
			if(listas_nuevo==NULL)return false;
			for(int i = 0; i<primos[dim] ; i++){
				listas_nuevo[i] = lista_crear();
				//Si por alguna razon no puedo crear una lista para la tabla, tengo que 
				//liberar las que ya estan creadas
				if(!listas_nuevo[i]){
					for(i=i ; i > 0 ; i--){
						free(listas_nuevo[i]);
					}
					free(listas_nuevo);	
					return false;
				}
			}
			return hash_copiar(hash,primos[dim],listas_nuevo);
		}else{
			if (hash->capacidad/2 <= 800011){
				lista_t** listas_nuevo = malloc(primos[15]*sizeof(lista_t*));
				for(int i = 0; i<primos[15] ; i++){
					listas_nuevo[i] = lista_crear();
					//Si por alguna razon no puedo crear una lista para la tabla, tengo que 
					//liberar las que ya estan creadas
					if(!listas_nuevo[i]){
						for(i=i ; i > 0 ; i--){
							free(listas_nuevo[i]);
						}
						free(listas_nuevo);	
						return false;
					}
				}	
				return hash_copiar(hash,primos[15],listas_nuevo);
			}else{
				lista_t** listas_nuevo = malloc(hash->capacidad*sizeof(lista_t*)/2);
				if(listas_nuevo==NULL)return false;
				for(int i = 0; i<(hash->capacidad)/2 ; i++){
					listas_nuevo[i] = lista_crear();
					//Si por alguna razon no puedo crear una lista para la tabla, tengo que 
					//liberar las que ya estan creadas
					if(!listas_nuevo[i]){
						for(i=i ; i > 0 ; i--){
							free(listas_nuevo[i]);
						}
						free(listas_nuevo);	
						return false;
					}
				}
				return hash_copiar(hash,(hash->capacidad)/2,listas_nuevo);
			}
		}
	}else{
		if (hash->capacidad <= 800011){
			for (int i=0 ; i<14; i++){
				if (primos[i]==hash->capacidad);
				dim = i+1;
			}
			
			lista_t** listas_nuevo = malloc(primos[dim]*sizeof(lista_t*));
			if(listas_nuevo==NULL)return false;
			for(int i = 0; i<primos[dim] ; i++){
				listas_nuevo[i] = lista_crear();
				//Si por alguna razon no puedo crear una lista para la tabla, tengo que 
				//liberar las que ya estan creadas
				if(!listas_nuevo[i]){
					for(i=i ; i > 0 ; i--){
						free(listas_nuevo[i]);
					}
					free(listas_nuevo);	
					return false;
				}
			}	
			return hash_copiar(hash,primos[dim],listas_nuevo);
		}else{
			lista_t** listas_nuevo = malloc(2*hash->capacidad*sizeof(lista_t*));
			if(listas_nuevo==NULL)return false;
			for(int i = 0; i<2*(hash->capacidad) ; i++){
				listas_nuevo[i] = lista_crear();
				//Si por alguna razon no puedo crear una lista para la tabla, tengo que 
				//liberar las que ya estan creadas
				if(!listas_nuevo[i]){
					for(i=i ; i > 0 ; i--){
						free(listas_nuevo[i]);
					}
					free(listas_nuevo);	
					return false;
				}
			}	
			return hash_copiar(hash,2*hash->capacidad,listas_nuevo);
		}
	}
}

/*
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
					size_t pos = hashing(campo->clave,primos[dim]);
					if(!lista_insertar_ultimo(listas_nuevo[pos],campo))return false;
					lista_iter_avanzar(lista_iter);
					hash_iter_avanzar(hash_iter);
				
				}
				lista_destruir(hash->listas[i],hash->destruir_dato);
				lista_iter_destruir(lista_iter);
				i++;
			}
		}
		hash_iter_destruir(hash_iter);
		hash->listas = listas_nuevo;
		}
	}
	return true;
*/


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
		size_t i = 0;
		while(lista_esta_vacia(hash->listas[i]) && i < hash_iter->hash->capacidad){
			i++;
		}
		hash_iter->pos = i;
		hash_iter->iter_lista = lista_iter_crear(hash->listas[i]);
		if(!hash_iter->iter_lista)return NULL;
	}
	return hash_iter;
}	
	
bool hash_iter_avanzar(hash_iter_t *iter){
	
	if(hash_iter_al_final(iter))return false;
	
	iter->iterados++;
	
	if(lista_iter_al_final(iter->iter_lista)){
		lista_iter_destruir(iter->iter_lista);
		iter->pos++;
		while(lista_esta_vacia(iter->hash->listas[iter->pos])){//while(iter->hash->listas[iter->pos]==NULL){
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
	if(iter->hash->cantidad==0){
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
