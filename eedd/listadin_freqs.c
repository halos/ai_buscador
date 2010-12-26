/**
 * @file listadin_freqs.c
 * @brief Implementación del TDA lista dinámica de enteros
 * @date 22/10/2010
 * @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "listadin_freqs.h"
#include "../depuracion.h"


 /**
  * @brief Representación del TDA lista dinámica
  * @par Descripcion
  *   Cabecera para el TDA lista dinámica
*/
struct tlistadin_idf_rep {
  int longitud;	/**< Número de nodos introducidos */
  struct tlistadin_idf_nodo *inicio; /**< dirección al primer elemento de la lista*/
  struct tlistadin_idf_nodo *fin;    /**< dirección al ultimo elemento de la lista*/
  struct tlistadin_idf_nodo *iterador; /**< elemento actual de iteración*/
};


/**
 * @brief Representaci�n del nodo para el TDA lista din�mica
 * @par Descripcion
 * 	  Nodo para el TDA lista dinamica
*/

struct tlistadin_idf_nodo {
  idf_file *dato; /** almacena un dato */
  struct tlistadin_idf_nodo *sig;  /** direcci�n del siguiente elemento de la lista */
};


/**
 * @brief Tipo Nodo para el TDA lista din�mica
 * @par Descripcion
	  Definir variables del tipo tnodo_freqs para trabajar con nodos de las listas din�micas
*/
typedef struct tlistadin_idf_nodo *tnodo_freqs;


/**
 * @brief Constructor primitivo
 * @pre Que haya memoria suficiente para crear la estructura
 * @post se crea la lista din�mica vac�a
 * @return una lista din�mica vac�a
*/
listadin_idf listadin_idf_crea() {
  listadin_idf l;

  if (!(l=(listadin_idf)malloc(sizeof(struct tlistadin_idf_rep))))
    depuracion("lista_freqs_nueva: No hay memoria.\n", ERR_NO_MEM);

  l->inicio=l->fin=l->iterador=NULL;
  l->longitud=0;

  return l;
}

/**
 * @brief Destructor
 * @param *l lista dinamica
 * @pre la lista tiene que existir
 * @post se libera la memoria de la lista din�mica
*/
void listadin_idf_destruye(listadin_idf *l) {

	tnodo_freqs viejo;

  while(viejo=((*l)->inicio)) {
    (*l)->inicio=viejo->sig;
    free(viejo);
  }
  free(*l);
}

/**
 * @brief  Observador para contar los elementos de la lista
 * @param l lista dinamica
 * @pre l exista
 * @post Devuelve el n�mero de elementos de la lista
*/
int listadin_idf_numElem(listadin_idf l) {
	return l->longitud;
}

/**
 * @brief Modificador para insertar un dato en una posicion dada
 * @param l lista dinamica
 * @param elem dato a insertar en la lista
 * @param pos posicion donde insertar el dato
 * @pre la lista exista
 * @pre la posici�n a insertar sea valida
 * @pre memoria suficiente para crear el nuevo nodo
 * @post creamos el nuevo nodo e insertamos en la posicion indicada
*/


void listadin_idf_insertaPos(listadin_idf l, idf_file *elem, int pos) {

	tnodo_freqs nuevo, act,ant;

	int i;

#ifdef _LISTADIN_idf_DEBUG

  if (!l)
    depuracion("lista_freqs_insertaPos: La lista no existe.\n",ERR_NO_EXISTE);
  if (pos<0 || pos>l->longitud)
    depuracion("lista_freqs_insertaPos: La posicion introducida no es valida.\n",ERR_NO_EXISTE);
#endif

  if(!(nuevo=(tnodo_freqs)malloc(sizeof(struct tlistadin_idf_nodo))))
    depuracion("lista_freqs_insertaPos: No hay memoria suficiente.\n",ERR_NO_MEM);

  nuevo->dato=elem;

  if (!l->inicio && pos==0) { // Es el primer valor
   nuevo->sig=NULL;
   l->inicio=l->fin=nuevo;
  } else {
    ant=NULL;
    act=l->inicio;
	for(i=0;i<pos;i++) {
	  ant=act;
	  act=act->sig;
	}
	nuevo->sig=act;

 	if (pos==0)
	  l->inicio=nuevo;
  	else {
	  ant->sig=nuevo;
	  if (pos==l->longitud)
	    l->fin=nuevo;
	}
  }

  l->longitud++;
  l->iterador=NULL;
}

/**
 * @brief Modificador para insertar un dato al principio de la lista
 * @param l lista din�mica
 * @param elem dato a insertar en la lista
 * @pre la lista exista
 * @pre memoria suficiente para crear el nuevo nodo
 * @post creamos el nuevo nodo e insertamos en la posici�n inicial
*/
void listadin_idf_insertaInicio(listadin_idf l, idf_file *elem) {
  tnodo_freqs nuevo, act;

#ifdef _LISTADIN_idf_DEBUG

  if (!l)
    depuracion("lista_freqs_insertaInicio: La lista no existe.\n",ERR_NO_EXISTE);
#endif

  if(!(nuevo=(tnodo_freqs)malloc(sizeof(struct tlistadin_idf_nodo))))
    depuracion("lista_freqs_insertaInicio: No hay memoria suficiente.\n",ERR_NO_MEM);

  nuevo->dato=elem;

  if (!l->inicio) { // Es el primer valor
   nuevo->sig=NULL;
   l->inicio=l->fin=nuevo;
  } else {
    act=l->inicio;
	nuevo->sig=act;
    l->inicio=nuevo;
  }

  l->longitud++;
  l->iterador=NULL;
}

/**
 * @brief Modificador para insertar un dato en la ultima posicion
 * @param l lista dinamica
 * @param elem dato a insertar en la lista
 * @pre la lista exista
 * @pre memoria suficiente para crear el nuevo nodo
 * @post creamos el nuevo nodo e insertamos en la �ltima posici�n
*/
void listadin_idf_insertaFinal(listadin_idf l, idf_file *elem) {
  tnodo_freqs nuevo, act;

#ifdef _LISTADIN_idf_DEBUG

  if (!l)
    depuracion("lista_freqs_insertaFinal: La lista no existe.\n",ERR_NO_EXISTE);
#endif

  if(!(nuevo=(tnodo_freqs)malloc(sizeof(struct tlistadin_idf_nodo))))
    depuracion("lista_freqs_insertaFinal: No hay memoria suficiente.\n",ERR_NO_MEM);

  nuevo->dato=elem;
  nuevo->sig=NULL;

  if (!l->inicio) { // Es el primer valor
	  l->inicio=l->fin=nuevo;
  } else {
    act=l->fin;
	act->sig=nuevo;
	l->fin=nuevo;
  }

  l->longitud++;
  l->iterador=NULL;
}

/**
 * @brief Modificador para insertar un dato una posicion predefinida (la �ltima)
 * @param l lista dinamica
 * @param elem dato a insertar en la lista
 * @pre la lista exista
 * @pre memoria suficiente para crear el nuevo nodo
 * @post creamos el nuevo nodo e insertamos en la posici�n predefinida
*/
void listadin_idf_inserta(listadin_idf l, idf_file *elem) {
  // Inserto al final
  listadin_idf_insertaFinal(l, elem);
}

/**
 * @brief  Modificador para sacar elementos de una lista dinamica de una posicion dada
 * @param l lista dinamica
 * @param elem dato donde extraer el dato de la lista
 * @param pos posicion donde extraer el dato
 * @pre l exista
 * @pre la posicion sea valida
 * @pre la lista no este vacia
 * @post sacamos el valor indicado y liberamos la memoria de ese nodo de la lista
*/
void listadin_idf_sacaPos(listadin_idf l, idf_file **elem, int pos) {
  tnodo_freqs viejo, act, ant;
  int i;

#ifdef _LISTADIN_idf_DEBUG

  if (!l)
    depuracion("lista_freqs_sacaPos: La lista no existe.\n",ERR_NO_EXISTE);
  if (pos<0 || pos>l->longitud)
    depuracion("lista_freqs_sacaPos: La posicion introducida no es valida.\n",ERR_NO_EXISTE);
  if (!l->inicio)
    depuracion("lista_freqs_sacaPos: La lista esta vacia.\n",ERR_NO_MEM);
#endif

  if (l->inicio==l->fin && pos==0) {
    viejo=l->inicio;
    l->inicio=l->fin=NULL;
  } else {
    ant=NULL;
    act=l->inicio;
    for(i=0;i<pos;i++) {
	  ant=act;
	  act=act->sig;
	}
	viejo=act;

	if (viejo==l->inicio)
	  l->inicio=viejo->sig;
	else {
	  ant->sig=viejo->sig;
	  if (l->fin==viejo)
	    l->fin=ant;
	}
  }

  *elem=viejo->dato;
  free(viejo);

  l->longitud--;
  l->iterador=NULL;
}

/**
 * @brief  Obtiene el elemento en una lista dinamica, de la posicion indicada
 * @param l lista dinamica
 * @param elem dato donde extraer el dato de la lista
 * @param pos posicion donde extraer el dato
 * @pre l que la lista exista
 * @pre la posicion sea valida
 * @pre la lista no este vacia
 * @post sacamos el valor indicado sin borrarlo de la lista
*/
void listadin_idf_obtienePos(listadin_idf l, idf_file **elem, int pos) {
  tnodo_freqs act, ant;
  int i;

#ifdef _LISTADIN_idf_DEBUG

  if (!l)
    depuracion("lista_freqs_obtienePos: La lista no existe.\n",ERR_NO_EXISTE);
  if (pos<0 || pos>l->longitud)
    depuracion("lista_freqs_obtienePos: La posicion introducida no es valida.\n",ERR_NO_EXISTE);
  if (!l->inicio)
    depuracion("lista_freqs_obtienePos: La lista esta vacia.\n",ERR_NO_MEM);
#endif

  if (l->inicio==l->fin && pos==0) {
    *elem=l->inicio->dato;
  } else {
    ant=NULL;
    act=l->inicio;
    for(i=0;i<pos;i++) {
	  ant=act;
	  act=act->sig;
	}
    *elem=act->dato;
  }
  l->iterador=NULL;
}

/**
 * @brief Observador para saber si un dato esta dentro de la lista
 * @param l lista dinamica
 * @param elem dato a buscar dentro de la lista
 * @param int (*comparar) (const void*, const void*) funcion para comparar elementos de la lista
 * @pre la lista din�mica debe existir
 * @post recorreremos la lista en busca del dato en cuestion
 * @return un entero con la posicion del dato en la lista, -1 si no existe, y en elem el propio dato copiado
*/
int listadin_idf_busca(listadin_idf l, idf_file **elem, int (*fcomp)(const idf_file **elem1, const idf_file **elem2)) {
  tnodo_freqs act, ant;
  int i;

#ifdef _LISTADIN_idf_DEBUG

  if (!l)
   depuracion("listadin_freqs_busca: La lista no existe.\n",ERR_DATO_INVALIDO);
#endif

  ant=NULL;
  act=l->inicio;
  i=0;
  while (act && fcomp((const idf_file**)elem,(const idf_file**)&act->dato)!=0) {
    ant=act;
    act=act->sig;
    i++;
  }

  if (!act) return -1;
  else {
  	listadin_idf_obtienePos(l, elem, i);
  	return i;
  }
}

//-------------------------------------------------
//primitivas de iteraci�n sobre la lista
//-------------------------------------------------

/**@brief Inicializa la iteraci�n sobre los elementos de la lista*/
void listadin_idf_iter_ini (listadin_idf l) {
	l->iterador=l->inicio;

}

/**@brief Devuelve la direcci�n de memoria del siguiente dato en la lista*/


idf_file** listadin_idf_iter_siguiente (listadin_idf l) {

	tnodo_freqs act;

	if (l->iterador==NULL)
		depuracion("listadin_freqs_iter_siguiente: ininto de sobrepasar final de la lista",ERR_NO_EXISTE);

	act=l->iterador;
	l->iterador=l->iterador->sig;
	return (&(act->dato));
}

/**@brief Devuelve 0 si quedan elementos por iterar. */
int listadin_idf_iter_fin(listadin_idf l) {
	return (l->iterador==NULL);
}

