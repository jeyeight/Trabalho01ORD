#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void executa_operacoes(char *argv[]); //Função responsável por ler o arquivo de operações e chamar a função correspondente
void print_linha(char operacao, short id, char *data);
int busca_registro(short id, int print); //Função responsável pela operação de busca de registro 
void imprime_resultado(char operacao, short id, short tamanho, int offset, char conteudo[], int tamanho_char); //Função de impressão do resultado das operações
void remove_registro(short id);

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

void dd(char caracter){
    printf("%c",caracter);
    exit(1);
}
//


void executa_operacoes(char *argv[]){
    FILE *arq_operacoes = fopen(argv[2], "rb+");
    short id;
    char operacao;
    short idDoRegistro;
    char BUFFER[10000];
    char data[10000];

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
                remove_registro(id);
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
    int achou = FALSE;
    char caracter;
    int byte_offset;

    if (arq_dados == NULL) {
        printf("Erro ao abrir o arquivo para busca");
        exit(1);
    }
    if(ftell(arq_dados) == 0){
        fseek(arq_dados, 4, SEEK_SET);
    }
    while((achou == FALSE) && (!feof(arq_dados))){
        auxiliar = 0; //reiniciará para todo do while 
        fread(&tamanho_reg, sizeof(tamanho_reg), 1, arq_dados);
        byte_offset += (int)tamanho_reg;
        caracter = fgetc(arq_dados);
        char conteudo[2000];

        while((caracter != '|') && (caracter != EOF)){
            id_verificando[auxiliar] = caracter;
            auxiliar++;
            caracter = fgetc(arq_dados);
        }
        id_verificando[auxiliar] = '\0'; //garantir que acaba ali, dar um fim,

        if(id == (short)atoi(id_verificando)){
            achou = TRUE;         
            fseek(arq_dados,-1,SEEK_CUR);
            fseek(arq_dados, -auxiliar ,SEEK_CUR);
            int offset = ftell(arq_dados); // o + 1 acho que n precisa, pq ele começa no 0
            fread(conteudo, sizeof(char),tamanho_reg,arq_dados);
            if(conteudo[auxiliar + 1] == '*'){
                print = 0;
                achou = FALSE; //caso ele não saia no return -1
                printf("\nRegistro ja foi removido.");
                return -1;
            }
            conteudo[tamanho_reg] = '\0';
            if(print == 1){
                imprime_resultado('b',id,tamanho_reg,offset, conteudo, 2000);
                id_verificando[0] = '\0';
            }
            
            return offset; 

        }else{
            if(!feof(arq_dados)){
                fseek(arq_dados, tamanho_reg - auxiliar, SEEK_CUR);
                fseek(arq_dados, -1, SEEK_CUR);
            }
        id_verificando[0] = '\0';
        }
    }

    if((achou == FALSE) || (feof(arq_dados))){
        printf("\nNao foi encontrado esse Registro com o ID - %hd\n\n", id);
    }
    fclose(arq_dados);
}

void remove_registro(short id){
    FILE *arq_dados = fopen("outros/dados.dat", "rb+");

    if (arq_dados == NULL) {
        printf("Erro ao abrir o arquivo para a operação de remoção.\n");
        return;
    }

    char conteudo[2000];
    char caracter;
    int byte_offset = busca_registro(id, FALSE);
    short tamano_registro;
    char tamanho_reg_char[20];
    
    fseek(arq_dados, byte_offset,SEEK_SET);
    fseek(arq_dados, -2, SEEK_CUR);
    fread(&tamano_registro, sizeof(tamano_registro), 1, arq_dados);
    
    short indice; 
    char caractere = fgetc(arq_dados);
    while(caractere != '|'){
        caractere = fgetc(arq_dados);
    }
    fseek(arq_dados,ftell(arq_dados),SEEK_SET);
    fputc('*', arq_dados);
    imprime_resultado('r', id, tamano_registro, byte_offset, conteudo, 2000);
    fclose(arq_dados);
    //insere_led(byte_offset,tamanho_reg)
}

void imprime_resultado(char operacao, short id, short tamanho, int offset, char conteudo[], int tamanho_char){
    conteudo[2000];
    switch (operacao){
        case 'r':
            printf("Remocao do registro de chave %hd Registro removido! (%hd bytes) Local: offset = %d bytes (%p)", id, tamanho, offset,&id);
            break;
        case 'b':
            printf("Busca pelo registro de chave %hd \n%s (%hd bytes)\nLocal: offset = %d bytes (%p)\n\n",id,conteudo,tamanho,offset);
            break;
        case 'i':
            printf("Remocao do registro de chave %hd Registro removido! (%hd bytes) Local: offset = %d bytes (%p)", id, tamanho, offset, &id);
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

struct No{
    struct No* prox;
    int info;
};

struct No* criar_No(int info){ 
    struct No* no = (struct No*)malloc(sizeof(struct No));
    no->info = info;
    no->prox = NULL;
    return no;

}

// void inicia_LED(struct ListaLED* lista){
//     lista->tamanho = 0;
// }

// void insere_LED(struct No** cabeca, int novo_registro){
    
//     struct No* atual = cabeca;
// }

int lista_vazia(struct No* cabeca) {
    return cabeca == NULL;
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