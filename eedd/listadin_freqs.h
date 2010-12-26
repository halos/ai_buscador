/**
 * @file listadin_freqs.h
 * @brief Fichero cabecera del TDA lista dinamica de freq_file
 * @date 22/10/2010
 * @version 1.0
*/

#ifndef __LISTADIN_freqs_H
#define __LISTADIN_freqs_H

#include "../estructuras2.h"


/** Comentar para eliminar las comprobaciones de precondiciones en las operaciones del tipo */
#define _LISTADIN_idf_DEBUG

/**
 * @brief TDA lista dinamica
 * @par Descripcion
 *   Definir variables del tipo tlistadin_idfs para trabajar con listas dinamicas
*/

typedef struct tlistadin_idf_rep *listadin_idf;

	listadin_idf listadin_idf_crea();
	void listadin_idf_destruye(listadin_idf *l);
	int listadin_idf_numElem(listadin_idf l);
	void listadin_idf_insertaPos(listadin_idf l, idf_file *elem, int pos);
	void listadin_idf_insertaInicio(listadin_idf l, idf_file *elem);
	void listadin_idf_insertaFinal(listadin_idf l, idf_file *elem);
	void listadin_idf_inserta(listadin_idf l, idf_file *elem);
	void listadin_idf_sacaPos(listadin_idf l, idf_file **elem, int pos);
	void listadin_idf_obtienePos(listadin_idf l, idf_file **elem, int pos);
	int listadin_idf_busca(listadin_idf l, idf_file **elem, int (*fcomp)(const idf_file **elem1, const idf_file **elem2));

	void listadin_idf_iter_ini (listadin_idf l);
	idf_file** listadin_idf_iter_siguiente (listadin_idf l);
	int listadin_idf_iter_fin(listadin_idf l);

#endif
