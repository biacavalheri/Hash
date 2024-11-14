#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_TABELA 13
#define TAMANHO_BUCKET 2

typedef struct {
    char idAluno[4];
    char siglaDisciplina[4];
    char nomeAluno[50];
    char nomeDisciplina[50];
    float media;
    float freq;
} REGISTRO;

typedef struct {
    char idAluno[4];
    char siglaDisciplina[4];
} CHAVE_PRIMARIA;

typedef struct {
    char chavePrimaria[8];
    long offset;
    int flag;
} INDICE_PRIMARIO;

INDICE_PRIMARIO hashTable[TAMANHO_TABELA][TAMANHO_BUCKET];

int hash(const char *chavePrimaria) {
    int soma = 0;
    int primo = 31;
    for (int i = 0; i < strlen(chavePrimaria); i++) {
        soma = (soma * primo + chavePrimaria[i]) % TAMANHO_TABELA;
    }
    return soma;
}

void iniciaHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        for (int j = 0; j < TAMANHO_BUCKET; j++) {
            hashTable[i][j].flag = 0;
        }
    }
}

FILE *verificaArquivo(const char *nomeArquivo, const char *modo) {
    FILE *fp = fopen(nomeArquivo, modo);
    if (!fp) {
        printf("Erro\n");
        exit(1);
    }
    return fp;
}

int buscarRegistroEmArquivo(const char *chavePrimaria, REGISTRO *registro) {
    FILE *fp = verificaArquivo("insere.bin", "rb");

    while (fread(registro, sizeof(REGISTRO), 1, fp) == 1) {
        char chaveAtual[8];
        snprintf(chaveAtual, sizeof(chaveAtual), "%s%s", registro->idAluno, registro->siglaDisciplina);

        if (strcmp(chaveAtual, chavePrimaria) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int verificarDuplicacao(const char *chavePrimaria) {
    int endereco = hash(chavePrimaria);
    for (int tentativa = 0; tentativa < TAMANHO_TABELA; tentativa++) {
        for (int j = 0; j < TAMANHO_BUCKET; j++) {
            if (hashTable[endereco][j].flag && strcmp(hashTable[endereco][j].chavePrimaria, chavePrimaria) == 0) {
                return 1;
            }
        }
        endereco = (endereco + 1) % TAMANHO_TABELA;
    }
    return 0;
}

void inserirRegistro(const char *chavePrimaria) {
    if (verificarDuplicacao(chavePrimaria)) {
        printf("Erro! Chave duplicada!\n");
        return;
    }

    REGISTRO registro;

    if (!buscarRegistroEmArquivo(chavePrimaria, &registro)) {
        printf("Erro! Registro não encontrado insere.bin.\n");
        return;
    }

    long offset;
    FILE *fp = verificaArquivo("resultado.bin", "ab+");
    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);

    int endereco = hash(chavePrimaria);
    int tentativa = 1;

    while (tentativa <= TAMANHO_TABELA) {
        for (int j = 0; j < TAMANHO_BUCKET; j++) {
            if (!hashTable[endereco][j].flag) {
                strcpy(hashTable[endereco][j].chavePrimaria, chavePrimaria);
                hashTable[endereco][j].offset = offset;
                hashTable[endereco][j].flag = 1;

                fwrite(&registro, sizeof(REGISTRO), 1, fp);
                printf("Endereço inicial calculado: %d\n", endereco);
                if (tentativa > 1) printf("Colisão - Tentativa %d\n", tentativa);
                printf("Chave %s inserida com sucesso na posição %d\n", chavePrimaria, endereco);
                fclose(fp);
                return;
            }
        }
        endereco = (endereco + 1) % TAMANHO_TABELA;
        tentativa++;
    }

    printf("Erro! Tabela hash cheia após %d tentativas!\n", tentativa - 1);
    fclose(fp);
}

int buscarRegistro(const char *chavePrimaria, REGISTRO *registro) {
    int endereco = hash(chavePrimaria);
    FILE *fp = verificaArquivo("resultado.bin", "rb");
    int acessos = 0;

    for (int tentativa = 0; tentativa < TAMANHO_TABELA; tentativa++) {
        for (int j = 0; j < TAMANHO_BUCKET; j++) {
            acessos++;
            if (hashTable[endereco][j].flag && strcmp(hashTable[endereco][j].chavePrimaria, chavePrimaria) == 0) {
                fseek(fp, hashTable[endereco][j].offset, SEEK_SET);
                fread(registro, sizeof(REGISTRO), 1, fp);
                printf("Chave encontrada: h(x): %s | end: %d | acessos: %d\n", chavePrimaria, endereco, acessos);
                fclose(fp);
                return 1;
            }
        }
        endereco = (endereco + 1) % TAMANHO_TABELA;
    }
    printf("Chave %s não encontrada\n", chavePrimaria);
    fclose(fp);
    return 0;
}

void removerRegistro(const char *chavePrimaria) {
    int endereco = hash(chavePrimaria);

    for (int tentativa = 0; tentativa < TAMANHO_TABELA; tentativa++) {
        for (int j = 0; j < TAMANHO_BUCKET; j++) {
            if (hashTable[endereco][j].flag && strcmp(hashTable[endereco][j].chavePrimaria, chavePrimaria) == 0) {
                hashTable[endereco][j].flag = 0;
                printf("Chave %s removida com sucesso!\n", chavePrimaria);
                return;
            }
        }
        endereco = (endereco + 1) % TAMANHO_TABELA;
    }
    printf("Chave %s não encontrada para remoção\n", chavePrimaria);
}

void printarTabela(){
    printf("\nTabela Hash Completa:\n");
            for (int i = 0; i < TAMANHO_TABELA; i++) {
                int bucketVazio = 1;
                printf("Vetor %2d:", i);

                for (int j = 0; j < TAMANHO_BUCKET; j++) {
                    if (hashTable[i][j].flag != 0) {
                        printf(" | Chave %s || Desloc %ld |", hashTable[i][j].chavePrimaria, hashTable[i][j].offset);
                        bucketVazio = 0;
                    }
                }

                if (bucketVazio) {
                    printf(" |    Vazio    ||    Vazio    |");
                }

                printf("\n");
            }
}


int main() {
    iniciaHash();
    int opcao, i;
    REGISTRO registro;

    FILE *arquivo;
    arquivo = verificaArquivo("insere.bin", "rb");
    REGISTRO registrosInseridos[3];
    fread(registrosInseridos, sizeof(REGISTRO), 3, arquivo);
    fclose(arquivo);

    arquivo = verificaArquivo("busca.bin", "rb");
    CHAVE_PRIMARIA buscaIndice[3];
    fread(buscaIndice, sizeof(CHAVE_PRIMARIA), 3, arquivo);
    fclose(arquivo);

    arquivo = verificaArquivo("remove.bin", "rb");
    CHAVE_PRIMARIA removeIndice[3];
    fread(removeIndice, sizeof(CHAVE_PRIMARIA), 3, arquivo);
    fclose(arquivo);

    FILE *resultado = fopen("resultado.bin", "rb");
    if (resultado) {
        fclose(resultado);
    }

    do {
        printf("\nSelecione uma das opções abaixo:\n\n");
                printf("1- Inserir um registro.\n");
                printf("2- Buscar por um registro.\n");
                printf("3- Remover um registro.\n");
                printf("4- Imprimir tabela hash.\n");
                printf("0- Sair.\n");
                printf("Opção: ");
                scanf("%d", &opcao);

        if (opcao == 1) {
            char chavePrimaria[8];
            printf("Informe um número de 1 a 3 para inserir: ");
            scanf("%d", &i);
            strcpy(chavePrimaria, registrosInseridos[i - 1].idAluno);
            strcat(chavePrimaria, registrosInseridos[i - 1].siglaDisciplina);
            inserirRegistro(chavePrimaria);
        } 
        
        else if (opcao == 2) {
            char chavePrimaria[8];
            printf("Informe um número de 1 a 3 para buscar: ");
            scanf("%d", &i);
            strcpy(chavePrimaria, buscaIndice[i - 1].idAluno);
            strcat(chavePrimaria, buscaIndice[i - 1].siglaDisciplina);
            if (buscarRegistro(chavePrimaria, &registro)) {
                printf("**Dados do registro**\nAluno: %s\nDisciplina: %s\nMédia: %.2f\nFrequência: %.2f\n",
                       registro.nomeAluno, registro.nomeDisciplina, registro.media, registro.freq);
            }
        } 
        
        else if (opcao == 3) {
            char chavePrimaria[8];
            printf("Informe um número de 1 a 3 para remover: ");
            scanf("%d", &i);
            strcpy(chavePrimaria, removeIndice[i - 1].idAluno);
            strcat(chavePrimaria, removeIndice[i - 1].siglaDisciplina);
            removerRegistro(chavePrimaria);
        } 
        
        else if (opcao == 4) {
            printarTabela();
        } 
        
        else if (opcao != 0) {
            printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    printf("\nSaindo do programa...\n\n");
    return 0;
}
