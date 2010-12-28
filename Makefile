#Practicas AI. Makefile Version 1.0

# Ficheros objeto que deben se generados para el proyecto
#OBJS2= listadin_freqs.o vdin_str.o vdin_documentos.o vdin_freqs.o avl_words.o depuracion.o ai_indice.o main.o PorterStemmer.o
OBJS= eedd/vdin_str.o eedd/avl_words.o eedd/listadin_freqs.o depuracion.o ai_indice.o ai_buscador.o main.o PorterStemmer.o qs.o
EEDDS= vdin_str.o avl_words.o listadin_freqs.o

# Nombre del programa ejecutable a generar cuando se enlacen los ficheros objeto
EJECUTABLE=ai_buscador.out

# Rutas donde se buscarn los ficheros cabecera del proyecto: -Idir1 -Idir2 ... 
INCLUDES= -I. -Ieedd

# Bibliotecas adicionales: -lnombre_blioteca1 -lnombre_biblioteca2 ...
LIBS=-lm

# Compilador a usar
CC=gcc

# Parmetro para incluir informacin de depuracin
CFLAGS_DEBUG=-g

# Parmetros finales para el compilador
CFLAGS=$(CFLAGS_DEBUG) -I$(INCLUDES) 

all: $(EJECUTABLE)

# Reglas de compilacin
$(EJECUTABLE): $(OBJS)
	mv $(EEDDS) eedd/
	$(CC) $(LIBS) $+ -o $@
	
%.o: %.c
	$(CC) $(CFLAGS) -c $+

clean:
	rm -f *.o *~ eedd/*.o
