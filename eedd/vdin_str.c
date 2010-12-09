#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../depuracion.h"
#include "vdin_str.h"

/**
	@brief Representacion del TDA AVL (nodo)
*/
struct vdin_str_rep {
	int tamf;
	int taml;
	char* *v;
};

/**
	@brief Constructor primitivo del vector dinamico
	@return Un nuevo vector dinamico
*/
vdin_str vdin_str_crea() {
	vdin_str vdin;

	if (!(vdin = (vdin_str) malloc(sizeof(struct vdin_str_rep))))
		depuracion("vdin_str_crea: sin memoria", ERR_NO_MEM);
	
	if (!(vdin->v = (char* *) malloc(sizeof(char*))))
		depuracion("vdin_str_crea: sin memoria", ERR_NO_MEM);

	vdin->tamf = 1;
	vdin->taml = 0;
	return vdin;
}

/**
	@brief Destructor primitivo del vector dinamico
	@param vdin Vector dinamico a destruir (pasado por referencia)
	@pre El vector debe haber sido creado previamente
	@post El vector queda apuntando a NULL
*/
void vdin_str_destruye(vdin_str *vdin) {
#ifdef __VDIN_DEBUG_
	if (!*vdin)
		depuracion("vdin_str_destruye: el vector no existe", ERR_NO_EXISTE);
#endif
	free((*vdin)->v);
	free(*vdin);
	*vdin = NULL;
}

/**
	@brief Crea un nuevo vector como copia de uno dado
	@param vdin Vector a copiar
	@return Una copia del vector
	@pre El vector vdin debe haber sido creado previamente
	@pos El vector original no sufre modificacion alguna
*/

vdin_str vdin_str_copia(vdin_str vdin) {
	vdin_str copia;
#ifdef __VDIN_DEBUG_
	if (!vdin)
		depuracion("vdin_str_copia: el vector no existe", ERR_NO_EXISTE);
#endif
	
	if (!(copia = (vdin_str) malloc(sizeof(struct vdin_str_rep))))
		depuracion("vdin_str_copia: sin memoria", ERR_NO_MEM);
	
	if (!(copia->v = (char* *) malloc(sizeof(char*)*vdin->tamf)))
		depuracion("vdin_str_copia: sin memoria", ERR_NO_MEM);

	copia->tamf = vdin->tamf;
	copia->taml = vdin->taml;
	memcpy(copia->v, vdin->v, sizeof(char*)*vdin->tamf);
	return copia;
}

/**
	@brief Aumenta el tama�o del vector, a�adiendo un elemento mas en el limite superior (derecho)
	@param vdin Vector dinamico que queremos aumentar
	@param dato Elemento que queremos a�adir al vector
	@pre El vector debe haber sido creado previamente
*/
void vdin_str_aumd(vdin_str vdin, char* dato) {

#ifdef __VDIN_DEBUG_
	if (!vdin)
		depuracion("vdin_str_aumd: el vector no existe", ERR_DATO_INVALIDO);
#endif

	if (vdin->taml == vdin->tamf) {
		vdin->tamf *= 2;
#ifdef __VDIN_DEBUG_
		if (vdin->tamf < 0)
			depuracion("vdin_str_aumd: tamaño maximo alcanzado", ERR_MAX_TAM);
#endif
		if (!(vdin->v = (char* *) realloc((void*)vdin->v, (long)sizeof(char*)*vdin->tamf)))
			depuracion("vdin_str_aumd: sin memoria", ERR_NO_MEM);
	}
	vdin->v[vdin->taml++] = dato;
}

/**
	@brief Disminuye el tama�o del vector, sacando el elemento mas a la derecha
	@param vdin Vector dinamico que queremos disminuir
	@return El dato que estaba mas a la derecha
	@pre El vector no puede estar vacio
	@pre El vector debe haber sido creado previamente
*/
char* vdin_str_disd(vdin_str vdin) {
	char* viejo;
#ifdef __VDIN_DEBUG_
	if (!vdin)
		depuracion("vdin_str_disd: el vector no existe", ERR_DATO_INVALIDO);
	if (vdin->taml == 0)
		depuracion("vdin_str_disd: vector vacío", ERR_DATO_INVALIDO);
#endif
	viejo = vdin->v[--vdin->taml];
	if (vdin->tamf == vdin->taml*2) {
		vdin->tamf /= 2;
		if (!(vdin->v = (char* *) realloc((void*)vdin->v, (long)sizeof(char*)*vdin->tamf)))
			depuracion("vdin_str_aumd: sin memoria", ERR_NO_MEM);
	}
	return viejo;
}

/**
	@brief Asigna un valor a una posicion del vector
	@param vdin Vector dinamico sobre el que queremos asignar el valor
	@param dato Elemento cuyo valor queremos asignar a una posicion del vector
	@param pos Posicion sobre la que queremos asignar el valor dado
	@pre La posicion del vector debe ser valida (menor a la longitud logica del mismo y positiva)
	@pre El vector debe haber sido creado previamente
*/

void vdin_str_asigna(vdin_str vdin, char* dato, int pos) {
#ifdef __VDIN_DEBUG_
	if (!vdin)
		depuracion("vdin_str_asigna: el vector no existe", ERR_DATO_INVALIDO);
	if (pos >= vdin->taml || pos < 0)
		depuracion("vdin_str_asigna: posicion invalida", ERR_DATO_INVALIDO);
#endif
	vdin->v[pos] = dato;
}

/**
	@brief Obtiene un valor de una posicion del vector
	@param vdin Vector dinamico del que queremos obtener el valor
	@param pos Posicion que queremos acceder
	@return El dato accedido
	@pre La posicion del vector debe ser valida (menor a la longitud logica del mismo y positiva)
	@pre El vector debe haber sido creado previamente
*/

char* vdin_str_obtiene(vdin_str vdin, int pos) {
#ifdef __VDIN_DEBUG_
	if (!vdin)
		depuracion("vdin_str_obtiene: el vector no existe", ERR_DATO_INVALIDO);
	if (pos >= vdin->taml || pos < 0)
		depuracion("vdin_str_obtiene: posicion invalida", ERR_DATO_INVALIDO);
#endif
	return (vdin->v[pos]);
}

/**
	@brief Devuelve el numero de elementos en el vector
	@param vdin Vector dinamico a consultar
	@return El numero de elementos alojados en el vector
	@pre El vector debe haber sido creado previamente
*/

int vdin_str_tama(vdin_str vdin) {
#ifdef __VDIN_DEBUG_
	if (!vdin)
		depuracion("vdin_str_tama: el vector no existe", ERR_DATO_INVALIDO);
#endif
	return vdin->taml;
}
