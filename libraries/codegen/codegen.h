#ifndef STB_LANG_CODEGEN_H
#define STB_LANG_CODEGEN_H

#include <stdarg.h>

#define STB_LANG_CODEGEN_LIST(...) __VA_ARGS__
#define STB_LANG_CODEGEN_CASE(typ, ...) else if( instr->type == typ ) {__VA_ARGS__;}

#define STB_LANG_EMIT_CODE(...) STB_CONCAT(CUR_CODEGEN_PREFIX, _add_text)(gen, __VA_ARGS__);


#define STB_LANG_SIZE_OFFSET() \
int STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(CUR_CODEGEN_NAME *gen, char *var){ \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *cur_scope = (STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->current_scope; \
    int typeinfo = STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find)(cur_scope, var).typeinfo; \
    return STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_size)(typeinfo); \
}; \
\
\
int STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(CUR_CODEGEN_NAME *gen, char *var){ \
    if (var == NULL) {return -1;} \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *scope = (STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->current_scope; \
    STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) symbol_table = scope->symbols; \
    for (int i=0; i<symbol_table.datalen; i++){\
        STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) symbol = symbol_table.data[i]; \
        if (symbol.name != NULL){ \
            if (strcmp(symbol.name, var) == 0){ \
                return symbol.offset; \
            }; \
        } \
    }; \
    return 0; \
}

#define STB_LANG_GO_TO_FUNC(nam) \
STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* scop = (((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->current_scope)); \
STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find_scope)((*((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->root_scope)), nam, scop);

#define STB_LANG_NEW_CODEGEN(list) \
dymarray_typenew(char, 300, 40); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    int cursor; \
    dymarray_char code; \
    int function_offset; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) current_scope; \
}CUR_CODEGEN_NAME; \
STB_LANG_SIZE_OFFSET(); \
CUR_CODEGEN_NAME *STB_CONCAT(CUR_CODEGEN_PREFIX, _init)(CUR_IR_NAME *ir){ \
    CUR_CODEGEN_NAME *codegen = malloc(sizeof(*codegen)); \
    codegen->code = dymarray_char_new(); \
    codegen->instrs = ir->instrs; \
    codegen->cursor = 0; \
    codegen->function_offset = 0; \
    codegen->root_scope = ir->root_scope; \
    codegen->current_scope = codegen->root_scope; \
    return codegen; \
} \
void STB_CONCAT(CUR_CODEGEN_PREFIX, _add_text)(CUR_CODEGEN_NAME *gen, char *str, ...){ \
    va_list args; \
    va_start(args, str); \
    char st[256]; \
    int len = vsnprintf(st, 256, str, args); \
    va_end(args); \
    if (len < 0 || len >= sizeof(st)) { \
        stb_lang_error_major_global("CodeGenError", "Too large of a buffer is being written at once"); \
        return; \
    } \
    int space = gen->code.datalen + len + 1; \
    if (space > gen->code.datacap){ \
        int cap = gen->code.datacap == 0 ? 512 : gen->code.datacap * 2; \
        while (cap < space){ \
            cap *= 2; \
        } \
        gen->code.datacap = cap; \
        gen->code.data = realloc(gen->code.data, gen->code.datacap); \
        if (!gen->code.data){ \
            stb_lang_error_major_global("CodeGenError", "Not enough memory to store assembly output"); \
            return; \
        } \
    } \
    memcpy(gen->code.data+gen->code.datalen, st, len); \
    gen->code.datalen += len; \
    ((char*)gen->code.data)[gen->code.datalen] = '\0'; \
} \
char STB_CONCAT(CUR_CODEGEN_PREFIX, _gen)(CUR_CODEGEN_NAME *gen, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    if(0){}list; \
    return 0; \
}; \
char STB_CONCAT(CUR_CODEGEN_PREFIX, _ir)(CUR_CODEGEN_NAME *gen){ \
    if (gen->cursor >= gen->instrs.datalen){ \
        return -1; \
    } \
    STB_CONCAT(CUR_IR_NAME, _Instr) *instr = (gen->instrs.data + gen->cursor); \
    if (STB_CONCAT(CUR_CODEGEN_PREFIX, _gen)(gen, instr) == -1){return -1;}; \
    gen->cursor++; \
    return 0; \
};


#define STB_LANG_ARM_PROLOGUE() \
STB_LANG_EMIT_CODE("%s:\n", instr->dest); \
STB_LANG_EMIT_CODE("\tstp x29, x30, [sp, #-16]!\n"); \
STB_LANG_EMIT_CODE("\tmov x29, sp\n"); \
STB_LANG_GO_TO_FUNC(instr->dest);


#define STB_LANG_X86_64_PROLOGUE() \
STB_LANG_EMIT_CODE("%s:\n", instr->dest); \
STB_LANG_EMIT_CODE("\tpush rbp\n"); \
STB_LANG_EMIT_CODE("\tmov rbp, rsp\n"); \
STB_LANG_EMIT_CODE("\tsub rsp, 32\n"); \
STB_LANG_GO_TO_FUNC(instr->dest);


#define STB_LANG_ARM_EPILOGUE() \
STB_LANG_EMIT_CODE("\tmov sp, x29\n"); \
STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n"); \
STB_LANG_EMIT_CODE("\tret\n");

#define STB_LANG_X86_64_EPILOGUE() \
STB_LANG_EMIT_CODE("\tmov rsp, rbp\n"); \
STB_LANG_EMIT_CODE("\tpop rbp\n"); \
STB_LANG_EMIT_CODE("\tret\n");

#define STB_LANG_CODEGEN_ASSIGN()\
int size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->dest); \
int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest);

#define STB_LANG_ARM_ASSIGN() \
STB_LANG_CODEGEN_ASSIGN(); \
if (size == 4) { \
    STB_LANG_EMIT_CODE("\tmov w0, #%s\n", instr->src); \
    STB_LANG_EMIT_CODE("\tstr w0, [sp, #%d]\n", offset); \
}  \
else if (size == 8) { \
    STB_LANG_EMIT_CODE("\tmov x0, #%s\n", instr->src); \
    STB_LANG_EMIT_CODE("\tstr x0, [sp, #%d]\n", offset); \
}

#define STB_LANG_ARM_DECLARE() STB_LANG_ARM_ASSIGN()
// ^ Yes, they are the same thing for now, but later on I will add a second symbol table with offsets and then they'll be different


#endif
