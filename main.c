#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE_LENGTH 1000

void ExecutaOperacoes(char *argv[]);
void ProcessLine(char operacao, short id, char *data);
void busca_registro(short id);

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
    FILE *arq_operacoes = fopen(argv[2], "rb+");
    short id;
    char operacao;
    short idDoRegistro;
    char BUFFER[1000];
    char data[1000];

    if (arq_operacoes == NULL) {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }

    while (fgets(BUFFER, sizeof(BUFFER), arq_operacoes) != NULL) {
        data[0] = '\0';
        sscanf(BUFFER, " %c %hd|%[^\n]", &operacao, &id, data);
        printLinha(operacao, id, data);

        switch(operacao){
            case 'r': 
                //remove_registro();
                break;
            case 'i':
                //insere_registro();
                break;
            case 'b':
                busca_registro(id);
                break;
        }
        
        exit(1);
          
    }

    fclose(arq_operacoes);
}

void busca_registro(short id){
    FILE *arq_dados = fopen("outros/dados.dat", "rb+");
    short tamanho_reg;
    char id_verificando[5];
    short auxiliar;
    int achou = 0;

    if (arq_dados == NULL) {
        printf("Erro ao abrir o arquivo para busca");
        exit(1);
    }

    fseek(arq_dados, 4, SEEK_SET);

    do{
        auxiliar = 0;
        fread(&tamanho_reg, sizeof(tamanho_reg), 1, arq_dados);
        printf("%hd\n", tamanho_reg);

        char caracter = fgetc(arq_dados);

        while(caracter != EOF && caracter != '|'){
            id_verificando[auxiliar] = caracter;
            auxiliar++;

            caracter = fgetc(arq_dados);

        }

        id_verificando[auxiliar] = '\0';

        // printf("ID - VERIFICANDO %s\n", id_verificando);
        // printf("TAMANHO - REG %hd\n", tamanho_reg);
        // printf("AUXILIAR - %hd\n", auxiliar);
        // printf("%hd\n", tamanho_reg - (auxiliar + 1)); 
        if(id == (short)atoi(id_verificando)){
            //Imprime_Resultado('r',);
            achou = 1;
            printf("ID %hd ENCONTRADO", id);
        }else{
            fseek(arq_dados, tamanho_reg - (auxiliar + 1), SEEK_CUR);
        }
    } while(achou == 0);


    fclose(arq_dados);
    
}

//     if(arq == NULL){
//         printf("Arquivo inexistente");
//         exit(1);
//     }
//     while ((character = fgetc(arq)) != EOF && character != '\n') {
//             BUFFER[index] = character;
//             index++;
//     }
//     printf("A linha lida do arquivo Ã©: %s\n", BUFFER);
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