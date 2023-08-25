#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>


#define TRUE 1
#define FALSE 0


void executa_operacoes(char *argv[]); //Função responsável por ler o arquivo de operações e chamar a função correspondente
int busca_registro(short id, int print, FILE* arq_dados); //Função responsável pela operação de busca de registro 
void imprime_resultado(char operacao, short id, short tamanho, int offset, char conteudo[], int tamanho_char); //Função de impressão do resultado das operações
int remove_registro(short id, FILE* arq_dados);
void insere_led(short tamanho, int offset, FILE* fd);
int imprime_led(FILE* arq_dados);

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "Portuguese");
    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        executa_operacoes(argv); 

    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Modo de impressao da LED ativado ...\n");
        // chamada da funcao que imprime as informacoes da led
        FILE* arq_dados = fopen("outros/dados.dat", "rb+");
        imprime_led(arq_dados);

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

    FILE *arq_dados = fopen("outros/dados.dat", "rb+");

    while (fgets(BUFFER, sizeof(BUFFER), arq_operacoes) != NULL) {
        data[0] = '\0';
        sscanf(BUFFER, " %c %hd|%[^\n]", &operacao, &id, data);

        switch(operacao){
            case 'r': 
                remove_registro(id, arq_dados);
                break;
            case 'i':
                //insere_registro();
                break;
            case 'b':
                busca_registro(id,TRUE, arq_dados);
                break;
        }          
    }
    fclose(arq_dados);
    fclose(arq_operacoes);
}
void printCabecalho(FILE* fd){
    rewind(fd);
    int cabecalho;
    fread(&cabecalho,sizeof(int),1,fd);
    printf("CABECA: %d",cabecalho);
}
int busca_registro(short id, int print, FILE* arq_dados){
    short tamanho_reg;
    char id_verificando[5];
    short auxiliar;
    int achou = FALSE;
    char caracter;
    int byte_offset;
    printf("procurando id: %hd\n", id);

    // printCabecalho(arq_dados);


    if (arq_dados == NULL) {
        printf("Erro ao abrir o arquivo para busca");
        exit(1);
    }
    
    fseek(arq_dados, 4, SEEK_SET);
    
    while((achou == FALSE) && (!feof(arq_dados))){
        auxiliar = 0; //reiniciará para todo do while 
        fread(&tamanho_reg, sizeof(tamanho_reg), 1, arq_dados);
        // printf("TAMANHO REG ATUAL: %hd", tamanho_reg);
        byte_offset += (int)tamanho_reg;
        caracter = fgetc(arq_dados);
        char conteudo[2000];

        while((caracter != '|') && (caracter != EOF)){
            id_verificando[auxiliar] = caracter;
            auxiliar++;
            caracter = fgetc(arq_dados);
        }
        id_verificando[auxiliar] = '\0'; //garantir que acaba ali, dar um fim,
        // printf("\n%s - id que estamos verificando\n", id_verificando);
        // printf("Auxiliar terminou em : %i\n", auxiliar);

        if(id == (short)atoi(id_verificando)){
            achou = TRUE;
            printf("nao entrei aqui");
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
        return -1;
    }

}

int remove_registro(short id, FILE* arq_dados){

    if (arq_dados == NULL) {
        printf("Erro ao abrir o arquivo para a operação de remoção.\n");
        return -1;
    }

    char conteudo[2000];
    char caracter;

    
    int byte_offset = busca_registro(id, FALSE, arq_dados);
    if(byte_offset == -1){
        printf("Impossível remover, esse registro não foi encontrado.");
        return -1;
    }
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

    

    insere_led(tamano_registro,byte_offset, arq_dados);

    
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
    printf("\nOFFSET COMO VEIO: %d\n", offset);
    
    short offset_em_short = (short) offset;

    printf("Virou short, offset ficou assim -> %hd\n", offset_em_short);
    
    int cabeca;
    short old_offset;
    int sair = FALSE;
    short tamanhoDoRegistro;
    int mudou = 0;
    short posicao_anterior;
    fread(&cabeca,sizeof(int),1,fd);
    printf("Cabeca atual: %i\n", cabeca);
    if(cabeca == -1){
        rewind(fd);
        int colocar = (int) offset;
        fwrite(&colocar,sizeof(int),1,fd);
        fseek(fd, offset, SEEK_SET);
        char caractere = fgetc(fd);
        while(caractere != '|'){
            caractere = fgetc(fd);
        }
        fseek(fd, 1, SEEK_CUR);
        short cabeca_short = (short) cabeca;
        printf("\nCabeca em short na primeira remocao: %hd\n", cabeca_short);
        fwrite(&cabeca_short, sizeof(short), 1, fd);
    }else{
        fseek(fd,cabeca,SEEK_SET);
        printf("POSICAO AGORA: %ld\n", ftell(fd));
        fread(&tamanhoDoRegistro,sizeof(short),1,fd);
        printf("Tamanho do registro checando: %hd\n", tamanhoDoRegistro);
        if(tamanho > tamanhoDoRegistro){
            rewind(fd);
            int colocar = (int)offset;
            fwrite(&colocar,sizeof(int),1,fd);
            fseek(fd, offset, SEEK_SET); 
            char caractere = fgetc(fd);
            while(caractere != '|'){
                caractere = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR);
            printf("Vamos escrever aqui: %ld\n", ftell(fd));
            short temp = (short) cabeca;
            printf("Escrevendo: %hd\n", temp);
            fwrite(&temp, sizeof(short), 1, fd);
            mudou = 1;
        }
        printf("%hd  - Tamanho do nosso x Tamanho do Reg Atual %hd\n", tamanho, tamanhoDoRegistro);
        printf("Sair = %d\n", sair);
        while((tamanho <= tamanhoDoRegistro) && (sair == FALSE)){
            char caractere = fgetc(fd);
            while(caractere != '|'){
                caractere = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR);
            short tamanho_anterior = tamanhoDoRegistro;
            tamanhoDoRegistro = 0; //reinicia ele ali embaixo pra quando for passar um novo valor
            posicao_anterior = ftell(fd);
            printf("Posicao anterior: %hd\n", posicao_anterior);
            fread(&old_offset, sizeof(short), 1, fd);
            printf("Old offset: %hd\n", old_offset);
            if(old_offset == -1){
                sair = TRUE;
                mudou = 1;
            }else{
                fseek(fd, old_offset, SEEK_SET);
                fread(&tamanhoDoRegistro,sizeof(short),1,fd);  
                printf("Novo tamanho do registro sendo comparado: %hd\n", tamanhoDoRegistro);
            }
        }

        if((mudou == 0) || (sair)){
            printf("\nInserindo:\n");
            printf("Indo para a Posicao anterior: %hd\n", posicao_anterior);
            fseek(fd, posicao_anterior, SEEK_SET);
            printf("Escrevendo esse offset: %hd\n", offset_em_short);
            fwrite(&offset_em_short, sizeof(short), 1, fd);
            fseek(fd, offset, SEEK_SET);
            char caracterer = fgetc(fd);
            while(caracterer != '|'){
                caracterer = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR); 
            printf("Voltamos para esse offset nosso: %d\n", offset);
            fwrite(&old_offset, sizeof(short), 1, fd);
            printf("Escrevendo esse offset: %hd\n", old_offset);

        }

        // if(sair){
        //     fseek(fd, posicao_anterior, SEEK_SET);
        //     fwrite(&offset_em_short, sizeof(short), 1, fd);
        // }
        
    }
}

int imprime_led(FILE* arq_dados){


    int cabeca;
    int quantidade_LED = 0;
    short prox_offset;
    short tamanho_reg;
    rewind(arq_dados);
    fread(&cabeca, sizeof(int), 1, arq_dados);
    if(cabeca == -1){
        printf("LED -> [offset: %i]\n", cabeca);
        printf("Não há espaços disponíveis para inserção na LED\n");
        return -1;
    }
    printf("LED ");  

    fseek(arq_dados, cabeca, SEEK_SET);
    quantidade_LED++;
    fread(&tamanho_reg, sizeof(short), 1, arq_dados);
    char caracterer = fgetc(arq_dados);
    while(caracterer != '|'){
        caracterer = fgetc(arq_dados);
    }
    fseek(arq_dados, 1, SEEK_CUR);
    fread(&prox_offset, sizeof(short), 1, arq_dados);
    printf("-> [offset: %i, tam: %hd]", cabeca, tamanho_reg);

    while(prox_offset != -1){
        quantidade_LED++;
        fseek(arq_dados, prox_offset, SEEK_SET);
        fread(&tamanho_reg, sizeof(short), 1, arq_dados);
        char caracterer = fgetc(arq_dados);
        while(caracterer != '|'){
            caracterer = fgetc(arq_dados);
        }
        fseek(arq_dados, 1, SEEK_CUR);
        printf("-> [offset: %hd, tam: %hd]", prox_offset, tamanho_reg);
        fread(&prox_offset, sizeof(short), 1, arq_dados);
    }

    printf("-> [offset: -1]\n");
    printf("Total de espacos disponiveis: %d\n", quantidade_LED);
}


