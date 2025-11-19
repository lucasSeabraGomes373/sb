//
// Created on 27/10/2025.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"
#include "executorInstrucoes.h"
#include "frames.h"
#include "attributeStructs.h"


void executarJVM(ClassFile *classFile) {
    
    // Inicialização do Ambiente
    inicializarInstrucoes();
    inicializarAmbiente(classFile);
    
    // 1. Encontrar o método main: main:([Ljava/lang/String;)V
    code_attribute *mainCode = getMethodCode(classFile, "main", "([Ljava/lang/String;)V");

    if (mainCode == NULL) {
        fprintf(stderr, "Erro: Método main não encontrado na classe.\n");
        return;
    }

    // 2. Criar o Frame inicial
    Frame mainFrame;
    mainFrame.pc = 0;
    mainFrame.max_stack = mainCode->max_stack;
    mainFrame.max_locals = mainCode->max_locals;
    mainFrame.code = mainCode->code;
    mainFrame.code_length = mainCode->code_length;
    mainFrame.sp = -1; // Pilha vazia

    // Alocar Pilha de Operandos e Variáveis Locais
    mainFrame.operand_stack = (int*)calloc(mainFrame.max_stack, sizeof(int));
    mainFrame.local_variables = (int*)calloc(mainFrame.max_locals, sizeof(int));

    if (!mainFrame.operand_stack || !mainFrame.local_variables) {
         fprintf(stderr, "Erro: Falha na alocação de memória para o Frame.\n");
         if (mainFrame.operand_stack) free(mainFrame.operand_stack);
         if (mainFrame.local_variables) free(mainFrame.local_variables);
         return;
    }
    
    printf("--- Iniciando Execução do Método Main ---\n");
    printf("Max Stack: %d, Max Locals: %d, Code Length: %d\n\n", 
           mainFrame.max_stack, mainFrame.max_locals, mainFrame.code_length);

    // 3. Iniciar o loop de execução
    executar(&mainFrame);
    
    printf("\n--- Execução Concluída ---\n");

    // 4. Limpeza
    free(mainFrame.operand_stack);
    free(mainFrame.local_variables);
}

int main() {
    int quantidadeArquivos;
    char nomeArquivo[256];
    int opcao;

    printf("Digite a quantidade de arquivos a serem lidos: ");
    if (scanf("%d", &quantidadeArquivos) != 1) {
        fprintf(stderr, "Erro na leitura da quantidade de arquivos.\n");
        return 1;
    }

    for (int i = 0; i < quantidadeArquivos; i++) {
        printf("\nDigite o nome do arquivo a ser lido (com extensão): ");
        if (scanf("%s", nomeArquivo) != 1) {
             fprintf(stderr, "Erro na leitura do nome do arquivo.\n");
             continue;
        }

        printf("Escolha a operação:\n");
        printf("1 - Imprimir em .txt\n");
        printf("2 - Executar como JVM\n");
        printf("Opção: ");
        if (scanf("%d", &opcao) != 1) {
             fprintf(stderr, "Erro na leitura da opção.\n");
             continue;
        }

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
                freeConstantPool(classFile->constant_pool, classFile->constant_pool_count);
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

        // A limpeza de memória deve ser feita uma única vez após o uso da ClassFile
        // independentemente da opção escolhida.
        freeConstantPool(classFile->constant_pool, classFile->constant_pool_count);
        // Implementação completa da função de liberação de memória para todo o ClassFile seria ideal aqui.
        free(classFile);
    }

    return 0;
}