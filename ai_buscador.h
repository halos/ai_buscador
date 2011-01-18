#include "eedd/vdin_str.h"

vdin_str ai_buscador_normaliza(vdin_str palabras);

vdin_str ai_buscador_stopper(vdin_str palabras);

vdin_str ai_buscador_stemmer(vdin_str palabras);

float* ai_buscador_similitud(vdin_str consulta, char* fich_ind);

int get_num_docs();

char* vuelca_fich(const char *ruta);

void ai_buscador_escribeResultado(float **s, int num_relevantes, char *c);
