#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void executa_operacoes(char *argv[]); //Função responsável por ler o arquivo de operações e chamar a função correspondente
void print_linha(char operacao, short id, char *data);
int busca_registro(short id, int print); //Função responsável pela operação de busca de registro 
void impime_resultado(char operacao, int id, int bytes, int offset); //Função de impressão do resultado das operações

int main(int argc, char *argv[]){
    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        executa_operacoes(argv); 

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

//debug functions
void print_linha(char operacao, short id, char *data) {
    printf("Operacao: %c, Id: %hd, Data: %s\n", operacao, id, data);
}

void tell(FILE* arquivo){
        printf("%ld",ftell(arquivo));
}
//


void executa_operacoes(char *argv[]){
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
        print_linha(operacao, id, data);

        switch(operacao){
            case 'r': 
                //remove_registro(id);
                break;
            case 'i':
                //insere_registro();
                break;
            case 'b':
                busca_registro(id,TRUE);
                break;
        }          
    }

    fclose(arq_operacoes);
}

int busca_registro(short id, int print){
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
    tell(arq_dados);
    exit(TRUE);
    do{
        auxiliar = 0;
        fread(&tamanho_reg, sizeof(tamanho_reg), 1, arq_dados);
        // printf("%hd\n", tamanho_reg);

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
            achou = TRUE;
            printf("ACHO");
            printf("%hd",tamanho_reg);
            if(print == 1){
            // impime_resultado('r',id,41,99);
            }
        }else{
            fseek(arq_dados, tamanho_reg - (auxiliar + 1), SEEK_CUR);
        }
    } while(achou == FALSE && !feof(arq_dados));

    fclose(arq_dados);
}

void remove_registro(short id){
    FILE *arq_dados = fopen("outros/dados.dat", "rb+");
    int byte_offset = busca_registro(id, FALSE);
    short tamanho_reg; 
    fseek(arq_dados,byte_offset,SEEK_SET);
    fread(&tamanho_reg,sizeof(tamanho_reg),1,arq_dados);
    fputc('*', arq_dados);
    fclose(arq_dados);
    //insere_led(byte_offset,tamanho_reg)
}

void impime_resultado(char operacao, int id, int bytes, int offset){
    switch (operacao){
        case 'r':
            printf("Remocao do registro de chave %d Registro removido! (%d bytes) Local: offset = %d bytes (%p)", id, bytes, offset,&id);
            break;
        case 'b':
            printf("Remocao do registro de chave %d Registro removido! (%d bytes) Local: offset = %d bytes (%p)", id, bytes, offset,&id);
        case 'i':
            printf("Remocao do registro de chave %d Registro removido! (%d bytes) Local: offset = %d bytes (%p)", id, bytes, offset, &id);
            break;
        default:
            printf("Operação não suportada");
            break;
    }
}

struct ListaLED{
    int LED[100];
    int tamanho;
};

void inicia_LED(struct ListaLED* lista){
    lista->tamanho = 0;
}

void insere_LED(struct ListaLED* lista, int novo_registro){
    if (lista->tamanho < 100){
        if(lista->tamanho == 0){
            lista->LED[0] = novo_registro;
            lista->tamanho++;
        }
        else{
            int i = 0;
            while(lista->LED[i] > novo_registro){
                i++;
            }
            int transloca_LED = lista->tamanho;
            while(transloca_LED >= i){
                lista->LED[transloca_LED + 1] = lista->LED[transloca_LED];
                transloca_LED--;
            }
            lista->LED[i] = novo_registro;
            lista->tamanho++;
        }
    }
    else{
        printf("\nLista está CHEIA. Não foi possível inserir esse elemento na LED");
    }

}

