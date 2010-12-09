/*
 * @file ai_indice.h
 * @brief cabeceras de los metodos para la creacion del indice del sistema de recuperacion de informacion
 * @version 1.1
 */

#ifndef AI_INDICE_H_
#define AI_INDICE_H_

#include "eedd/vdin_str.h"
#include "eedd/vdin_documentos.h"
#include "estructuras.h"

vdin_str ls_files(const char* ruta);

// Cabeceras de funciones publicas
vdin_documentos ai_indice_read(char *directorio);

vdin_documentos ai_indice_normaliza(vdin_documentos originales);

vdin_str ai_indice_stopper(char* stoplist_path, vdin_str text);

void ai_calc_idf(vdin_documentos docs);

vdin_str split_text(const char* str);

void indexa_docs(vdin_documentos docs);

#endif /* AI_INDICE_H_ */
