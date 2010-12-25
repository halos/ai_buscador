#include <stdio.h>
#include <string.h>

#include "eedd/vdin_str.h"
#include "ai_buscador.h"

int main(int argc,char** argv)
{

    char *stoplist;

    char query[] = "dell laptop functions";
    vdin_str consulta, vs_free;

    //stoplist = argv[2];
    stoplist = strdup("englishST.txt");

    consulta = split_text(query);

    // TODO: Liberar las cadenas
    // stemmer
    vs_free = consulta;
    consulta = ai_buscador_stemmer(consulta);
    vdin_str_destruye(&vs_free);

    // stopper
    vs_free = consulta;
    consulta = ai_buscador_stopper(consulta);
    vdin_str_destruye(&vs_free);

    // normaliza
    vs_free = consulta;
    consulta = ai_buscador_normaliza(consulta);
    vdin_str_destruye(&vs_free);

    return 0;

}

