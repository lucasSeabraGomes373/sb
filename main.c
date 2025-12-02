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


// Simple global registry of classes that already ran <clinit>
static char *g_initialized_classes[1024];
static int g_initialized_count = 0;
static char *g_loading_classes[1024];
static int g_loading_count = 0;

static int is_initialized(const char *name) {
    for (int i = 0; i < g_initialized_count; i++) {
        if (strcmp(g_initialized_classes[i], name) == 0) return 1;
    }
    return 0;
}

static void mark_initialized(const char *name) {
    if (g_initialized_count < 1024) {
        g_initialized_classes[g_initialized_count++] = strdup(name);
    }
}

static int is_loading(const char *name) {
    for (int i = 0; i < g_loading_count; i++) if (strcmp(g_loading_classes[i], name) == 0) return 1;
    return 0;
}

static void mark_loading(const char *name) {
    if (g_loading_count < 1024) g_loading_classes[g_loading_count++] = strdup(name);
}

static void unmark_loading(const char *name) {
    for (int i = 0; i < g_loading_count; i++) {
        if (strcmp(g_loading_classes[i], name) == 0) {
            free(g_loading_classes[i]);
            // compact
            for (int j = i; j < g_loading_count-1; j++) g_loading_classes[j] = g_loading_classes[j+1];
            g_loading_count--;
            return;
        }
    }
}

static void run_clinit_and_load_deps(ClassFile *classFile, const char *base_dir);

// (Crash handler removed.)

void executarJVM(ClassFile *classFile, const char *base_dir) {
    
    // Inicialização do Ambiente
    inicializarInstrucoes();
    inicializarAmbiente(classFile);

    // Ensure static initializers for this class and its dependencies run
    run_clinit_and_load_deps(classFile, base_dir);

    
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
    int is_constructor = 0;

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
             if (methodCode != NULL) { method_name_to_run = "<init>(I,Ljava/lang/String;I)V"; is_constructor = 1; }
        } else if (strcmp(this_class_name, "Jogador") == 0) {
             methodCode = getMethodCode(classFile, "<init>", "(Ljava/lang/String;)V");
             if (methodCode != NULL) { method_name_to_run = "<init>(Ljava/lang/String;)V"; is_constructor = 1; }
        }
    }
    
    // 3. ÚLTIMA TENTATIVA (Construtor Padrão)
    if (methodCode == NULL) {
        methodCode = getMethodCode(classFile, "<init>", "()V");
        if (methodCode != NULL) {
            method_name_to_run = "<init>()V";
            is_constructor = 1;
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
     /* Allocate a small safety margin above the class-declared max_stack to
         avoid immediate interpreter aborts on slightly under-reported max_stack
         values in some class files. This is a conservative runtime safety
         margin; the interpreter still enforces bounds against the allocated
         buffer. */
     mainFrame.max_stack = mainFrame.max_stack + 64;
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

    // Se for um construtor (<init>), precisamos simular que um objeto foi
    // criado e que `this` está em local_variables[0]. A instrução `new`
    // e `invokespecial` do executor usam referências inteiras simuladas
    // (ex.: 2). Aqui atribuímos uma referência compatível para evitar que
    // instructions como `aload_0` falhem.
    if (is_constructor) {
        if (mainFrame.max_locals > 0) {
            int obj_ref = create_object_ref();
            mainFrame.local_variables[0] = obj_ref; // referência simulada de objeto
            fprintf(stderr, "[DEBUG] Inicializando local 0 (this) com Ref: %d\n", obj_ref);
        } else {
            fprintf(stderr, "[WARN] Método construtor possui max_locals == 0\n");
        }
    }

    // 3. Iniciar o loop de execução
    executar(&mainFrame);
    
    printf("\n--- Execução Concluída ---\n");

    // 4. Limpeza
    free(mainFrame.operand_stack);
    free(mainFrame.local_variables);
}

static void run_clinit_and_load_deps(ClassFile *classFile, const char *base_dir) {
    if (!classFile) return;
    if (!base_dir) base_dir = ".";

    // decode this class name
    cp_info *this_entry = classFile->constant_pool + classFile->this_class - 1;
    char *this_name = NULL;
    if (this_entry && this_entry->tag == CONSTANT_Class) {
        this_name = decodeNIeNT(classFile->constant_pool, this_entry->UnionCP.CONSTANT_Class.name_index, 3);
    }
    if (!this_name) return;

    if (is_initialized(this_name)) { free(this_name); return; }
    if (is_loading(this_name)) { free(this_name); return; }
    // mark loading to avoid cycles
    mark_loading(this_name);

    // Scan constant pool for referenced classes and try to load .class files from base_dir
    for (int i = 0; i < classFile->constant_pool_count - 1; i++) {
        cp_info *entry = classFile->constant_pool + i;
        if (!entry) continue;
        if (entry->tag == CONSTANT_Class) {
            // index is i+1
            char *dep_name = decodeNIeNT(classFile->constant_pool, i+1, 1);
            if (!dep_name) continue;
            if (is_initialized(dep_name) || is_loading(dep_name)) { free(dep_name); continue; }

            // build candidate path
            char dep_path[4096];
            // replace any '\\' with '/' in dep_name (already likely contains '/').
            char dep_file[4096];
            strncpy(dep_file, dep_name, sizeof(dep_file)-1); dep_file[sizeof(dep_file)-1]='\0';
            for (char *p = dep_file; *p; ++p) if (*p == '\\') *p = '/';
            size_t bl = strlen(base_dir);
            size_t fl = strlen(dep_file);
            const char *suf = ".class";
            size_t need = bl + 1 + fl + strlen(suf) + 1; // '/' + null
            if (need <= sizeof(dep_path)) {
                memcpy(dep_path, base_dir, bl);
                dep_path[bl] = '/';
                memcpy(dep_path + bl + 1, dep_file, fl);
                memcpy(dep_path + bl + 1 + fl, suf, strlen(suf));
                dep_path[bl + 1 + fl + strlen(suf)] = '\0';
            } else {
                // path too long, skip this dependency
                dep_path[0] = '\0';
            }

            FILE *f = fopen(dep_path, "rb");
            if (f) {
                fclose(f);
                ClassFile *dep_cf = readFile(dep_path);
                if (dep_cf) {
                    // recursively ensure dependencies' clinit run
                    run_clinit_and_load_deps(dep_cf, base_dir);

                    // run dep's <clinit> if present
                    code_attribute *cl = getMethodCode(dep_cf, "<clinit>", "()V");
                    if (cl && !is_initialized(dep_name)) {
                        inicializarAmbiente(dep_cf);
                        Frame fframe;
                        fframe.pc = 0;
                        fframe.max_stack = cl->max_stack;
                        fframe.max_locals = cl->max_locals;
                        fframe.code = cl->code;
                        fframe.code_length = cl->code_length;
                        fframe.sp = -1;
                        fframe.max_stack = fframe.max_stack + 64;
                        fframe.operand_stack = (int*)calloc(fframe.max_stack, sizeof(int));
                        fframe.local_variables = (int*)calloc(fframe.max_locals, sizeof(int));
                        if (fframe.operand_stack && fframe.local_variables) {
                            executar(&fframe);
                        }
                        if (fframe.operand_stack) free(fframe.operand_stack);
                        if (fframe.local_variables) free(fframe.local_variables);
                        mark_initialized(dep_name);
                    }

                    freeClassFile(dep_cf);
                }
            }

            free(dep_name);
        }
    }

    // Now run this class <clinit> if present
    code_attribute *this_cl = getMethodCode(classFile, "<clinit>", "()V");
    if (this_cl && !is_initialized(this_name)) {
        inicializarAmbiente(classFile);
        Frame cframe;
        cframe.pc = 0;
        cframe.max_stack = this_cl->max_stack;
        cframe.max_locals = this_cl->max_locals;
        cframe.code = this_cl->code;
        cframe.code_length = this_cl->code_length;
        cframe.sp = -1;
        cframe.max_stack = cframe.max_stack + 64;
        cframe.operand_stack = (int*)calloc(cframe.max_stack, sizeof(int));
        cframe.local_variables = (int*)calloc(cframe.max_locals, sizeof(int));
        if (cframe.operand_stack && cframe.local_variables) {
            executar(&cframe);
        }
        if (cframe.operand_stack) free(cframe.operand_stack);
        if (cframe.local_variables) free(cframe.local_variables);
        // mark initialized and unmark loading
        mark_initialized(this_name);
    }

    unmark_loading(this_name);
    free(this_name);
}



int main() {
    // (Crash handler removed.)
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

        ClassFile *classFile = NULL;
        // If a .java file was provided, use the internal frontend (no external javac)
        if (strlen(nomeArquivo) > 5 && strcmp(nomeArquivo + strlen(nomeArquivo) - 5, ".java") == 0) {
            // compile a limited subset of Java into an in-memory ClassFile
            classFile = compile_java_to_classfile(nomeArquivo);
            if (classFile == NULL) {
                fprintf(stderr, "Erro ao compilar/parsear o arquivo .java: %s\n", nomeArquivo);
                continue;
            }
        } else {
            classFile = readFile(nomeArquivo);
            if (classFile == NULL) {
                fprintf(stderr, "Erro ao ler o arquivo: %s\n", nomeArquivo);
                continue;
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
                continue;
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
    }

    return 0;
}