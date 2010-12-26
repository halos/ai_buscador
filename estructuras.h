/*
 * @file estructuras.h
 * @brief Estructuras del proyecto de Acceso a la Informaci�n
 * @version 1.2.1
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "eedd/vdin_str.h"
#include "eedd/listadin_freqs.h"

struct rep_document{
	char* id; // ID del documento
        int doc_num;
	vdin_str titulo; // Titulo del documento
	vdin_str etiquetas; // Vector de etiquetas
	vdin_str categorias; // Vector de categorias
	char* texto; // Texto del documento original
	vdin_str text_stopped; // Texto sinpalabras vacias
        vdin_str text_stemmed; // Texto con las raices
	//vdin_str text_parsed; // Texto del documento con las raices de cada palabra
};

typedef struct rep_document document;

struct rep_word{
    char *palabra;
    idf_file idfs;
};

typedef struct rep_word word;

#endif /* ESTRUCTURAS_H_ */
