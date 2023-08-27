#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void executa_operacoes(char *argv[]); //Função responsável por ler o arquivo de operações e chamar a função correspondente
int busca_registro(short id, int print, FILE* arq_dados); //Função responsável pela operação de busca de registro 
void imprime_resultado(char operacao, short id, short tamanho, short tamanho_reinsercao, int offset, char conteudo[]); //Função de impressão do resultado das operações
int remove_registro(short id, FILE* arq_dados);
void insere_led(short tamanho, int offset, FILE* fd);
int imprime_led();
int insere_registro(char data[],short id, FILE* arq_dados);
void remove_led(FILE* fd, int cabeca, short offset_antes);

int main(int argc, char *argv[]){
    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        executa_operacoes(argv); 
    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {
        printf("Modo de impressao da LED ativado ...\n");
        imprime_led();
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
    FILE *arq_dados = fopen("outros/dados.dat", "rb+");
    short id;
    char operacao;
    char BUFFER[10000];
    char data[10000];

    if (arq_operacoes == NULL) {
        printf("Erro ao abrir o arquivo");
        exit(1);
    }
    while (fgets(BUFFER, sizeof(BUFFER), arq_operacoes) != NULL) {
        data[0] = '\0';
        sscanf(BUFFER, " %c %hd|%[^\n]", &operacao, &id, data);
        char registro[10000];
        if(operacao == 'i'){
            itoa(id, registro, 10);
            strcat(registro, "|");
            strcat(registro, data);
            unsigned int tamanho_reg = strlen(registro);
        }
        switch(operacao){
            case 'r': 
                remove_registro(id, arq_dados);
                break;
            case 'i':
                insere_registro(registro , id, arq_dados);
                break;
            case 'b':
                busca_registro(id,TRUE, arq_dados);
                break;
        }          
    }
    fclose(arq_dados);
    fclose(arq_operacoes);
}
int busca_registro(short id, int print, FILE* arq_dados){
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
    fseek(arq_dados, 4, SEEK_SET);
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
        char sera_que_removeu = fgetc(arq_dados);
        if(sera_que_removeu == '*'){
            if(id == (short)atoi(id_verificando)){
                print = 0;
                achou = FALSE; //caso ele não saia no return -1
                printf("\nRegistro ja foi removido.");
                return -1;
            }
            fseek(arq_dados, tamanho_reg - auxiliar, SEEK_CUR);
            fseek(arq_dados, -2, SEEK_CUR);
            id_verificando[0] ='\0';
        }
        else{
            fseek(arq_dados, -1, SEEK_CUR);
            if(id == (short)atoi(id_verificando)){
                achou = TRUE;
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
                    imprime_resultado('b',id,tamanho_reg,0 ,offset, conteudo);
                    id_verificando[0] = '\0';
                }
                return offset; 
            }else{
                char fim = fgetc(arq_dados);
                if((!feof(arq_dados)) || (fim != EOF)){
                    fseek(arq_dados, -1, SEEK_CUR);
                    fseek(arq_dados, tamanho_reg - auxiliar, SEEK_CUR);
                    fseek(arq_dados, -1, SEEK_CUR);
                }
                else{
                    printf("Registro nao encontrado\n");
                    return -1;
                }
            id_verificando[0] = '\0';
            }
        }
    }
    if((achou == FALSE) || (feof(arq_dados))){
        printf("\nNao foi encontrado esse Registro com o ID - %hd\n\n", id);
        return -1;
    }
}

int remove_registro(short id, FILE* arq_dados){
    char caracter;
    short tamanho_registro;
    char tamanho_reg_char[20];
    int byte_offset = busca_registro(id, FALSE, arq_dados);

    if (arq_dados == NULL) {
        printf("Erro ao abrir o arquivo para a operação de remoção.\n");
        return -1;
    }
    if(byte_offset == -1){
        printf("Impossível remover, esse registro não foi encontrado.");
        return -1;
    }
    fseek(arq_dados, byte_offset,SEEK_SET);
    fread(&tamanho_registro, sizeof(tamanho_registro), 1, arq_dados);
    fseek(arq_dados,ftell(arq_dados),SEEK_SET);
    fputc('*', arq_dados);
    fseek(arq_dados, -1, SEEK_CUR);
    
    char caractere = fgetc(arq_dados);
    while(caractere != '|'){
        caractere = fgetc(arq_dados);
    }
    fseek(arq_dados,ftell(arq_dados),SEEK_SET);
    fputc('*', arq_dados);
    imprime_resultado('r', id, tamanho_registro, 0, byte_offset, "" );
    insere_led(tamanho_registro,byte_offset, arq_dados);
}

void imprime_resultado(char operacao, short id, short tamanho, short tamanhoDoRegistro, int offset, char conteudo[]){
    printf("------------------------------------------------------------------------------------\n");
    switch (operacao){
        case 'r':
            printf("Remocao do registro de chave %hd Registro removido! (%hd bytes) Local: offset = %d bytes (0X%X)", id, tamanho, offset, offset);
            printf("------------------------------------------------------------------------------------\n");

            break;
        case 'b':
            printf("Busca pelo registro de chave %hd \n%s (%hd bytes)\nLocal: offset = %d bytes (0X%X)\n\n",id,conteudo,tamanho,offset, offset);
            printf("------------------------------------------------------------------------------------\n");

            break;
        case 'i':
        if(tamanhoDoRegistro !=0){
            printf("Insercao do registro de chave %hd (%hd bytes)\nTamanho do espaco reutilizado: %hd bytes (Sobra de %hd bytes)\nLocal: offset = %hd bytes (0X%X)", id, tamanho, tamanhoDoRegistro , (tamanhoDoRegistro - tamanho - 2), offset, offset);
            printf("------------------------------------------------------------------------------------\n");

            break;
        }else{
            printf("Insercao do registro de chave %hd (%hd bytes)\nLocal: Fim do arquivo", id, tamanho);
            printf("------------------------------------------------------------------------------------\n");

            break;
        }
        default:
            printf("Operação não suportada");
            printf("------------------------------------------------------------------------------------\n"); 
            break;
    }
}
void insere_led(short tamanho, int offset, FILE* fd){ // PROBLEMA, ESSE OFFSET ESTÁ VINDO COMO SHORT, porém é um int antes
    short offset_em_short = (short) offset;
    int cabeca;
    short old_offset;
    int sair = FALSE;
    short tamanhoDoRegistro;
    int mudou = 0;
    short posicao_anterior;

    rewind(fd);
    fread(&cabeca,sizeof(int),1,fd);
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
        short cabeca_short = (short)cabeca;
        fwrite(&cabeca_short, sizeof(short), 1, fd);
    }else{
        fseek(fd,cabeca,SEEK_SET);
        fread(&tamanhoDoRegistro,sizeof(short),1,fd);
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
            short temp = (short) cabeca;
            fwrite(&temp, sizeof(short), 1, fd);
            mudou = TRUE;
        }
        while((tamanho <= tamanhoDoRegistro) && (sair == FALSE)){
            char caractere = fgetc(fd);
            while(caractere != '|'){
                caractere = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR);
            short tamanho_anterior = tamanhoDoRegistro;
            tamanhoDoRegistro = 0; //reinicia ele ali embaixo pra quando for passar um novo valor
            posicao_anterior = ftell(fd);
            fread(&old_offset, sizeof(short), 1, fd);
            if(old_offset == -1){
                sair = TRUE;
                mudou = TRUE;
            }else{
                fseek(fd, old_offset, SEEK_SET);
                fread(&tamanhoDoRegistro,sizeof(short),1,fd);  
            }
        }

        if((mudou == FALSE) || (sair)){
            fseek(fd, posicao_anterior, SEEK_SET);
            fwrite(&offset_em_short, sizeof(short), 1, fd);
            fseek(fd, offset, SEEK_SET);
            char caracterer = fgetc(fd);
            while(caracterer != '|'){
                caracterer = fgetc(fd);
            }
            fseek(fd, 1, SEEK_CUR); 
            fwrite(&old_offset, sizeof(short), 1, fd);
        }
    }
}

int insere_registro(char data[], short id, FILE* fd){
    int cabeca;
    int auxiliar = 1;
    short tamanho = strlen(data);
    short offset_reinsercao;
    short tamanho_reinsercao;
    short tamanhoDoRegistro;
    short offset_anterior;
    short offset_insercao;
    short verifica_id = busca_registro(id,0,fd);
    if(verifica_id != -1){
        printf("ID já cadastrado\n");
        return -1;
    }

    rewind(fd);    
    fread(&cabeca,sizeof(int),1,fd);
    if(cabeca == -1){
        fseek(fd, 0, SEEK_END);
        offset_insercao = ftell(fd);
        fwrite(&tamanho,sizeof(short),1,fd);
        fseek(fd,0,SEEK_CUR);
        fputs(data,fd);
        imprime_resultado('i',id,tamanho, 0, offset_insercao,"");
    }else{
        fseek(fd,cabeca,SEEK_SET);
        fread(&tamanhoDoRegistro,sizeof(short),1,fd);//Lendo offset que está na cabeça
        char caracterer = fgetc(fd);
        while(caracterer != '|'){
            caracterer = fgetc(fd);
            auxiliar++;
        }
        fseek(fd, 1, SEEK_CUR);
        fread(&offset_anterior, sizeof(short), 1, fd);
        fseek(fd, -3, SEEK_CUR); //menos 1 + o menos 2 do short
        fseek(fd, -auxiliar, SEEK_CUR);
        if(tamanho <= tamanhoDoRegistro){ // Comparando tamanho da cabeça com o do registro a ser inserido
            fseek(fd,-2,SEEK_CUR);
            offset_insercao = ftell(fd);
            fwrite(&tamanho,sizeof(short),1,fd);
            fseek(fd,0,SEEK_CUR);
            data[tamanho] = '\0';
            fputs(data,fd);
            offset_reinsercao = ftell(fd);
            tamanho_reinsercao = tamanhoDoRegistro - tamanho - 2;
            fseek(fd,offset_reinsercao,SEEK_SET);
            fwrite(&tamanho_reinsercao, sizeof(short), 1, fd);
            fputs("|*",fd);
            remove_led(fd,cabeca, offset_anterior);
            if (tamanho_reinsercao > 6){ // Tamanho mínimo da reinserção (vai de cada um né pô)
                insere_led(tamanho_reinsercao, offset_reinsercao, fd);
            }
            imprime_resultado('i',id,tamanho, tamanhoDoRegistro, offset_insercao,"");
        }
        else{ 
            data[tamanho] = '\0';
            fseek(fd, 0, SEEK_END);
            offset_insercao = ftell(fd);
            fwrite(&tamanho,sizeof(short),1,fd);
            fseek(fd,0,SEEK_CUR);
            fputs(data,fd);
            imprime_resultado('i',id,tamanho, 0, offset_insercao,"");
        }
    }
}

void remove_led(FILE* fd, int cabeca, short offset_antes){
    short nova_cabeca;
    fseek(fd,cabeca,SEEK_SET);
    int nova_cabeca_int = (int) offset_antes;
    rewind(fd);
    fwrite(&nova_cabeca_int,sizeof(int),1,fd);
}

int imprime_led(){
    FILE* arq_dados = fopen("outros/dados.dat", "rb+");
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