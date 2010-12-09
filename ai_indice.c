/*
 * @file ai_indice.c
 * @brief metodos para la creacion del indice del sistema de recuperacion de informacion
 * @version 1.4.5
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "estructuras.h"
#include "ai_indice.h"
#include "eedd/avl_words.h"
#include "eedd/listadin_freqs.h"

document* document_crea(){
    document *d;

    d = malloc(sizeof(document));

    // inicialización del documento
    d->id = 0;
    d->titulo = 0;
    d->etiquetas = 0;
    d->categorias = 0;
    d->texto = 0;
    d->text_stemmed = 0;
    d->text_stopped = 0;
/*
    d->text_parsed = 0;
*/

}

/**
 * @brief Aniade un campo al documento segun el tag
 * @param doc Documento para modificar
 * @param field Campo donde almacenar el contenido
 * @param text Texto que almacenar
 */
void document_add_field(document* doc ,char* field,char* text){

    if(strcmp(field,"ID") == 0){
            doc->id = malloc(strlen(text) + 1);
            strncpy(doc->id, text, strlen(text) + 1);
    }
    else{
        if(strcmp(field,"TEXT") == 0){
            doc->texto = malloc(strlen(text) + 1);
            strncpy(doc->texto, text, strlen(text) + 1);
        }
        else{
            if(strcmp(field,"CATEGORIES") == 0){
                doc->categorias = split_text(text);
            }
            else{

                if(strcmp(field,"TAGS") == 0){
                    doc->etiquetas = split_text(text);
                }
                else{
                    if(strcmp(field,"TITLE") == 0){
                        doc->titulo = split_text(text);
                    }
                    else{
                        fprintf(stderr, "Tag no reconocido: %s\n", field);
                    }

                }

            }

        }

    }

}

/**
 * @brief Lista los archivos de un directorio
 * @param Ruta de la carpeta a listar
 * @return Vector de cadenas con los nombres de los archivos
 */
vdin_str ls_files(const char* _ruta){

    DIR *dir;
    struct dirent *entrada;
    struct stat estado;
    int asd;
    vdin_str rutas;
    char* filename;
    char ruta[256],slash[2]="/";

    strncpy(ruta,_ruta,256);

    if(_ruta[strlen(_ruta)-1] != '/')
            strcat(ruta,slash);

    rutas = vdin_str_crea();
    dir = opendir(ruta);
	
    while(entrada = readdir(dir)){

        filename = malloc(strlen(ruta) + strlen(entrada->d_name) + 1);

        *filename = 0;

        strncpy(filename,ruta,strlen(ruta) + 1); // Carpeta
        strncat(filename,entrada->d_name,strlen(entrada->d_name) + 1); //Archivo

        stat(filename, &estado);

        if((estado.st_mode & S_IFMT) != S_IFDIR) // Si no es un directorio
            vdin_str_aumd(rutas,filename);
    }

    closedir(dir);

    return rutas;

}

/**
 * @brief Separa las cadenas de texto cuando hay ',' ';' '.' ':' '\n' '\t'
 * @param str Cadena que separar
 * @return vdin_str con las palabras separadas
 * @post Las palabras devueltas tienen su memoria reservada
 */
vdin_str split_text(const char* str){

    char *ptr;
    char *word;
    char *copia_str;
    char elim[] = " ,;.:\r\n\t";
    vdin_str separadas;

    separadas = vdin_str_crea();

    copia_str = malloc(strlen(str) + 1);
    strncpy(copia_str, str, strlen(str) + 1);
    ptr = strtok(copia_str,elim);

    while(ptr){

        word = malloc(strlen(ptr) + 1);
        strncpy(word,ptr,strlen(ptr) + 1);
        vdin_str_aumd(separadas,word);
        ptr = strtok(NULL,elim);

    }

    free(copia_str);

    return separadas;

}

/**
 * @brief Obtiene el texto encerrado dentro del tag
 * @param buff Texto de donde obtener los datos
 * @param tag Tag dinde se encuentra el texto
 * @return Texto que se encuentra dentro del tag o  NULL si no pudo obtenerse
 */
char* get_tag_text(char* buff, char* _tag){

    char *ptr_inic, *ptr_fin;
    char *txt = 0;
    char *open_tag, *close_tag;

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
 * @brief Parsea un documento extrayendo su informacion
 * @param Ruta del archivo que contiene el documento
 * @return Puntero al documento, NULL en caso de fallo
 */
document* parse_file(const char* ruta){

    document* doc;
    FILE* fd;
    char *buff, *txt;
    int tamf;
	
    //printf("Parseando: %s\n",ruta);
	
    fd = fopen(ruta, "r");
    doc = NULL;

    if (fd != NULL){

        fseek(fd, 0, SEEK_END);
        tamf = ftell(fd);
        buff = malloc(tamf+1);
        fseek(fd, 0, SEEK_SET);
		
        fread(buff,1,tamf,fd);

        if(strstr(buff, "<DOC>")){

            doc = document_crea();

            // id
            if(txt = get_tag_text(buff, "ID")){
                document_add_field(doc, "ID", txt);
                free(txt);
            }

            // titulo
            if(txt = get_tag_text(buff, "TITLE")){
                document_add_field(doc, "TITLE", txt);
                free(txt);
            }

            // etiquetas
            if(txt = get_tag_text(buff, "TAGS")){
                document_add_field(doc, "TAGS", txt);
                free(txt);
            }
            
            // categorias
            if(txt = get_tag_text(buff, "CATEGORIES")){
                document_add_field(doc, "CATEGORIES", txt);
                free(txt);
            }
            
            // texto
            if(txt = get_tag_text(buff, "TEXT")){
                document_add_field(doc, "TEXT", txt);
                free(txt);
            }

            fclose(fd);
        }
        else{
            fprintf(stderr, "%s : failed to parse\n", ruta);
            fclose(fd);
            return 0;
        }

    }
    else{
        fprintf(stderr, "Unable to open %s\n", ruta);
        return 0;
    }
	
    return doc;

}

/**
 * @brief Metodo que lee los documentos de un directorio
 * @par directorio Ruta donde se encuentran los archivos a indexar
 */
vdin_documentos ai_indice_read(char *directorio){

	int tam_v,i;
	char* ruta;

	document* doc;
	vdin_documentos docs;
	vdin_str rutas;

	docs = vdin_documentos_crea();

	rutas = ls_files((const char*)directorio);

	tam_v = vdin_str_tama(rutas);

	for(i = 0; i < tam_v; i++){ //Para cada documento

		ruta = vdin_str_obtiene(rutas, i);
		if (doc = parse_file(ruta)){
                    doc->doc_num = i;
                    vdin_documentos_aumd(docs, *doc);
                }

	}

	return docs;
}

/**
 * @brief Indica si el caracter es alfa-numerico
 * @param letra Letra para chequear
 * @return 0 si no es alfa-numerico, 1 en caso contrario
 */
int is_alpha_num(char letra){

    //48-57 [0-9]
    if((letra >= 48) && (letra <=57))
        return 1;

    //65-90 [A-Z]
    if((letra >= 65) && (letra <=90))
        return 1;

    //97-122 [a-z]
    if((letra >= 97) && (letra <=122))
        return 1;

    return 0;

}

/**
 * @brief Normaliza una cadena
 * @param word Palabra para normalizar
 * @return Cadena normalizada
 */
char* normaliza_str(char* word){

    int i, j;
    int tam;
    char* new_word;

    tam = strlen(word);
    new_word = malloc(tam);

    j = 0;

    for(i = 0; i < tam; i++)
        if(is_alpha_num(word[i]))
            new_word[j++] = word[i];

    new_word[j] = 0;

    realloc(new_word,strlen(new_word));

    return new_word;

}

/**
 * @brief Normaliza el texto de los documentos indicados
 * @param originales Documentos que se normalizaran
 * @return Vector de documentos normalizados
 */
vdin_documentos ai_indice_normaliza(vdin_documentos originales){
	
    int i, j;
    int tam, num_words;
    char* norm_word;
    document doc, *new_doc;
    vdin_documentos vnorm;

    vnorm = vdin_documentos_crea();

    tam = vdin_documentos_tama(originales);

    for(i = 0; i < tam; i++){

        doc = vdin_documentos_obtiene(originales,i);

        new_doc = document_crea();


        //doc_num
        new_doc->doc_num = doc.doc_num;

        //id
        if(doc.id){
            new_doc->id = doc.id;
        }

        //titulo
        if(doc.titulo){
            num_words = vdin_str_tama(doc.titulo);
            new_doc->titulo = vdin_str_crea();

            for(j = 0; j < num_words; j++){
                norm_word = normaliza_str(vdin_str_obtiene(doc.titulo, j));
                if(strlen(norm_word))
                    vdin_str_aumd(new_doc->titulo, norm_word);

            }
        }

        //etiquetas
        if(doc.etiquetas){
            num_words = vdin_str_tama(doc.etiquetas);
            new_doc->etiquetas = vdin_str_crea();

            for(j = 0; j < num_words; j++){
                norm_word = normaliza_str(vdin_str_obtiene(doc.etiquetas, j));
                if(strlen(norm_word))
                    vdin_str_aumd(new_doc->etiquetas, norm_word);

            }
        }

        //categorias
        if(doc.categorias){
            num_words = vdin_str_tama(doc.categorias);
            new_doc->categorias = vdin_str_crea();

            for(j = 0; j < num_words; j++){
                norm_word = normaliza_str(vdin_str_obtiene(doc.categorias, j));
                if(strlen(norm_word))
                    vdin_str_aumd(new_doc->categorias, norm_word);

            }
        }

        //text_parsed
        if(doc.text_stemmed){
            num_words = vdin_str_tama(doc.text_stemmed);
            new_doc->text_stemmed = vdin_str_crea();

            for(j = 0; j < num_words; j++){
                norm_word = normaliza_str(vdin_str_obtiene(doc.text_stemmed, j));
                if(strlen(norm_word))
                    vdin_str_aumd(new_doc->text_stemmed, norm_word);

            }
        }

        vdin_documentos_aumd(vnorm, *new_doc);

    }

    return vnorm;
	
}

/**
 * Indica si un apalabra es vacía
 * @param palabra Palabra que se quiere comprobar
 * @param stopwords Vector de palabras vacías
 * @return 0 si no es una palabra vacía, 1 si es una palabra vacía
 */
int is_stop(char *palabra, vdin_str stopwords){

    int i;
    int tam;

    tam = vdin_str_tama(stopwords);

    for(i = 0; i < tam; i++)
        if(!strcmp(palabra, vdin_str_obtiene(stopwords, i)))
            return 1;

    return 0;

}

/**
 * @brief Elimina las palabras vacias de una cadena
 * @param stoplist_path Ruta del fichero con las palabras vacias
 * @param text Cadena de caracteres de donde eliminar las las palabres vacías
 * @return Cadena de caracteres sin las palabras vacias
 */
vdin_str ai_indice_stopper(char* stoplist_path, vdin_str palabras){

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

        stopwords = split_text(stw_buff);

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
 * @brief Genera un archivo ids.dat con el indice
 * @param docs Documentos a los que se le hace el indice
 * @post id|num_doc
 */
void indexa_docs(vdin_documentos docs){

    int i, tam;
    FILE *fd;
    char *str;
    document d;

    fd = fopen("ids.dat", "w");
    tam = vdin_documentos_tama(docs);
    str = malloc(64);

    for(i = 0; i < tam; i++){

        d = vdin_documentos_obtiene(docs, i);
        sprintf(str, "%s|%d\n", d.id, d.doc_num);
        fwrite(str, 1, strlen(str), fd);

    }

    fclose(fd);

}

int compar_word(const word *a, const word *b){

    return strcmp(a->palabra, b->palabra);

}

int compar_freqs(const freq_file **a, const freq_file **b){

    return (*a)->file - (*b)->file;

}

void ai_calc_idf(vdin_documentos docs){

    avl_words words;
    freq_file *ff, *ff_temp, *new_ff;
    word busq, *new_w;
    document *d;
    int i, tam;
    int j, tam2;

    words = avl_words_crea();
    d = malloc(sizeof(document));
    ff = malloc(sizeof(freq_file));
    ff_temp = ff;

    // Generar la EE.DD.
    tam = vdin_documentos_tama(docs);


    for(i = 0; i < tam; i++){ // Itera sobre los documentos

        *d = vdin_documentos_obtiene(docs, i);

        tam2 = vdin_str_tama(d->text_stemmed);

        for(j = 0; j < tam2; j++){ // Itera sobre las palabras

            busq.palabra = vdin_str_obtiene(d->text_stemmed, j);

            if(avl_words_busca(words, &busq, compar_word)){// existe la palabra
                
                ff->file = d->doc_num;

                if(listadin_freqs_busca(busq.frecuencias, &ff, compar_freqs) != -1){ // el documento está en la lista

                    ff->freq++;
                    ff = ff_temp;

                }
                else{ // el documento no está en la lista

                    new_ff = malloc(sizeof(freq_file));
                    new_ff->file = d->doc_num;
                    new_ff->freq = 1;
                    listadin_freqs_insertaFinal(busq.frecuencias, new_ff);

                }

            }
            else{ // no existe la palabra

                new_w = malloc(sizeof(word));
                new_w->palabra = strdup(busq.palabra);
                new_w->frecuencias = listadin_freqs_crea();

                new_ff = malloc(sizeof(freq_file));
                new_ff->file = d->doc_num;
                new_ff->freq = 1;
                listadin_freqs_insertaFinal(new_w->frecuencias, new_ff);

                avl_words_mete(&words, *new_w, compar_word);

            }

        }

    }

    free(d);
    free(ff_temp);

    // Realiza los cálculos
    double idf, norma, aux_norma, peso;
    int num_words, num_files;
    FILE *fd;
    
    num_words = vdin_documentos_tama(docs);
    fd = fopen("index.ind", "w");

    while(!avl_words_vacio(words)){

        //Calcular idf palabra
        avl_words_raiz(words, &busq);
        avl_words_saca(&words, &busq, compar_word);
        //TODO: Liberar emoria
        
        idf = log2((double)num_words / listadin_freqs_numElem(busq.frecuencias));

        //Calcular la Norma
        num_files = listadin_freqs_numElem(busq.frecuencias);
        norma = 0;
        listadin_freqs_iter_ini (busq.frecuencias);

        for(i = 0; i < num_files; i++){
            aux_norma = (*listadin_freqs_iter_siguiente(busq.frecuencias))->freq * idf;
            norma += aux_norma * aux_norma;
        }

        norma = sqrt(norma);

        //para cada fichero lo dividimos entre la norma
        listadin_freqs_iter_ini (busq.frecuencias);

        // escribir "<palabra>"
        fwrite(busq.palabra, 1, strlen(busq.palabra), fd);

        for(i = 0; i < num_files; i++){

            ff_temp = (*listadin_freqs_iter_siguiente(busq.frecuencias));
            peso = ff_temp->freq * idf / norma;

            //escribir en el fichero "<espacio><id_doc>:peso"
            fprintf(fd, " %d:%f", ff_temp->file, peso);

        }

        // escribir '\n'
        fwrite("\n", 1, 1, fd);

    }

    fclose(fd);
    
    
    
}