//
// Created on 27/10/2025.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"
#include "executorInstrucoes.h"  // para inicializarInstrucoes e executar
#include "frames.h"               // para Frame
#include "metodoInstrucoes.h"          // para enum de opcodes






void executarJVM(ClassFile *classFile) {
    inicializarInstrucoes();

    for (int i = 0; i < classFile->methods_count; i++) {
        method_info method = classFile->methods[i];

        char *methodName = getUtf8FromConstantPool(classFile->constant_pool, method.name_index, classFile->constant_pool_count);
        if (methodName && strcmp(methodName, "main") == 0) {
            for (int j = 0; j < method.attributes_count; j++) {
                attribute_info *attr = method.attributes[j];
                char *attrName = getUtf8FromConstantPool(classFile->constant_pool, attr->attribute_name_index, classFile->constant_pool_count);
                if (attrName && strcmp(attrName, "Code") == 0) {
                    code_attribute *codeAttr = (code_attribute *)attr->info;

                    // Aloca pilha e variáveis locais dinamicamente
                    Frame frame;
                    frame.pc = 0;
                    frame.code = codeAttr->code;
                    frame.max_stack = codeAttr->max_stack;
                    frame.max_locals = codeAttr->max_locals;
                    frame.sp = -1;

                    frame.operand_stack = malloc(sizeof(int) * frame.max_stack);
                    frame.local_variables = malloc(sizeof(int) * frame.max_locals);

                    if (!frame.operand_stack || !frame.local_variables) {
                        fprintf(stderr, "Erro ao alocar memória para o frame.\n");
                        exit(1);
                    }

                    memset(frame.operand_stack, 0, sizeof(int) * frame.max_stack);
                    memset(frame.local_variables, 0, sizeof(int) * frame.max_locals);

                    printf("\n--- Iniciando execução do método main ---\n");
                    executar(&frame, codeAttr->code_length);
                    printf("--- Execução finalizada ---\n");

                    free(frame.operand_stack);
                    free(frame.local_variables);
                    free(attrName);
                    break;
                }
                if (attrName) free(attrName);
            }
            free(methodName);
            return;
        }
        if (methodName) free(methodName);
    }

    printf("Método main não encontrado.\n");
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
        free(classFile->methods);
        freeConstantPool(classFile->constant_pool, classFile->constant_pool_count);
        free(classFile);
    }

    return 0;
}

