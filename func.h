/*
Executado no Ubuntu 12.04
Compilado com gcc version 4.6.3

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BP 512
#define SIZE 1024

struct CAMPOS {
   char nome[40];
   char tipo;
   int tamanho;
};

struct page {
   unsigned int nrec;            // numero de registros
   char data[SIZE];              //armazenamento das tuplas da página
   unsigned int pc;              // pin_count -> contador de uso da página
   unsigned int db;              // dirty_bit -> se dados foram modificados
   unsigned int position;        // posição livre da página
};

struct CAMPOS *leMetadados(char *meta);
int tamTupla(struct CAMPOS *campos, char *meta);
void leTupla(struct CAMPOS *campos, char *meta, char *linha);
int qtCampos(char *meta);
char *getTupla(struct CAMPOS *campos, char *meta, char *dado, int from);
void carregaDados(struct page *buffer, char *data,char *meta, struct CAMPOS *campos);
struct page *inicializaBuffer();
void setTupla(struct page *buffer,char *tupla, int tam, int pos);
void colocaTuplaBuffer(struct page *buffer, char *tupla, struct CAMPOS *campos, char *meta);
char *strcop(char *data, int k, int tam);
void showBuffer(struct page *buffer, struct CAMPOS *campos, char *meta);
void showTupleBuffer(struct page *buffer, struct CAMPOS *campos, char *meta, int pg, int rg);


