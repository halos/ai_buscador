/*
 * @file ai_indice.h
 * @brief cabeceras de los metodos para la creacion del indice del sistema de recuperacion de informacion
 * @version 1.1
 */

#ifndef AI_INDICE_H_
#define AI_INDICE_H_

#include "eedd/vdin_str.h"

vdin_str ls_files(const char* ruta);

// Cabeceras de funciones publicas

vdin_str ai_indice_stopper(char* stoplist_path, vdin_str text);

vdin_str split_text(const char* str, const char* elim);

char* normaliza_str(char* word);

#endif /* AI_INDICE_H_ */
