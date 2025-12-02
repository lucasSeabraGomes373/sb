#include "java_frontend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include "constantPool.h"
#include "attributeStructs.h"

// Very small frontend that supports a tiny subset of Java:
// - single top-level class without package
// - a single method: public static void main(String[] args) { ... }
// - statements: int x = expr; x = expr; System.out.println(expr);
// - expressions: integer literals, variables, + - * / % and parentheses
// This frontend emits bytecode that uses existing integer opcodes and a
// custom `print_int` opcode to print integers.

// Utility dynamic byte vector
typedef struct {
    unsigned char *data;
    size_t len;
} ByteBuf;

static void bb_init(ByteBuf *b){ b->data = NULL; b->len = 0; }
static void bb_emit(ByteBuf *b, unsigned char v){ b->data = realloc(b->data, b->len+1); b->data[b->len++] = v; }
static void bb_emitu2(ByteBuf *b, unsigned short v){ bb_emit(b, (v>>8)&0xFF); bb_emit(b, v&0xFF); }

// Simple expression parser (shunting-yard into AST-free recursive generation)
// We'll implement a recursive descent parser returning a sequence of emissions

typedef struct {
    const char *s;
    size_t i;
} Parser;

static void skip_ws(Parser *p){ while (isspace((unsigned char)p->s[p->i])) p->i++; }

static int is_ident_char(char c){ return isalnum((unsigned char)c) || c=='_' ; }

// forward
static int emit_expr(Parser *p, ByteBuf *code, char **locals, int *local_count);

static int parse_number(Parser *p, int *out) {
    skip_ws(p);
    int sign = 1;
    if (p->s[p->i] == '-') {
        sign = -1;
        p->i++;
    }

    int val = 0;
    int started = 0;
    while (isdigit((unsigned char)p->s[p->i])) {
        started = 1;
        val = val * 10 + (p->s[p->i] - '0');
        p->i++;
    }

    if (!started) return 0;
    *out = val * sign;
    return 1;
}

static int parse_ident(Parser *p, char *buf, size_t buf_sz){ skip_ws(p); size_t start = p->i; if (!isalpha((unsigned char)p->s[p->i]) && p->s[p->i]!='_') return 0; p->i++; while(is_ident_char(p->s[p->i])) p->i++; size_t len = p->i - start; if (len+1 > buf_sz) return 0; memcpy(buf, p->s+start, len); buf[len]='\0'; return 1; }

// Emit code to push a literal integer onto stack
static void emit_push_int(ByteBuf *code, int val){ if (val >= -128 && val <= 127){ bb_emit(code, 0x10); bb_emit(code, (unsigned char)val); } else if (val >= -32768 && val <= 32767){ bb_emit(code, 0x11); bb_emitu2(code, (unsigned short)val); } else { // fallback: clamp
        bb_emit(code, 0x10); bb_emit(code, (unsigned char)(val & 0xFF)); } }

// Find or create local variable index; locals[0] reserved for args (index 0)
static int get_local_index(char **locals, int *local_count, const char *name){ for (int i=0;i<*local_count;i++) if (strcmp(locals[i], name)==0) return i+1; // +1 because 0 reserved
    // new
    locals[*local_count] = strdup(name);
    (*local_count)++;
    return *local_count; }

// parse factor: number | ident | '(' expr ')'
static int emit_factor(Parser *p, ByteBuf *code, char **locals, int *local_count){ skip_ws(p);
    if (p->s[p->i]=='('){ p->i++; emit_expr(p, code, locals, local_count); skip_ws(p); if (p->s[p->i]==')') p->i++; return 1; }
    int val; if (parse_number(p, &val)){ emit_push_int(code, val); return 1; }
    char name[128]; if (parse_ident(p, name, sizeof(name))){ // variable
        // emit iload for variable
        int idx = get_local_index(locals, local_count, name);
        if (idx >=0 && idx <=3){ // iload_0 .. iload_3 opcodes
            unsigned char opcode = 0x1a + idx; // iload_0 is 0x1a
            bb_emit(code, opcode);
        } else {
            bb_emit(code, 0x15); bb_emit(code, (unsigned char)idx);
        }
        return 1;
    }
    return 0; }

// parse term: factor ( (*|/|%) factor )*
static int emit_term(Parser *p, ByteBuf *code, char **locals, int *local_count){ if (!emit_factor(p, code, locals, local_count)) return 0; while(1){ skip_ws(p); char op = p->s[p->i]; if (op=='*' || op=='/' || op=='%'){ p->i++; emit_factor(p, code, locals, local_count); if (op=='*') bb_emit(code, 0x68); else if (op=='/') bb_emit(code, 0x6c); else bb_emit(code, 0x70); } else break; } return 1; }

// parse expr: term ( (+|-) term )*
static int emit_expr(Parser *p, ByteBuf *code, char **locals, int *local_count){ if (!emit_term(p, code, locals, local_count)) return 0; while(1){ skip_ws(p); char op = p->s[p->i]; if (op=='+'||op=='-'){ p->i++; emit_term(p, code, locals, local_count); if (op=='+') bb_emit(code, 0x60); else bb_emit(code, 0x64); } else break; } return 1; }

// Trim helper
static void trim(char *s){ char *p=s; while(isspace((unsigned char)*p)) p++; memmove(s,p,strlen(p)+1); while(strlen(s)>0 && isspace((unsigned char)s[strlen(s)-1])) s[strlen(s)-1]='\0'; }

ClassFile* compile_java_to_classfile(const char *java_path){
    FILE *f = fopen(java_path, "r");
    if (!f) return NULL;
    fseek(f,0,SEEK_END); long sz = ftell(f); fseek(f,0,SEEK_SET);
    char *src = malloc(sz+1); if (!src){ fclose(f); return NULL; }
    fread(src,1,sz,f); src[sz]='\0'; fclose(f);

    // find class name
    char class_name[128] = "";
    char *cpos = strstr(src, "class ");
    if (cpos){ cpos += 6; while(isspace((unsigned char)*cpos)) cpos++; char *p=cpos; int i=0; while(*p && (isalnum((unsigned char)*p) || *p=='_')){ if (i<127) class_name[i++]=*p; p++; } class_name[i]='\0'; }
    if (strlen(class_name)==0){ // fallback derive from filename
        const char *base = strrchr(java_path, '/'); if (!base) base = strrchr(java_path, '\\'); if (!base) base = java_path; else base++; strncpy(class_name, base, sizeof(class_name)-1); class_name[sizeof(class_name)-1]='\0'; char *dot = strrchr(class_name, '.'); if (dot) *dot='\0'; }

    // find main body
    char *main_sig = strstr(src, "public static void main");
    if (!main_sig){ free(src); return NULL; }
    char *brace = strchr(main_sig, '{'); if (!brace){ free(src); return NULL; }
    // find matching closing brace
    char *q = brace+1; int depth = 1; while(*q && depth>0){ if (*q=='{') depth++; else if (*q=='}') depth--; q++; }
    if (depth!=0){ free(src); return NULL; }
    size_t body_len = (q-1) - (brace+1);
    char *body = malloc(body_len+1); memcpy(body, brace+1, body_len); body[body_len]='\0';

    // split statements by ';'
    // locals management
    char *locals[256]; int local_count = 0; // maps to indices 1..local_count
    for (int i=0;i<256;i++) locals[i]=NULL;

    ByteBuf code; bb_init(&code);

    char *cur = body; while(*cur){ char *semi = strchr(cur, ';'); if (!semi) break; size_t stmt_len = semi - cur; char *stmt = malloc(stmt_len+1); memcpy(stmt, cur, stmt_len); stmt[stmt_len]='\0'; trim(stmt);
        if (strlen(stmt)>0){ // handle patterns
            if (strncmp(stmt, "int ", 4)==0){ // declaration
                char *eq = strchr(stmt, '='); if (eq){ // int x = expr
                    // parse var name
                    char name[128]; int j=4; while(isspace((unsigned char)stmt[j])) j++; int k=0; while(is_ident_char(stmt[j]) && k<127){ name[k++]=stmt[j++]; } name[k]='\0'; // parse expr after '='
                    Parser p = { .s = eq+1, .i = 0 };
                    emit_expr(&p, &code, locals, &local_count);
                    int idx = get_local_index(locals, &local_count, name);
                    // istore
                    if (idx>=0 && idx<=3) bb_emit(&code, 0x3b + idx); else { bb_emit(&code, 0x36); bb_emit(&code, (unsigned char)idx); }
                }
            } else if (strncmp(stmt, "System.out.println", 18)==0){ // print
                char *lp = strchr(stmt, '('); char *rp = strrchr(stmt, ')'); if (lp && rp && rp>lp){ char *expr = malloc(rp-lp); memcpy(expr, lp+1, rp-lp-1); expr[rp-lp-1]='\0'; Parser p = { .s = expr, .i = 0 };
                        emit_expr(&p, &code, locals, &local_count);
                        // custom print opcode
                        bb_emit(&code, 0xCB);
                        free(expr);
                }
            } else { // assignment or expression
                char *eq = strchr(stmt, '='); if (eq){ char name[128]; int i2=0; while(isspace((unsigned char)stmt[i2])) i2++; int k=0; while(is_ident_char(stmt[i2]) && k<127){ name[k++]=stmt[i2++]; } name[k]='\0'; Parser p = { .s = eq+1, .i = 0 };
                        emit_expr(&p, &code, locals, &local_count);
                        int idx = get_local_index(locals, &local_count, name);
                        if (idx>=0 && idx<=3) bb_emit(&code, 0x3b + idx); else { bb_emit(&code, 0x36); bb_emit(&code, (unsigned char)idx); }
                }
            }
        }
        free(stmt);
        cur = semi+1;
    }

    // return
    bb_emit(&code, 0xb1); // return (void)

    // Build ClassFile structure
    ClassFile *cf = malloc(sizeof(ClassFile)); memset(cf,0,sizeof(ClassFile));
    cf->magic = 0xCAFEBABE;
    cf->minor_version = 0;
    cf->major_version = 52; // Java 8-ish

    // build constant pool entries as described
    int cp_entries = 7; // 1:Utf8 class,2:Class class,3:Utf8 super,4:Class super,5:Utf8 main,6:Utf8 desc,7:Utf8 Code
    cf->constant_pool_count = cp_entries + 1;
    cf->constant_pool = malloc(cp_entries * sizeof(cp_info));
    // index 1
    cf->constant_pool[0].tag = 1; cf->constant_pool[0].UnionCP.CONSTANT_UTF8.length = strlen(class_name); cf->constant_pool[0].UnionCP.CONSTANT_UTF8.bytes = malloc(strlen(class_name)); memcpy(cf->constant_pool[0].UnionCP.CONSTANT_UTF8.bytes, class_name, strlen(class_name));
    // index 2 CLASS -> name_index 1
    cf->constant_pool[1].tag = 7; cf->constant_pool[1].UnionCP.CONSTANT_Class.name_index = 1;
    // index 3 super utf8 "java/lang/Object"
    const char *supername = "java/lang/Object";
    cf->constant_pool[2].tag = 1; cf->constant_pool[2].UnionCP.CONSTANT_UTF8.length = strlen(supername); cf->constant_pool[2].UnionCP.CONSTANT_UTF8.bytes = malloc(strlen(supername)); memcpy(cf->constant_pool[2].UnionCP.CONSTANT_UTF8.bytes, supername, strlen(supername));
    // index 4 CLASS -> name_index 3
    cf->constant_pool[3].tag = 7; cf->constant_pool[3].UnionCP.CONSTANT_Class.name_index = 3;
    // index 5 utf8 "main"
    const char *mainname = "main";
    cf->constant_pool[4].tag = 1; cf->constant_pool[4].UnionCP.CONSTANT_UTF8.length = strlen(mainname); cf->constant_pool[4].UnionCP.CONSTANT_UTF8.bytes = malloc(strlen(mainname)); memcpy(cf->constant_pool[4].UnionCP.CONSTANT_UTF8.bytes, mainname, strlen(mainname));
    // index 6 utf8 "([Ljava/lang/String;)V"
    const char *desc = "([Ljava/lang/String;)V";
    cf->constant_pool[5].tag = 1; cf->constant_pool[5].UnionCP.CONSTANT_UTF8.length = strlen(desc); cf->constant_pool[5].UnionCP.CONSTANT_UTF8.bytes = malloc(strlen(desc)); memcpy(cf->constant_pool[5].UnionCP.CONSTANT_UTF8.bytes, desc, strlen(desc));
    // index 7 utf8 "Code"
    const char *coden = "Code";
    cf->constant_pool[6].tag = 1; cf->constant_pool[6].UnionCP.CONSTANT_UTF8.length = strlen(coden); cf->constant_pool[6].UnionCP.CONSTANT_UTF8.bytes = malloc(strlen(coden)); memcpy(cf->constant_pool[6].UnionCP.CONSTANT_UTF8.bytes, coden, strlen(coden));

    // class/super
    cf->access_flags = 0x0021; // public, super
    cf->this_class = 2;
    cf->super_class = 4;
    cf->interfaces_count = 0; cf->interfaces = NULL;
    cf->fields_count = 0; cf->fields = NULL;

    // methods
    cf->methods_count = 1;
    cf->methods = malloc(sizeof(method_info));
    cf->methods[0].access_flags = 0x0009; // public static
    cf->methods[0].name_index = 5;
    cf->methods[0].descriptor_index = 6;
    cf->methods[0].attributes_count = 1;
    cf->methods[0].attributes = malloc(sizeof(attribute_info*));

    // Code attribute
    code_attribute *ca = malloc(sizeof(code_attribute));
    ca->max_stack = 32; ca->max_locals = (byte2)(local_count + 1); ca->code_length = (byte4)code.len;
    ca->code = malloc(code.len);
    memcpy(ca->code, code.data, code.len);
    ca->exception_table_length = 0; ca->table = NULL;
    ca->attributes_count = 0; ca->attributes = NULL;

    attribute_info *ai = malloc(sizeof(attribute_info));
    ai->attribute_name_index = 7; // index of "Code" in cp
    ai->attribute_length = 12 + ca->code_length; // rough, not strictly necessary for in-memory usage
    ai->info = ca;
    cf->methods[0].attributes[0] = ai;

    // class attributes
    cf->attributes_count = 0; cf->attributes = NULL;

    free(src); free(body);
    if (code.data) free(code.data);

    // Ensure output directory exists: "exemplos"
    const char *out_dir = "class_out";
#if defined(_WIN32)
    _mkdir(out_dir);
#else
    mkdir(out_dir, 0755);
#endif

    // Build output path: exemplos/<ClassName>.class
    char outpath[512];
    snprintf(outpath, sizeof(outpath), "%s/%s.class", out_dir, class_name);

    // Write class file to disk
    FILE *of = fopen(outpath, "wb");
    if (of) {
        // write magic
        uint32_t magic = 0xCAFEBABE;
        unsigned char buf4[4];
        buf4[0] = (magic >> 24) & 0xFF; buf4[1] = (magic >> 16) & 0xFF; buf4[2] = (magic >> 8) & 0xFF; buf4[3] = magic & 0xFF;
        fwrite(buf4, 1, 4, of);

        unsigned short tmp2;
        unsigned char b2[2];
        // minor_version
        tmp2 = (unsigned short)cf->minor_version;
        b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
        // major_version
        tmp2 = (unsigned short)cf->major_version; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

        // constant_pool_count
        tmp2 = (unsigned short)cf->constant_pool_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

        // constant pool entries (array length = constant_pool_count - 1)
        int cp_len = cf->constant_pool_count - 1;
        for (int i = 0; i < cp_len; i++) {
            cp_info *e = &cf->constant_pool[i];
            unsigned char tag = e->tag;
            fwrite(&tag, 1, 1, of);
            if (tag == 1) { // UTF8
                unsigned short len = e->UnionCP.CONSTANT_UTF8.length;
                unsigned char l2[2]; l2[0] = (len >> 8) & 0xFF; l2[1] = len & 0xFF; fwrite(l2,1,2,of);
                fwrite(e->UnionCP.CONSTANT_UTF8.bytes, 1, len, of);
            } else if (tag == 7) { // Class
                unsigned short name_index = e->UnionCP.CONSTANT_Class.name_index;
                unsigned char n2[2]; n2[0] = (name_index >> 8) & 0xFF; n2[1] = name_index & 0xFF; fwrite(n2,1,2,of);
            } else {
                // For this minimal frontend we only emit UTF8 and Class entries
            }
        }

        // access_flags
        tmp2 = (unsigned short)cf->access_flags; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
        // this_class
        tmp2 = (unsigned short)cf->this_class; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
        // super_class
        tmp2 = (unsigned short)cf->super_class; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

        // interfaces_count
        tmp2 = (unsigned short)cf->interfaces_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
        // interfaces (none)
        // fields_count
        tmp2 = (unsigned short)cf->fields_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

        // methods_count
        tmp2 = (unsigned short)cf->methods_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

        // methods
        for (int mi = 0; mi < cf->methods_count; mi++) {
            method_info *m = &cf->methods[mi];
            tmp2 = m->access_flags; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
            tmp2 = m->name_index; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
            tmp2 = m->descriptor_index; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
            tmp2 = m->attributes_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

            for (int ai = 0; ai < m->attributes_count; ai++) {
                attribute_info *attr = m->attributes[ai];
                tmp2 = attr->attribute_name_index; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
                // attribute_length (4 bytes)
                uint32_t alen = (uint32_t)attr->attribute_length;
                unsigned char a4[4]; a4[0] = (alen >> 24) & 0xFF; a4[1] = (alen >> 16) & 0xFF; a4[2] = (alen >> 8) & 0xFF; a4[3] = alen & 0xFF;
                fwrite(a4,1,4,of);

                // For Code attribute, write structured info
                if (attr->info != NULL) {
                    code_attribute *ca = (code_attribute*)attr->info;
                    // max_stack
                    tmp2 = ca->max_stack; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
                    // max_locals
                    tmp2 = ca->max_locals; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
                    // code_length
                    uint32_t clen = ca->code_length; unsigned char c4[4]; c4[0] = (clen>>24)&0xFF; c4[1]=(clen>>16)&0xFF; c4[2]=(clen>>8)&0xFF; c4[3]=clen&0xFF; fwrite(c4,1,4,of);
                    // code bytes
                    if (clen>0) fwrite(ca->code,1,clen,of);
                    // exception_table_length
                    tmp2 = ca->exception_table_length; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
                    // (we have none)
                    // attributes_count
                    tmp2 = ca->attributes_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);
                }
            }
        }

        // class attributes count
        tmp2 = cf->attributes_count; b2[0] = (tmp2 >> 8) & 0xFF; b2[1] = tmp2 & 0xFF; fwrite(b2,1,2,of);

        fclose(of);
        printf("Arquivo .class gerado em: %s\n", outpath);
    } else {
        fprintf(stderr, "Falha ao escrever %s\n", outpath);
    }

    return cf;
}
