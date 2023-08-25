#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void executa_operacoes(char *argv[]); //Função responsável por ler o arquivo de operações e chamar a função correspondente
int busca_registro(short id, int print); //Função responsável pela operação de busca de registro 
void imprime_resultado(char operacao, short id, short tamanho, int offset, char conteudo[], int tamanho_char); //Função de impressão do resultado das operações
void remove_registro(short id);
void insere_led(short tamanho, int offset, FILE* fd);

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
void printCabecalho(FILE* fd){
    rewind(fd);
    int cabecalho;
    fread(&cabecalho,sizeof(int),1,fd);
    printf("CABECA: %d",cabecalho);
}
int busca_registro(short id, int print){
    FILE *arq_dados = fopen("outros/dados.dat", "rb+");
    short tamanho_reg;
    char id_verificando[5];
    short auxiliar;
    int achou = FALSE;
    char caracter;
    int byte_offset;

    // printCabecalho(arq_dados);


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
            //printf("ACHOU AQUI: %ld",ftell(arq_dados));         
            fseek(arq_dados,-1,SEEK_CUR);
            fseek(arq_dados, -auxiliar ,SEEK_CUR);
            int offset = ftell(arq_dados) - 2; // o + 1 acho que n precisa, pq ele começa no 0
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
    fread(&tamano_registro, sizeof(tamano_registro), 1, arq_dados);
    
    short indice; 
    char caractere = fgetc(arq_dados);
    while(caractere != '|'){
        caractere = fgetc(arq_dados);
    }
    fseek(arq_dados,ftell(arq_dados),SEEK_SET);
    fputc('*', arq_dados);
    imprime_resultado('r', id, tamano_registro, byte_offset, conteudo, 2000);

    

    insere_led(byte_offset,tamano_registro, arq_dados);

    fclose(arq_dados);
    
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

void insere_led(short tamanho, int offset, FILE* fd){ // PROBLEMA, ESSE OFFSET ESTÁ VINDO COMO SHORT, porém é um int antes
    rewind(fd);
    short offset_em_short = (short) offset;
    int cabeca;
    short old_offset;
    short tamanhoDoRegistro;
    int mudou = 0;
    short posicao_anterior;
    fread(&cabeca,sizeof(int),1,fd);
    if(cabeca == -1){
        rewind(fd);
        int colocar = (int)offset;
        fwrite(&colocar,sizeof(int),1,fd);
    }else{
        fseek(fd,cabeca,SEEK_SET);
        fseek(fd,-2,SEEK_CUR);
        fread(&tamanhoDoRegistro,sizeof(short),1,fd);
        if(tamanho > tamanhoDoRegistro){
            rewind(fd);
            int colocar = (int)offset;
            fwrite(&offset,sizeof(int),1,fd);
            fseek(fd, offset, SEEK_CUR); //NAO SERÁ TRES, depende do tamanho do indice, colocar o fgetc até '|'
            char caractere = fgetc(fd);
            while(caractere != '|'){
                caractere = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR);
            short temp = (short) cabeca;
            fwrite(&temp, sizeof(short), 1, fd);
            mudou = 1;
        }
        while(tamanho < tamanhoDoRegistro){
            old_offset = 0; //reinicia tb ele, n sei se precisa pra quando fará outro fread nele.
            char caractere = fgetc(fd);
            while(caractere != '|'){
                caractere = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR);
            short tamanho_anterior = tamanhoDoRegistro;
            tamanhoDoRegistro = 0; //reinicia ele ali embaixo pra quando for passar um novo valor
            posicao_anterior = ftell(fd);
            fread(&old_offset, sizeof(short), 1, fd);
            fseek(fd, old_offset, SEEK_SET);
            fseek(fd, -2, SEEK_CUR);
            fread(&tamanhoDoRegistro,sizeof(short),1,fd);  
        }

        if(mudou == 0){
            fseek(fd, posicao_anterior, SEEK_SET);
            fwrite(&offset_em_short, sizeof(short), 1, fd);
            fseek(fd, offset, SEEK_SET);
            fseek(fd, 2, SEEK_CUR); //Pula short ( NAO QUEREMOS LER TAMANHO AGR.)
            char caracterer = fgetc(fd);
            while(caracterer != '|'){
                caracterer = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR); 
            fwrite(&old_offset, sizeof(short), 1, fd);
        }
        
    }
}

