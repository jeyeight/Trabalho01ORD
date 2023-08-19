#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE_LENGTH 1000

void ExecutaOperacoes(char *argv[]);
void ProcessLine(char operacao, short id, char *data);

int main(int argc, char *argv[]){
    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        ExecutaOperacoes(argv); 
        

    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Modo de impressao da LED ativado ...\n");
        // chamada da funcao que imprime as informacoes da led
        // imprime_led();

    } else {
        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s -e nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}

void printLinha(char operacao, short id, char *data) {
    printf("Operacao: %c, Id: %hd, Data: %s\n", operacao, id, data);
}


void ExecutaOperacoes(char *argv[]){
    FILE *arq = fopen(argv[2], "rb+");
    short id;
    char operacao;
    short idDoRegistro;
    char BUFFER[1000]; //adsdasdasdsadas
    char data[1000];

    if (arq == NULL) {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }

    while (fgets(BUFFER, sizeof(BUFFER), arq) != NULL) {
        sscanf(BUFFER, " %c %hd|%[^\n]", &operacao, &id, data);
        printLinha(operacao, id, data);
    }

    fclose(arq);
}

//     if(arq == NULL){
//         printf("Arquivo inexistente");
//         exit(1);
//     }
//     while ((character = fgetc(arq)) != EOF && character != '\n') {
//             BUFFER[index] = character;
//             index++;
//     }
//     printf("A linha lida do arquivo é: %s\n", BUFFER);
//     }

//     fclose(arq);
// }

// void ImprimeResultado(char operacao, int id, int bytes, int offset){
//     if(strcmp(operacao,"r") == 1){
//             printf("Remocao do registro de chave %d Registro removido! (%d bytes) Local: offset = %d bytes (%p)", id, bytes, offset,&id);
//     }else if(strcmp(operacao,"i") == 1){
//             printf("Remocao do registro de chave %d Registro removido! (%d bytes) Local: offset = %d bytes (%p)", id, bytes, offset,&id);
//     }else{
//             printf("Remocao do registro de chave %d Registro removido! (%d bytes) Local: offset = %d bytes (%p)", id, bytes, offset, &id);
//     }
// }