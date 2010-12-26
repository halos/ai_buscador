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

#include "ai_indice.h"

/**
 * @brief Separa las cadenas de texto cuando hay ',' ';' '.' ':' '\n' '\t'
 * @param str Cadena que separar
 * @return vdin_str con las palabras separadas
 * @post Las palabras devueltas tienen su memoria reservada
 */
vdin_str split_text(const char* str, const char* elim){

    char *ptr;
    char *word;
    char *copia_str;
    //char elim[] = " ,;.:\r\n\t";
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

