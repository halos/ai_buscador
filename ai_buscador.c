#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ai_buscador.h"
#include "ai_indice.h"
#include "PorterStemmer.h"
#include "estructuras.h"

#include "eedd/avl_words.h"
#include "eedd/listadin_freqs.h"

/**
 * @brief Normaliza el texto de los documentos indicados
 * @param originales Documentos que se normalizaran
 * @return Vector de documentos normalizados
 */
vdin_str ai_buscador_normaliza(vdin_str palabras){
	
    int i;
    int num_words;
    char* norm_word;
    vdin_str norm_words;

	num_words = vdin_str_tama(palabras);
	norm_words = vdin_str_crea();

	for(i = 0; i < num_words; i++){
		norm_word = normaliza_str(vdin_str_obtiene(palabras, i));
		if(strlen(norm_word))
			vdin_str_aumd(norm_words, norm_word);

	}
    
    return norm_words;
	
}

/**
 * @brief Elimina las palabras vacias de una cadena
 * @param stoplist_path Ruta del fichero con las palabras vacias
 * @param text Cadena de caracteres de donde eliminar las las palabres vacías
 * @return Cadena de caracteres sin las palabras vacias
 */
vdin_str ai_buscador_stopper(char* stoplist_path, vdin_str palabras){

    static vdin_str stopwords = 0;
    vdin_str stopped;
    FILE *stw_file;
    int i, j;
    int tam;
    char *stw_buff;
    char *palabra;

    // Creación del vector de palabras vacías

    if(!stopwords){

        stw_file = fopen(stoplist_path, "r");

        fseek(stw_file, 0, SEEK_END);
        tam = ftell(stw_file);
        fseek(stw_file, 0, SEEK_SET);

        stw_buff = malloc(tam + 1); // \0
        fread(stw_buff, 1, tam, stw_file);
        stw_buff[tam - 1] = 0;

        stopwords = split_text(stw_buff, " ,;.:\r\n\t");

        free(stw_buff);
        fclose(stw_file);

    }

    // Búsqueda de las palabras vacías
    stopped = vdin_str_crea();
    tam = vdin_str_tama(palabras);

    for(i = 0; i < tam; i++){
        palabra = vdin_str_obtiene(palabras, i);

        // Pasar a minuscula las palabras
        for(j = 0; j < strlen(palabra); j++)
            palabra[j] = tolower(palabra[j]);

        if(!is_stop(palabra, stopwords)){
            vdin_str_aumd(stopped, palabra);
        }
    }

    return stopped;

}

/**
 * @brief Extrae la raiz de las palabras
 * @param palabras Vector de palabras a stemmizar
 * @return Cadena de caracteres con la raiz de las palabras
 */
vdin_str ai_buscador_stemmer(vdin_str palabras){

	return stemstring(palabras);
	
}

int compar_word(const word *a, const word *b){

    return strcmp(a->palabra, b->palabra);

}

/*
ai_buscador_similitud
*/

ai_buscador_similitud(){

    FILE* fd;
    int file_size, tam, tam2, i, j;
    int file_id, num_rep;
    char *buff, *palabra;
    vdin_str lineas, linea;
    avl_words words;
    freq_file *new_ff;
    word *new_w;

    // Cargar fichero
    fd = fopen("index.ind", "r");

    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);
    buff = malloc(file_size);
    words = avl_words_crea();

    fclose(fd);
    
    // Parsear buffer
    lineas = split_text(buff, "\r\n");

    tam = vdin_str_tama(lineas);

    for(i = 0; i < tam; i++){

        linea = split_text(vdin_str_obtiene(lineas, i), " ");

        tam2 = vdin_str_tama(linea);
        palabra = vdin_str_obtiene(linea, 0);
        //crear palabra en eedd
        new_w = malloc(sizeof(word));
        new_w->palabra = strdup(palabra);
        new_w->frecuencias = listadin_freqs_crea();

        for(j = 1; j < tam2; j++){

            sscanf(vdin_str_obtiene(linea, i), "%d:%d", &file_id, &num_rep);
            // añadir file_id : num_rep
            new_ff = malloc(sizeof(freq_file));
            new_ff->file = file_id;
            new_ff->freq = num_rep;
            listadin_freqs_insertaFinal(new_w->frecuencias, new_ff);

            avl_words_mete(&words, *new_w, compar_word);

        }


    }

    



}

/*
ai_buscador_escribeResultado
*/
