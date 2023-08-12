#include <stdio.h>
#include <string.h>
#include <stdlib.h>

executaOperacoes(FILE *arq, int argv[2]);

int main(int argc, char *argv[]) {

    if (argc == 3 && strcmp(argv[1], "-e") == 0) {

        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        FILE *arq = fopen(argv[2], "rb+"); 

        if(arq == NULL) {
            printf("Arquivo de operações não existe.");
            fclose(arq);
            exit(1);
        }
        //programa[0] -e[1] nomedoarquivo de operacoes[2]
        executaOperacoes(arq, argv[2]);
        // chamada da funcao que executa o arquivo de operacoes
        // o nome do arquivo de operacoes estara armazenado na variavel argv[2]
        // executa_operacoes(argv[2])

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