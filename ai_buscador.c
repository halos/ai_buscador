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

/**
 * @brief Función para comparar dos 'word'
 * @param a
 * @param b
 * @return 0 si son iguales, < 0 si b > a, > 0 en caso contrario
 */
int compar_word(const word *a, const word *b){

    return strcmp(a->palabra, b->palabra);

}

/**
 * @brief Genera el índice con los pesos a partir del fichero
 * @param file_path Ruta del fichero índice
 * @return Estructura de datos del índice
 */
avl_words gen_eedd(const char* file_path){

    FILE* fd;
    int file_size, tam, tam2, i, j;
    int file_id;
    float peso;
    char *buff, *palabra;
    vdin_str lineas, linea;
    avl_words words;
    peso_file *new_ff;
    word *new_w;

    // Cargar fichero
    fd = fopen(file_path, "r");

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
        new_w->pesos = listadin_pesos_crea();
        avl_words_mete(words, new_w, compar_word);

        for(j = 1; j < tam2; j++){

            sscanf(vdin_str_obtiene(linea, i), "%d:%f", &file_id, &peso);
            // añadir file_id : peso
            new_ff = malloc(sizeof(peso_file));
            new_ff->file = file_id;
            new_ff->peso = peso;
            listadin_pesos_insertaFinal(new_w->pesos, new_ff);

        }
    }

    return words;

}

int compar_freqs(const peso_file **a, const peso_file **b){

    return (*a)->file - (*b)->file;

}

/**
 * @brief Camcula el vector de similitudes
 * @param consulta Consulta
 * @return Vector de similitudes
 */
float* ai_buscador_similitud(vdin_str consulta){

    int i, tam, j, tam2;
    avl_words words;
    word palabra;
    vdin_str indices;
    peso_file *ff;
    float *s; // Similitudes
    float w, idfc, tfc;
    char* buff;
    FILE* fd;


    // Carga el índice de palabras
    words = gen_eedd("index.ind");

    // Calcula similitudes

    fd = fopen("index.ind", "r");
    fseek(fd, 0, SEEK_END);
    buff = malloc(ftell(fd));
    fclose(fd);
    indices = split_text(buff, "\r\n");

    tam = vdin_str_tama(indices);
    s = calloc(tam, sizeof(float));

    free(buff);
    vdin_str_destruye(indices);

    tam2 = vdin_str_tama(consulta);
    ff = malloc(sizeof(peso_file));

    for(i = 0; i < tam; i++){ // Docs

        for(j = 0; j < tam2; j++){ // Palabras consulta

            // TODO: Contar veces que se repite la palabra
            idfc = 1;
            palabra.palabra = vdin_str_obtiene(consulta, j);

            // comprobar si la palabra está en el árbol
            if(avl_words_busca(palabra, compar_word){
            //coger la palabra!!!!!!

                ff->file = i;

                //comprobar si el documento está en la lista
                if(listadin_pesos_busca(palabra.pesos, &ff, compar_freqs())){
                    //coger lista!!!!!!
                    tfc = ff->peso;
                    w = tfc * idfc; // Tfc * Idfc

                    s[i] += w;

                }

            }

        }

    }

}

/*
ai_buscador_escribeResultado
*/
