#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eedd/vdin_str.h"
#include "ai_buscador.h"
#include "ai_indice.h"

int main(int argc,char** argv)
{

    char *query;
    char *fich_ind;
    char *fich_cons;
    int relevantes;
    int i, n_qry;
    float *s;
    vdin_str consulta, consultas;
    vdin_str vs_free;

    if(argc != 4){
        printf("Uso:\n");
        printf("%s <fich_indice> <fich_consultas> <num_relevantes>\n", argv[0]);
        exit(1);
    }

    //fichero indice
    fich_ind = argv[1];
    //fich_ind = strdup("index.ind");

    //fichero consulta
    fich_cons = argv[2];
    //fich_cons = strdup("consultas.txt");

    //num relevantes
    relevantes = atoi(argv[3]);
    //relevantes = 5;

    //Obtener las consultas
    consultas = obtiene_consultas(fich_cons);

    n_qry = vdin_str_tama(consultas);

    for(i = 0; i < n_qry; i++){

        // TODO: Obtener línea de consulta
        query = vdin_str_obtiene(consultas, i);
        consulta = split_text(query, " ");

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

        // busca similitud
        s = ai_buscador_similitud(consulta, fich_ind);

/*
        int i, tam;

        tam = get_num_docs();

        printf("Resultados:\n");

        for(i = 0; i < tam; i++)
            printf("%d --> %f\n",i, s[i]);
*/

        ai_buscador_escribeResultado(s, relevantes, query);

    }

    vdin_str_destruye(&consultas);
    vdin_str_destruye(&consulta);

    return 0;

}

