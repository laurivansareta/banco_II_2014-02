/*
Executado no Ubuntu 12.04
Compilado com gcc version 4.6.3

*/
#include "func.h"

struct CAMPOS *leMetadados(char *meta){ // Retorna os metadados em uma estrutura.
	FILE *metadados;
	metadados = fopen(meta, "r"); // Abre os metadados armazenados em meta
	int qtdCampos, i, j = 0, tam;
	char c;

	if (metadados == NULL)
       exit(0);
    if(fread(&qtdCampos, sizeof(int), 1, metadados)) // Lê o primeiro inteiro que representa a quantidade de campos da tabela.
    {
	   struct CAMPOS *campos=(struct CAMPOS *)malloc(sizeof(struct CAMPOS)*qtdCampos); // Cria uma estrutura para armazenar os dados dos campos.
	   for( i = 0; i < qtdCampos ; i ++) // Laço para ler o nome, tipo e tamanho de cada campo.
	   {
		   fread(&c, 1,1,metadados);
			while (c != '\0') // Laço para ler o nome do campo.
			{
				campos[i].nome[j] = c;
				fread(&c, 1,1,metadados);
				j++;
			}
			fread(&c, 1, 1, metadados); // Lê o tipo do campo.
			campos[i].tipo = c;
			fread(&tam , sizeof(int), 1, metadados); // Lê o tamanho do campo.
			campos[i].tamanho = tam;
			j = 0;

	   }
	   fclose(metadados);// Fecha o arquivo meta
	   return campos;
    }

	return NULL;
}
int tamTupla(struct CAMPOS *campos, char *meta){// Retorna o tamanho total da tupla da tabela.

    int qtdCampos = qtCampos(meta), i, tamanhoGeral = 0;

    if(qtdCampos){ // Lê o primeiro inteiro que representa a quantidade de campos da tabela.
		for(i = 0; i < qtdCampos; i++)
			tamanhoGeral += campos[i].tamanho ; // Soma os tamanhos de cada campo da tabela.
	}

	return tamanhoGeral;
}
int qtCampos(char *meta){ // Retorna a quantidade de campos do esquema
    FILE *metadados;
    int qtdCampos;

    metadados = fopen(meta, "r");

    if (metadados == NULL)
        exit(0);
    fread(&qtdCampos, sizeof(int), 1, metadados); //Lê o primeiro inteiro que representa a quantidade de campos da tabela.
    fclose(metadados);

    return qtdCampos;
}
void leTupla(struct CAMPOS *campos, char *meta, char *linha){ //Lê uma tupla da memória

    char *auxStr; //Variável auxiliar para leitura de stringd

    if(linha == NULL)
        return;

    int qtdCampos, j, k=0 ; // k é a posição do byte dentro da tupla
    qtdCampos = qtCampos(meta);

    for(j=0 ; j<qtdCampos; j++){
        if(j!=0)
            printf(" | ");
        if(campos[j].tipo == 'S'){
            auxStr = linha + k;    //acesso a posição de inínio de uma string
            printf("%-20s", auxStr);
            k += campos[j].tamanho;     //Atualição de leitura do número de bytes para char
        }else if(campos[j].tipo == 'D'){
            double *n = (double *)&linha[k];
            printf("%-20.2f", *n);
            k += sizeof(double);   //Atualição de leitura do número de bytes para double
        }else if(campos[j].tipo == 'I'){
            int *n = (int *)&linha[k];
            printf("%-20d", *n);
            k += sizeof(int);   //Atualição de leitura do número de bytes para int
        }
    }
    printf("\n");

}
char *getTupla(struct CAMPOS *campos, char *meta, char *dado, int from){ //Pega uma tupla do disco a partir do valor de from

    int tamTpl = tamTupla(campos, meta);
    char *linha=(char *)malloc(sizeof(char)*tamTpl);
    FILE *dados;

    dados = fopen(dado, "r");
	if (dados == NULL)
       exit(0);
    fseek(dados, from, 1);
    if(fgetc (dados) != EOF){
        fseek(dados, -1, 1);
        fread(linha, sizeof(char), tamTpl, dados); //Traz a tupla inteira do arquivo
    }
    else{       //Caso em que o from possui uma valor inválido para o arquivo de dados
        fclose(dados);
        return NULL;
    }
    fclose(dados);
    return linha;
}


//============================ BUFFER ===========================//

struct page *inicializaBuffer(){

    struct page *buffer = (struct page *)malloc(sizeof(struct page)*BP); //Aloca as páginas do buffer
    int i;
	for (i=0;i<BP;i++)  //Inicializa o buffer
	{
		buffer->db=0;
		buffer->pc=0;
		buffer->nrec=0;
		buffer->position=0;
		buffer++;
	}
	buffer-=i;

	return buffer;
}
void setTupla(struct page *buffer,char *tupla, int tam, int pos){ //Coloca uma tupla de tamanho "tam" no buffer e na página "pos"
	int i=buffer[pos].position;
	for (;i<buffer[pos].position + tam;i++)
        buffer[pos].data[i] = *(tupla++);

}
void colocaTuplaBuffer(struct page *buffer, char *tupla, struct CAMPOS *campos, char *meta){//Define a página que será incluida uma nova tupla

    int i=0, found=0;
	while (!found && i < BP)//Procura pagina com espaço para a tupla.
	{
    	if(SIZE - buffer[i].position > tamTupla(campos, meta)){// Se na pagina i do buffer tiver espaço para a tupla, coloca tupla.
            setTupla(buffer, tupla, tamTupla(campos, meta), i);
            found = 1;
            buffer[i].position += tamTupla(campos,meta); // Atualiza proxima posição vaga dentro da pagina.
            buffer[i].nrec += 1;
            break;
    	}
    	i++;// Se não, passa pra proxima página do buffer.
    }
    /*
    if (!found)
    {
        printf("Buffer Cheio! Implementar a política de troca.\n");
		return;
    }
    */
}
void showTupleBuffer(struct page *buffer, struct CAMPOS *campos, char *meta, int pg, int rg){
//mostra o registro de número "rg" da página "pg" do bufffer
    int k, i, tam = tamTupla(campos,meta), qt=qtCampos(meta);
    char *linha = NULL;

    if(buffer[pg].position != 0){
        for(i = 0; i< qt; i++)
            printf("%-25s", campos[i].nome);
        printf("\n--------------------------------------------------------------------------------------------\n");
        for(k = 0; k < buffer[pg].nrec; k ++){
            if(k == rg){
                linha = strcop(buffer[pg].data, k, tam); //Função que devolve uma string para a impressão da tupla
                leTupla(campos, meta, linha); //Mostra a string
                return;
            }
        }
        printf("Posição inválida!\n");
    }

}
void showBuffer(struct page *buffer, struct CAMPOS *campos, char *meta){
//Mostra todas as páginas do buffer que contenham registros
    int i, k, tam = tamTupla(campos,meta);
    char *linha = NULL;
    for(i = 0; i < BP; i++){
        if(buffer[i].position != 0){
            printf("Página %d:\n", i);
            for(k = 0; k < buffer[i].nrec; k ++){
                linha = strcop(buffer[i].data, k, tam);
                leTupla(campos, meta, linha);
            }
        }
    }
}
char *strcop(char *data, int pos, int tam){//Copia registro do buffer
    //pos é a posição inicial do registro a ser copiado e tam é seu tamanho total
    int i;
    char *linha = (char *)malloc(sizeof(char)*tam);
    for(i = 0; i < tam ; i++){
        linha[i] = data[(tam*pos) + i];// Copia a tupla para uma variável auxiliar.
    }
    return linha;
}
void carregaDados(struct page *buffer, char *data, char *meta, struct CAMPOS *campos){
//Traz todos os registros para o buffer
    int i=1, tamTpl = tamTupla(campos, meta);
    char *linha = getTupla(campos, meta, data, 0);

    while(linha != NULL){ //Pega as tuplas e insere no buffer até que acabe o arquivo
        colocaTuplaBuffer(buffer, linha, campos, meta);
        linha = getTupla(campos, meta, data, i*tamTpl);
        i++;
    }
}

