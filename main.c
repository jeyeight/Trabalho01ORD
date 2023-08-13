#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ExecutaOperacoes(char *argv[]);

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

void ExecutaOperacoes(char *argv[]){
    FILE *arq = fopen(argv[2], "rb+");
    char operacao;
    short idDoRegistro;
    char BUFFER[1000];

    if(arq == NULL){
        printf("Arquivo inexistente");
        exit(1);
    }

    while (!feof(arq))
    {
        fread(BUFFER, sizeof(char), 1000, arq);
        printf("%s", BUFFER);
    }

    printf("%s",BUFFER);
    

    // while(!feof(arq)){
    //     BUFFER[0] = '\0';
    //     operacao = fgetc(arq);
    //     fseek(arq, 1, SEEK_CUR);
    //     fread(BUFFER, sizeof(short), 1, arq);
    //     idDoRegistro = atoi(BUFFER);
    //     printf("%c", operacao);
    //     printf("%d\n", idDoRegistro);

    //     if(!feof(arq)){
    //         fseek(arq, 2, SEEK_CUR);
    //     }
    // }

    fclose(arq);
}