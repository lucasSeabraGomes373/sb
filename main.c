//
// Created on 27/10/2025.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"

// Função placeholder para execução da JVM
void executarJVM(ClassFile *classFile) {
    printf("Execução da JVM ainda não implementada.\n");
}

int main() {
    int quantidadeArquivos;
    char nomeArquivo[256];
    int opcao;

    printf("Digite a quantidade de arquivos a serem lidos: ");
    scanf("%d", &quantidadeArquivos);

    for (int i = 0; i < quantidadeArquivos; i++) {
        printf("\nDigite o nome do arquivo a ser lido (com extensão): ");
        scanf("%s", nomeArquivo);

        printf("Escolha a operação:\n");
        printf("1 - Imprimir em .txt\n");
        printf("2 - Executar como JVM\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        ClassFile *classFile = readFile(nomeArquivo);
        if (classFile == NULL) {
            fprintf(stderr, "Erro ao ler o arquivo: %s\n", nomeArquivo);
            continue;
        }

        if (opcao == 1) {
            char nomeSaida[256];
            strcpy(nomeSaida, nomeArquivo);
            char *ponto = strrchr(nomeSaida, '.');
            if (ponto != NULL) *ponto = '\0';
            strcat(nomeSaida, ".txt");

            FILE *saida = fopen(nomeSaida, "a");
            if (saida == NULL) {
                fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
                free(classFile);
                continue;
            }

            printClassFile(classFile, saida);
            fclose(saida);
            printf("Arquivo %s processado com sucesso.\n", nomeArquivo);
        } else if (opcao == 2) {
            executarJVM(classFile);
        } else {
            printf("Opção inválida.\n");
        }

        for (int i = 0; i < classFile->methods_count; i++) {
            freeMethod(classFile->methods[i]);
        }
        freeConstantPool(classFile->constant_pool, classFile->constant_pool_count);
        free(classFile);
    }

    return 0;
}

