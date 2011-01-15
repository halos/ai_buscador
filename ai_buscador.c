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
    char *buff;
    vdin_str norm_words;

	num_words = vdin_str_tama(palabras);
	norm_words = vdin_str_crea();

	for(i = 0; i < num_words; i++){
            buff = vdin_str_obtiene(palabras, i);
            norm_word = normaliza_str(buff);
            free(buff);
            
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
vdin_str ai_buscador_stopper(vdin_str palabras){

    static vdin_str stopwords = 0;
    vdin_str stopped;
    int i, j;
    int tam;
    char *stw_buff;
    char *palabra;

    // Creación del vector de palabras vacías

    if(!stopwords){

        stw_buff = vuelca_fich("englishST.txt");

        stopwords = split_text(stw_buff, " ,;.:\r\n\t");

        free(stw_buff);

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

    int tam, tam2, i, j;
    int file_id;
    float peso;
    char *buff, *palabra;
    vdin_str lineas, linea;
    avl_words words;
    idf_file *new_ff;
    word *new_w;

    words = avl_words_crea();

    // Cargar fichero
    buff = vuelca_fich(file_path);

    // Parsear buffer
    lineas = split_text(buff, "\r\n");

    free(buff);

    tam = vdin_str_tama(lineas);

    for(i = 0; i < tam; i++){

        buff = vdin_str_obtiene(lineas, i);

        linea = split_text(buff, " ");

        free(buff);

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

    int num_docs;
    char *buff;
    vdin_str indices;

    buff = vuelca_fich("ids.dat");
    
    indices = split_text(buff, "\r\n");

    free(buff);

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
float* ai_buscador_similitud(vdin_str consulta, char* fich_ind){

    int i, tam, j, tam2;
    avl_words words;
    word palabra;
    idf_file *idff, *idff_aux;
    float *s; // Similitudes
    float w, idfc, tfc;

    // Carga el índice de palabras
    words = gen_eedd(fich_ind);

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
 * @brief Obtiene el texto encerrado dentro del tag
 * @param buff Texto de donde obtener los datos
 * @param tag Tag dinde se encuentra el texto
 * @return Texto que se encuentra dentro del tag o NULL si no pudo obtenerse
 */
char* get_tag_text(char* buff, char* _tag){

    char *ptr_inic, *ptr_fin;
    char *txt;
    char *open_tag, *close_tag;

    txt = 0;
    
    open_tag = malloc(strlen(_tag) + 2 + 1); // < > \0
    close_tag = malloc(strlen(_tag) + 3 + 1); // </ > \0

    *open_tag = 0;
    *close_tag = 0;

    strcat(open_tag, "<");
    strcat(open_tag, _tag);
    strcat(open_tag, ">");

    strcat(close_tag, "</");
    strcat(close_tag, _tag);
    strcat(close_tag, ">");

    // Busca los tags
    ptr_inic = strstr(buff,open_tag);
    ptr_fin = strstr(buff,close_tag);

    if(ptr_inic && ptr_fin){ // Si están los dos

        ptr_inic += strlen(open_tag);

        txt = malloc(ptr_fin - ptr_inic + 1);

        strncpy(txt, ptr_inic, ptr_fin - ptr_inic);

        txt[ptr_fin - ptr_inic] = 0;

    }

    free(open_tag);
    free(close_tag);

    return txt;

}

/**
 * @brief Obtiene el nombre del fichero indicado
 * @param index Índice del fichero
 * @return Nombre del fichero
 */
char* get_nombre_fichero(int index){

    FILE *fd;
    fd = fopen("ids.dat", "r");
    int encontrado = 0;
    char *file_name;
    char *file_path;
    int id_f;

    file_path = malloc(64);
    file_name = malloc(64);

    do{

        fscanf(fd, "%[^|]|%d\n", file_name, &id_f);
        if(id_f == index)
            encontrado = 1;
        
    }while(!encontrado);

    fclose(fd);

    file_name = realloc(file_name, strlen(file_name));

    strcpy(file_path, "../coleccion/");
    strcat(file_path, file_name);
    strcat(file_path, ".sgml");

    file_path = realloc(file_path, strlen(file_path) + 1);
    file_path[strlen(file_path)] = 0;
    free(file_name);

    return file_path;

}

/**
 * @brief Obtiene el título del fichero con el identificador indicado
 * @param index Identificador del fichero
 * @return título del documento
 */
char* get_titulo(int index){

    char *file_name, *titulo, *buff;

    file_name = get_nombre_fichero(index);

    buff = vuelca_fich(file_name);

    titulo = get_tag_text(buff, "TITLE");

    return titulo;

}

/**
 * @brief Obtiene una frase con una palabra de la consulta
 * @param i Índice del archivo donde buscar
 * @param c Consulta
 * @return Frase con una palabra d ela consulta
 */
char* get_frase(int ind, char* c){

    int i, tam, j, tam2, k, tam3;
    vdin_str frases, pals_cons, pals_frase;
    vdin_str frase_stop, frase_stem,frase_pars;
    vdin_str pals_pars, pals_stop, pals_stem;
    char *buff, *file_buff;
    char *frase, *pal_cons, *pal_frase;

    file_buff = vuelca_fich(get_nombre_fichero(ind));

    buff = get_tag_text(file_buff, "TEXT");
    free(file_buff);
    frases = split_text(buff, ".");
    free(buff);

    pals_cons = split_text(c, " ");

    tam = vdin_str_tama(frases);

    pals_stop = ai_buscador_stopper(pals_cons);
    pals_stem = ai_buscador_stemmer(pals_stop);
    pals_pars = ai_buscador_normaliza(pals_stem);
    tam2 = vdin_str_tama(pals_pars);

    for(i = 0; i < tam; i++){ // frases

        frase = vdin_str_obtiene(frases, i);
        pals_frase = split_text(frase, " ,;\n\r");

        frase_stop = ai_buscador_stopper(pals_frase);
        frase_stem = ai_buscador_stemmer(frase_stop);
        frase_pars = ai_buscador_normaliza(frase_stem);

        for(j = 0; j < tam2; j++){ // palabras frase

            pal_frase = vdin_str_obtiene(frase_pars, j);

            for(k = 0; k < tam3; k++){ // palabras consulta
                pal_cons = vdin_str_obtiene(pals_pars, k);

                if(strstr(pal_frase, pal_cons)){
                    return vdin_str_obtiene(pals_cons, k);;
                }
            }

        }

    }

}

/**
 * @brief Vuelca los resultados en el fichero
 * @param docs Vector con los documentos
 * @param s Vector de similitudes
 * @param tam número de resultados
 * @param c Consulta
 */
void write_results(int *docs, float *s, int tam, char *c){

    int i;
    char *file_name, *titulo, *frase;
    FILE *fd;
    fd = fopen("results.dat", "a");

    fprintf(fd, "\"%s\"\n", c);

    for(i = 0; i < tam; i++){

        file_name = get_nombre_fichero(docs[i]);
        titulo = get_titulo(docs[i]);
        frase = get_frase(docs[i], c);

        fprintf(fd, "%d- %s:%f:%s:%s\n",i+1 ,file_name, s[i], titulo, frase);
    }

    fprintf(fd, "\n");

    fclose(fd);

}

/**
 * @brief Parsea y escribe los resultados en el fichero
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
        tam = relevantes;

    }

    // Escribe resultados
    write_results(docs, s2, tam, c);

}

vdin_str obtiene_consultas(char *fich_cons){

    char *buff;
    vdin_str cons;

    buff = vuelca_fich(fich_cons);

    cons = split_text(buff, "\n");

    free(buff);

    return cons;

}

char* vuelca_fich(const char *ruta){

    FILE *fd;
    int tam;
    char *buff;

    fd = fopen(ruta, "r");
    
    fseek(fd, 0, SEEK_END);
    tam = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    buff = malloc(tam + 1); // \0
    fread(buff, 1, tam, fd);
    buff[tam - 1] = 0;

    fclose(fd);

    return buff;

}