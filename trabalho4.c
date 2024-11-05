#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGISTROS 13
#define TAM_REGISTRO 256
#define MAX_ALUNOS 3
#define MAX_BUSCAS 2
#define MAX_REMOVAS 1

typedef struct {
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;
} REGISTRO;

typedef struct {
    char chave[8]; // ID do aluno + sigla da disciplina
    int rrn;       // Registro de referência no arquivo principal
} INDICE;

REGISTRO registros[MAX_ALUNOS];
INDICE indice[MAX_REGISTROS];
int num_registros = 0; // Contador para a quantidade de registros utilizados

// Função hash para calcular o endereço
int hash(char* chave) {
    return (atoi(chave) % MAX_REGISTROS);
}

// Função para inserir um registro
void inserirRegistro(REGISTRO novoRegistro) {
    char chave[256];
    sprintf(chave, "%.3s#%.3s\n", 
            novoRegistro.id_aluno,
            novoRegistro.sigla_disc);

    int endereco = hash(chave);
    int tentativas = 0;

    // Verifica se o registro já está na hash
    while (tentativas < 2) { // Cada bucket pode conter até 2 chaves
        if (indice[endereco].rrn == -1 || strcmp(indice[endereco].chave, "") == 0) {
            // Espaço livre encontrado
            strcpy(indice[endereco].chave, chave);
            indice[endereco].rrn = num_registros; // RRN é o índice atual
            num_registros++;

            // Escreve no arquivo
            FILE *fd = fopen("dados.bin", "ab");
            if (fd) {
                char registro[TAM_REGISTRO];
                sprintf(registro, "%.3s#%.3s#%.50s#%.50s#%.2f#%.2f#\n", 
                    novoRegistro.id_aluno,
                    novoRegistro.sigla_disc,
                    novoRegistro.nome_aluno,
                    novoRegistro.nome_disc,
                    novoRegistro.media,
                    novoRegistro.freq);
                fwrite(registro, sizeof(char), strlen(registro), fd);
                fclose(fd);
            }

            printf("Endereço %d\n", endereco);
            printf("Chave %s inserida com sucesso\n", chave);
            return; // Inserção bem-sucedida
        } else {
            // Colisão
            printf("Endereço %d\n", endereco);
            printf("Colisão\n");
            tentativas++;
            endereco = (endereco + 1) % MAX_REGISTROS; // Overflow progressivo
        }
    }

    printf("Falha ao inserir a chave após %d tentativas\n", tentativas);
}

// Função para buscar um registro
void buscarRegistro(char* chave) {
    int endereco = hash(chave);
    int tentativas = 0;

    while (tentativas < 2) {
        if (strcmp(indice[endereco].chave, chave) == 0) {
            printf("Chave %s encontrada, endereço %d, %d acessos\n", chave, endereco, tentativas + 1);
            // Carregar dados do arquivo
            FILE *fd = fopen("dados.bin", "rb");
            if (fd) {
                char registro[TAM_REGISTRO];
                fseek(fd, indice[endereco].rrn * TAM_REGISTRO, SEEK_SET);
                fread(registro, sizeof(char), TAM_REGISTRO, fd);
                printf("Registro: %s\n", registro);
                fclose(fd);
            }
            return;
        }
        tentativas++;
        endereco = (endereco + 1) % MAX_REGISTROS;
    }
    printf("Chave %s não encontrada\n", chave);
}

// Função para remover um registro
void removerRegistro(char* chave) {
    int endereco = hash(chave);
    int tentativas = 0;

    while (tentativas < 2) {
        if (strcmp(indice[endereco].chave, chave) == 0) {
            // Remover a chave
            strcpy(indice[endereco].chave, "");
            indice[endereco].rrn = -1; // RRN vazio
            printf("Chave %s removida com sucesso\n", chave);
            return;
        }
        tentativas++;
        endereco = (endereco + 1) % MAX_REGISTROS;
    }
    printf("Chave %s não encontrada para remoção\n", chave);
}

// Função principal
int main() {
    // Inicializa o índice
    for (int i = 0; i < MAX_REGISTROS; i++) {
        indice[i].rrn = -1;
        strcpy(indice[i].chave, "");
    }

    // Carrega os registros do arquivo insere.bin
    FILE *fd = fopen("insere.bin", "rb");
    if (fd) {
        fread(registros, sizeof(REGISTRO), MAX_ALUNOS, fd);
        fclose(fd);
    }

    int opcao, i;
    do {
        printf("\nSelecione uma das opções abaixo:\n\n");
        printf("1- Inserir um registro.\n");
        printf("2- Buscar por um registro.\n");
        printf("3- Remover um registro.\n");
        printf("0- Sair.\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                while (1) {
                    printf("\nDigite 0 para retornar ao menu principal.");
                    printf("\nInforme um número de 1 a %d: ", MAX_ALUNOS);
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > MAX_ALUNOS) {
                        printf("Opção inválida!\n");
                    } else {
                        inserirRegistro(registros[i - 1]);
                    }
                }
                break;

            case 2:
                while (1) {
                    printf("\nDigite 0 para retornar ao menu principal.");
                    printf("\nInforme um número de 1 a %d: ", MAX_BUSCAS);
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > MAX_BUSCAS) {
                        printf("Opção inválida!\n");
                    } else {
                        char chave[8];
                        sprintf(chave, "%s%s", registros[i - 1].id_aluno, registros[i - 1].sigla_disc);
                        buscarRegistro(chave);
                    }
                }
                break;

            case 3:
                while (1) {
                    printf("\nDigite 0 para retornar ao menu principal.");
                    printf("\nInforme um número de 1 a %d: ", MAX_REMOVAS);
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > MAX_REMOVAS) {
                        printf("Opção inválida!\n");
                    } else {
                        char chave[8];
                        sprintf(chave, "%s%s", registros[i - 1].id_aluno, registros[i - 1].sigla_disc);
                        removerRegistro(chave);
                    }
                }
                break;

            case 0:
                printf("\nSaindo do programa...\n\n");
                break;

            default:
                printf("Opção inválida!\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
