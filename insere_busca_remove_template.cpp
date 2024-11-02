#include<stdio.h>
#include<conio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct hist {
        char id_aluno[4];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[3] = { {"001", "001", "Nome-1", "Disc-001", 8.9, 80.3},
                 {"002", "001", "Nome-2", "Disc-001", 3.3, 72.3},
                 {"001", "002", "Nome-1", "Disc-002", 9.7, 73.7}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_b[2] = { {"001","001"},
                   {"001","002"}};
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
    
    //////////////////////////////
    struct remove {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_r[1] = { {"001","001"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

