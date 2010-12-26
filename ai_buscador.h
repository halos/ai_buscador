#include "eedd/vdin_str.h"

vdin_str ai_buscador_normaliza(vdin_str palabras);

vdin_str ai_buscador_stopper(char* stoplist_path, vdin_str palabras);

vdin_str ai_buscador_stemmer(vdin_str palabras);

float* ai_buscador_similitud(vdin_str consulta);

int get_num_docs();

/*
ai_buscador_escribeResultado
*/
