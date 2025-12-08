

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"
#include "executorInstrucoes.h"
#include "frames.h"
#include "attributeStructs.h"

#include "inits.h"

void executarJVM(ClassFile *classFile, const char *dir_base) {
    
    inicializarInstrucoes();
    inicializarAmbiente(classFile);

    
    run_clinit_and_load_deps(classFile, dir_base);

    

        cp_info *entrada_classe_atual = classFile->constant_pool + classFile->this_class - 1;
        if (entrada_classe_atual->tag != CONSTANT_Class) {
            fprintf(stderr, "Erro: Índice this_class (%d) não aponta para CONSTANT_Class.\n", classFile->this_class);
            return;
        }
        char *nome_classe_atual = decodeNIeNT(classFile->constant_pool, entrada_classe_atual->UnionCP.CONSTANT_Class.name_index, 3);

        code_attribute *codigo_metodo = NULL;
        const char *nome_metodo_a_executar = NULL;
        int eh_construtor = 0;

    
    codigo_metodo = getMethodCode(classFile, "main", "([Ljava/lang/String;)V");
    if (codigo_metodo != NULL) {
        nome_metodo_a_executar = "main";
    }

    
    if (codigo_metodo == NULL) {
           if (strcmp(nome_classe_atual, "Carta") == 0) {
               codigo_metodo = getMethodCode(classFile, "<init>", "(ILjava/lang/String;I)V");
               if (codigo_metodo != NULL) { nome_metodo_a_executar = "<init>(I,Ljava/lang/String;I)V"; eh_construtor = 1; }
           } else if (strcmp(nome_classe_atual, "Jogador") == 0) {
               codigo_metodo = getMethodCode(classFile, "<init>", "(Ljava/lang/String;)V");
               if (codigo_metodo != NULL) { nome_metodo_a_executar = "<init>(Ljava/lang/String;)V"; eh_construtor = 1; }
        }
    }

    
    if (codigo_metodo == NULL) {
        codigo_metodo = getMethodCode(classFile, "<init>", "()V");
        if (codigo_metodo != NULL) {
            nome_metodo_a_executar = "<init>()V";
            eh_construtor = 1;
        }
    }

    fprintf(stderr, "[DEBUG] Classe Lida: %s. Tentando executar: %s\n", nome_classe_atual, nome_metodo_a_executar ? nome_metodo_a_executar : "Nenhum método inicial encontrado");

    if (codigo_metodo == NULL) {
        fprintf(stderr, "Erro: Não foi encontrado um método 'main' ou um construtor ('<init>') conhecido para iniciar a classe.\n");
        free(nome_classe_atual);
        return;
    }

    free(nome_classe_atual);

    
        Frame quadro_principal;
        quadro_principal.pc = 0;
        quadro_principal.max_stack = codigo_metodo->max_stack;
        quadro_principal.max_locals = codigo_metodo->max_locals;
        quadro_principal.code = codigo_metodo->code;
        quadro_principal.code_length = codigo_metodo->code_length;
        quadro_principal.sp = -1;

        quadro_principal.max_stack = quadro_principal.max_stack + 64;
        quadro_principal.operand_stack = (int*)calloc(quadro_principal.max_stack, sizeof(int));
        quadro_principal.local_variables = (int*)calloc(quadro_principal.max_locals, sizeof(int));

        if (!quadro_principal.operand_stack || !quadro_principal.local_variables) {
            fprintf(stderr, "Erro: Falha na alocação de memória para o Frame.\n");
            if (quadro_principal.operand_stack) free(quadro_principal.operand_stack);
            if (quadro_principal.local_variables) free(quadro_principal.local_variables);
            return;
        }

        printf("--- Iniciando Execução do Método Main ---\n");
        printf("Max Stack: %d, Max Locals: %d, Code Length: %d\n\n", 
            quadro_principal.max_stack, quadro_principal.max_locals, quadro_principal.code_length);

    if (eh_construtor) {
        if (quadro_principal.max_locals > 0) {
            int ref_objeto = create_object_ref();
            quadro_principal.local_variables[0] = ref_objeto;
            fprintf(stderr, "[DEBUG] Inicializando local 0 (this) com Ref: %d\n", ref_objeto);
        } else {
            fprintf(stderr, "[WARN] Método construtor possui max_locals == 0\n");
        }
    }

    executar(&quadro_principal);

    printf("\n--- Execução Concluída ---\n");

    free(quadro_principal.operand_stack);
    free(quadro_principal.local_variables);
}

int main() {
    char nomeArquivo[256];
    int opcao;

    printf("Digite o nome do arquivo a ser lido (com extensão): ");
    if (scanf("%255s", nomeArquivo) != 1) {
        fprintf(stderr, "Erro na leitura do nome do arquivo.\n");
        return 1;
    }

    printf("Escolha a operação:\n");
    printf("1 - Imprimir em .txt\n");
    printf("2 - Executar como JVM\n");
    printf("Opção: ");
    if (scanf("%d", &opcao) != 1) {
        fprintf(stderr, "Erro na leitura da opção.\n");
        return 1;
    }

    ClassFile *classFile = NULL;
    
    if (strlen(nomeArquivo) > 5 && strcmp(nomeArquivo + strlen(nomeArquivo) - 5, ".java") == 0) {
        
        classFile = compile_java_to_classfile(nomeArquivo);
        if (classFile == NULL) {
            fprintf(stderr, "Erro ao compilar/parsear o arquivo .java: %s\n", nomeArquivo);
            return 1;
        }
    } else {
        classFile = readFile(nomeArquivo);
        if (classFile == NULL) {
            fprintf(stderr, "Erro ao ler o arquivo: %s\n", nomeArquivo);
            return 1;
        }
    }

    char base_dir[512];
    if (strlen(nomeArquivo) > 5 && strcmp(nomeArquivo + strlen(nomeArquivo) - 5, ".java") == 0) {
        strncpy(base_dir, "class_out", sizeof(base_dir));
        base_dir[sizeof(base_dir)-1] = '\0';
    } else {
        char *slash = strrchr(nomeArquivo, '/');
        if (!slash) slash = strrchr(nomeArquivo, '\\');
        if (slash) {
            size_t dlen = slash - nomeArquivo;
            if (dlen >= sizeof(base_dir)) dlen = sizeof(base_dir)-1;
            strncpy(base_dir, nomeArquivo, dlen);
            base_dir[dlen] = '\0';
        } else {
            strncpy(base_dir, ".", sizeof(base_dir)); base_dir[1] = '\0';
        }
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
            freeClassFile(classFile);
            return 1;
        }

        printClassFile(classFile, saida);
        fclose(saida);
        printf("Arquivo %s processado com sucesso.\n", nomeArquivo);
    } else if (opcao == 2) {
        executarJVM(classFile, base_dir);
    } else {
        printf("Opção inválida.\n");
    }

    freeClassFile(classFile);
    return 0;
}