#include <stdio.h>
#include <string.h>

#include "eedd/vdin_str.h"
#include "ai_buscador.h"
#include "ai_indice.h"

int main(int argc,char** argv)
{

    char *stoplist;
    float *s;

    char query[] = "dell laptop functions";
    vdin_str consulta, vs_free;

    //stoplist = argv[2];
    stoplist = strdup("englishST.txt");

    consulta = split_text(query, " ");
    
    // TODO: Liberar las cadenas
    // stemmer
    vs_free = consulta;
    consulta = ai_buscador_stemmer(consulta);
    vdin_str_destruye(&vs_free);

    // stopper
    vs_free = consulta;
    consulta = ai_buscador_stopper(stoplist ,consulta);
    vdin_str_destruye(&vs_free);

    // normaliza
    vs_free = consulta;
    consulta = ai_buscador_normaliza(consulta);
    vdin_str_destruye(&vs_free);

    // busca similitud
    s = ai_buscador_similitud(consulta);

    int i, tam;

    tam = get_num_docs();

    for(i = 0; i < tam; i++)
        printf("%d --> %f\n",i, s[i]);

    return 0;

}

