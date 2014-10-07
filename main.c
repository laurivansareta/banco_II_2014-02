#include "func.h"

int main(int rg, char *arq[]){
    //main demonstrativa, para mais detalhes consulte arquivo LEIAME.txt e func.c

    char *meta = arq[1], *data = arq[2]; //Faça a atribuição conforme a ordem de entrada na execução pela linha de comando

    struct page *buffer = inicializaBuffer();

	struct CAMPOS *campos = leMetadados(meta); // Traz pra memória o esquema dos dados

    carregaDados(buffer, data, meta, campos);

    //showBuffer(buffer, campos, meta);

    showTupleBuffer(buffer, campos, meta, 0, 1);

    return 0;
}
