#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ai_buscador.h"
#include "ai_indice.h"
#include "PorterStemmer.h"
#include "estructuras.h"
#include "qs.h"

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
    idf_file *new_ff;
    word *new_w;

    // Cargar fichero
    fd = fopen(file_path, "r");

    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    buff = malloc(file_size);
    words = avl_words_crea();
    fread(buff, file_size, 1, fd);

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
        new_w->idfs = listadin_idf_crea();
        avl_words_mete(&words, *new_w, compar_word);

        for(j = 1; j < tam2; j++){

            sscanf(vdin_str_obtiene(linea, j), "%d:%f", &file_id, &peso);
            // añadir file_id : peso
            new_ff = malloc(sizeof(listadin_idf));
            new_ff->file = file_id;
            new_ff->idf = peso;
            listadin_idf_insertaFinal(new_w->idfs, new_ff);

        }
    }

    return words;

}

/**
 * @brief Obtiene el número de documentos de la colección
 * @return Número de documenos de la colección
 */
int get_num_docs(){

    int file_size, num_docs;
    char *buff;
    FILE *fd;
    vdin_str indices;

    fd = fopen("ids.dat", "r");
    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    buff = malloc(file_size);
    fread(buff, file_size, 1, fd);
    fclose(fd);
    
    indices = split_text(buff, "\r\n");

    num_docs = vdin_str_tama(indices);

    vdin_str_destruye(&indices);

    return num_docs;

}

int compar_idf_files(const idf_file **a, const idf_file **b){

    return (*a)->file - (*b)->file;

}

/**
 * @brief Camcula el vector de similitudes
 * @param consulta Consulta
 * @return Vector de similitudes
 */
float* ai_buscador_similitud(vdin_str consulta){

    int i, tam, j, tam2;
    int file_size;
    avl_words words;
    word palabra;
    vdin_str indices;
    idf_file *idff, *idff_aux;
    float *s; // Similitudes
    float w, idfc, tfc;
    FILE* fd;

    // Carga el índice de palabras
    words = gen_eedd("index.ind");

    // Calcula similitudes

    tam = get_num_docs();
    s = calloc(tam, sizeof(float));

    //free(buff);
    

    tam2 = vdin_str_tama(consulta);
    idff = malloc(sizeof(idf_file));
    idff_aux = idff;

    for(i = 0; i < tam; i++){ // Docs

        for(j = 0; j < tam2; j++){ // Palabras consulta

            // TODO: Contar veces que se repite la palabra
            idfc = 1;
            palabra.palabra = vdin_str_obtiene(consulta, j);

            // comprobar si la palabra está en el árbol
            if(avl_words_busca(words, &palabra, compar_word)){

                idff->file = i;

                //comprobar si el documento está en la lista
                if(listadin_idf_busca(palabra.idfs, &idff, compar_idf_files)){
                    //coger lista!!!!!!
                    tfc = idff->idf;
                    w = tfc * idfc; // Tfc * Idfc

                    s[i] += w;

                }

                idff = idff_aux;

            }

        }

    }

    free(idff_aux);

    return s;

}

/**
 * @brief
 * @param s Vector de similitudes
 * @param relevantes Cantidad de elementos relevantes
 */
void ai_buscador_escribeResultado(float *s, int relevantes, char *c){

    int i, tam, j;
    int *docs;
    float *s2; // vector de similitudes sin los ceros

    // Eliminación de elementos con similitud 0
    tam = get_num_docs();
    docs = calloc(tam, sizeof(int));
    s2 = calloc(tam, sizeof(float));

    j = 0;

    for(i = 0; i < tam; i++)
        if(s[i] > 0){

            s2[j] = s[i];
            docs[j] = i;
            j++;

        }

    tam = j;

    s2 = realloc(s2, tam * sizeof(float));
    docs = realloc(docs, tam * sizeof(int));

    // ordena
    quicksort(s2, docs, 0, tam - 1);

    //realloc si procede
    if(tam > relevantes){
        s2 = realloc(s2, relevantes * sizeof(float));
        docs = realloc(docs, relevantes * sizeof(int));

    }

    // Escribe resultados
    write_results(docs, s, tam, c);

}

void write_results(int *docs, float *s, int tam, char *c){

    int i, tam;
    char *file_name, titulo, frase;
    FILE *fd;
    fopen("results.dat", "a");

    fprintf(fd, "\"%s\"\n", c);

    for(i = 0; i < tam; i++){
        // get nombre fichero
        // get titulo
        // get frase
        fprintf(fd, "%d- %s:%f:%s:%s\n",i+1 ,file_name, s[i], titulo, frase);
    }

    fprintf(fd, "\n");

    fclose(fd);

}