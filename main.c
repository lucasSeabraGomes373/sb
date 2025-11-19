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
    
    // 1. Decodificar o nome desta classe
    // Esta decodificação é necessária para a lógica de decisão.
    cp_info *this_class_entry = classFile->constant_pool + classFile->this_class - 1;
    if (this_class_entry->tag != CONSTANT_Class) {
         fprintf(stderr, "Erro: Índice this_class (%d) não aponta para CONSTANT_Class.\n", classFile->this_class);
         return;
    }
    
    // Decodifica o nome da classe (ex: Belote, Carta, Jogador)
    char *this_class_name = decodeNIeNT(classFile->constant_pool, this_class_entry->UnionCP.CONSTANT_Class.name_index, 3);
    
    code_attribute *methodCode = NULL;
    const char *method_name_to_run = NULL;

    // --- Lógica de Decisão Generalizada ---
    
    // 1. PRIMEIRA TENTATIVA: MAIN (Ponto de entrada de aplicação)
    methodCode = getMethodCode(classFile, "main", "([Ljava/lang/String;)V");
    if (methodCode != NULL) {
        method_name_to_run = "main";
    }

    // 2. SEGUNDA TENTATIVA (Construtor Específico)
    if (methodCode == NULL) {
        if (strcmp(this_class_name, "Carta") == 0) {
             methodCode = getMethodCode(classFile, "<init>", "(ILjava/lang/String;I)V");
             if (methodCode != NULL) method_name_to_run = "Construtor Carta (I, String, I)V";
        } else if (strcmp(this_class_name, "Jogador") == 0) {
             methodCode = getMethodCode(classFile, "<init>", "(Ljava/lang/String;)V");
             if (methodCode != NULL) method_name_to_run = "Construtor Jogador (String)V";
        }
    }
    
    // 3. ÚLTIMA TENTATIVA (Construtor Padrão)
    if (methodCode == NULL) {
        methodCode = getMethodCode(classFile, "<init>", "()V");
        if (methodCode != NULL) {
             method_name_to_run = "Construtor Padrão ()V";
        }
    }

    // Debug da Decisão e Tratamento de Erro
    fprintf(stderr, "[DEBUG] Classe Lida: %s. Tentando executar: %s\n", this_class_name, method_name_to_run ? method_name_to_run : "Nenhum método inicial encontrado");
    
    if (methodCode == NULL) {
        fprintf(stderr, "Erro: Não foi encontrado um método 'main' ou um construtor ('<init>') conhecido para iniciar a classe.\n");
        free(this_class_name);
        return;
    }

    free(this_class_name);
    // --- Fim da Lógica de Decisão ---

    // 2. Criar o Frame inicial
    Frame mainFrame;
    mainFrame.pc = 0;
    mainFrame.max_stack = methodCode->max_stack;
    mainFrame.max_locals = methodCode->max_locals;
    mainFrame.code = methodCode->code;
    mainFrame.code_length = methodCode->code_length;
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

        freeConstantPool(classFile->constant_pool, classFile->constant_pool_count);
        free(classFile);
    }

    return 0;
}