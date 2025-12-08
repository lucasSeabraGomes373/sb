

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inits.h"
#include "executorInstrucoes.h"
#include "frames.h"

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
            for (int j = i; j < g_loading_count-1; j++) g_loading_classes[j] = g_loading_classes[j+1];
            g_loading_count--;
            return;
        }
    }
}

void run_clinit_and_load_deps(ClassFile *classFile, const char *base_dir) {
    if (!classFile) return;
    if (!base_dir) base_dir = ".";

    
    cp_info *this_entry = classFile->constant_pool + classFile->this_class - 1;
    char *this_name = NULL;
    if (this_entry && this_entry->tag == CONSTANT_Class) {
        this_name = decodeNIeNT(classFile->constant_pool, this_entry->UnionCP.CONSTANT_Class.name_index, 3);
    }
    if (!this_name) return;

    if (is_initialized(this_name)) { free(this_name); return; }
    if (is_loading(this_name)) { free(this_name); return; }
    
    mark_loading(this_name);

    
    for (int i = 0; i < classFile->constant_pool_count - 1; i++) {
        cp_info *entry = classFile->constant_pool + i;
        if (!entry) continue;
        if (entry->tag == CONSTANT_Class) {
            
            char *dep_name = decodeNIeNT(classFile->constant_pool, i+1, 1);
            if (!dep_name) continue;
            if (is_initialized(dep_name) || is_loading(dep_name)) { free(dep_name); continue; }

            
            char dep_path[4096];
            char dep_file[4096];
            strncpy(dep_file, dep_name, sizeof(dep_file)-1); dep_file[sizeof(dep_file)-1]='\0';
            for (char *p = dep_file; *p; ++p) if (*p == '\\') *p = '/';
            size_t bl = strlen(base_dir);
            size_t fl = strlen(dep_file);
            const char *suf = ".class";
            size_t need = bl + 1 + fl + strlen(suf) + 1; 
            if (need <= sizeof(dep_path)) {
                memcpy(dep_path, base_dir, bl);
                dep_path[bl] = '/';
                memcpy(dep_path + bl + 1, dep_file, fl);
                memcpy(dep_path + bl + 1 + fl, suf, strlen(suf));
                dep_path[bl + 1 + fl + strlen(suf)] = '\0';
            } else {
                dep_path[0] = '\0';
            }

            FILE *f = fopen(dep_path, "rb");
            if (f) {
                fclose(f);
                ClassFile *dep_cf = readFile(dep_path);
                if (dep_cf) {
                    
                    run_clinit_and_load_deps(dep_cf, base_dir);

                    
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
        mark_initialized(this_name);
    }

    unmark_loading(this_name);
    free(this_name);
}
