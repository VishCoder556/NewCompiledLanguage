#ifndef STB_LANG_CODEGEN_H
#define STB_LANG_CODEGEN_H

#include <stdarg.h>
#include <stdbool.h>

#define STB_LANG_CODEGEN_LIST(...) __VA_ARGS__
#define STB_LANG_CODEGEN_CASE(typ, ...) else if( instr->type == typ ) {__VA_ARGS__;}

#define STB_LANG_EMIT_CODE(...) STB_CONCAT(CUR_CODEGEN_PREFIX, _add_text)(gen, __VA_ARGS__);
#define STB_LANG_CODEGEN_REGISTERS(...) __VA_ARGS__

#define STB_LANG_CODEGEN_REGISTER_NAMES(...) __VA_ARGS__
#define STB_LANG_CODEGEN_REGISTER_MATCH(reg, n8, n4, n2, n1) else if (r == reg){switch(size){case 8: return n8; case 4: return n4; case 2: return n2; case 1: return n1;};}


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

#define STB_LANG_INVOKE_TYPENEW3(a) dymarray_typenew(a, STB_LANG_CODEGEN_REG_MAX, 1)

#define STB_LANG_NEW_CODEGEN(regtypes, matches, list) \
dymarray_typenew(char, 300, 40); \
typedef enum{ \
    regtypes, \
    STB_LANG_CODEGEN_REG_MAX \
} STB_CONCAT(CUR_CODEGEN_NAME, _Reg); \
typedef struct{ \
    bool available; \
} STB_CONCAT(CUR_CODEGEN_NAME, _Register); \
STB_LANG_INVOKE_TYPENEW3(STB_CONCAT(CUR_CODEGEN_NAME, _Register)); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    int cursor; \
    dymarray_char code; \
    int function_offset; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) current_scope; \
    STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register) *regs; \
}CUR_CODEGEN_NAME; \
STB_LANG_SIZE_OFFSET(); \
STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register) *STB_CONCAT(CUR_CODEGEN_PREFIX, _regs_init)(){ \
    STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register) *dym = malloc(sizeof(*dym)); \
    *dym = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register), _new)(); \
    for (int i=0; i<STB_LANG_CODEGEN_REG_MAX; i++){ \
        dym->data[dym->datalen++] = (STB_CONCAT(CUR_CODEGEN_NAME, _Register)){.available=1}; \
    } \
    return dym;\
}\
void STB_CONCAT(CUR_CODEGEN_PREFIX, _free_register)(CUR_CODEGEN_NAME *gen, STB_CONCAT(CUR_CODEGEN_NAME, _Reg) reg){ \
    gen->regs->data[(int)reg].available = 1; \
}; \
STB_CONCAT(CUR_CODEGEN_NAME, _Reg) STB_CONCAT(CUR_CODEGEN_PREFIX, _alloc_register)(CUR_CODEGEN_NAME *gen){ \
    STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register) *dym = gen->regs; \
    for (int i=0; i<dym->datalen; i++){ \
        if (dym->data[i].available == 1){ \
            dym->data[i].available = 0; \
            return (STB_CONCAT(CUR_CODEGEN_NAME, _Reg))i; \
        } \
    } \
    stb_lang_error_major_global("RegisterError", "Could not allocate a register"); \
    return -1; \
}; \
char *STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg_inner)(STB_CONCAT(CUR_CODEGEN_NAME, _Reg) r, int size){ \
    if(0){}matches; \
    return NULL; \
} \
char *STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg)(STB_CONCAT(CUR_CODEGEN_NAME, _Reg) r, int size){ \
    char *str = STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg_inner)(r, size); \
    if (str == NULL){ \
        stb_lang_error_major_global("RegisterError", "Invalid register or size accessed"); \
        return NULL; \
    } \
    return str; \
}; \
CUR_CODEGEN_NAME *STB_CONCAT(CUR_CODEGEN_PREFIX, _init)(CUR_IR_NAME *ir){ \
    CUR_CODEGEN_NAME *gen = malloc(sizeof(*gen)); \
    gen->code = dymarray_char_new(); \
    gen->instrs = ir->instrs; \
    gen->cursor = 0; \
    gen->function_offset = 0; \
    gen->root_scope = ir->root_scope; \
    gen->current_scope = gen->root_scope; \
    gen->regs = STB_CONCAT(CUR_CODEGEN_PREFIX, _regs_init)(); \
 \
    return gen; \
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
    ; \
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

#define STB_LANG_ALLOC_REGISTER(reg) STB_CONCAT(CUR_CODEGEN_NAME, _Reg) reg = STB_CONCAT(CUR_CODEGEN_PREFIX, _alloc_register)(gen);

#define STB_LANG_REGISTER(r, size) STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg)(r, size)

#define STB_LANG_FREE_REGISTER(r) STB_CONCAT(CUR_CODEGEN_PREFIX, _free_register)(gen, r)



#define STB_LANG_ARM_PROLOGUE() \
STB_LANG_EMIT_CODE("%s:\n", instr->dest); \
STB_LANG_EMIT_CODE("\tstp x29, x30, [sp, #-16]!\n"); \
STB_LANG_EMIT_CODE("\tmov x29, sp\n"); \
STB_LANG_GO_TO_FUNC(instr->dest);

// #define 
// STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg)(STB_CONCAT(CUR_CODEGEN_PREFIX, _alloc_register)(CUR_CODEGEN_NAME *gen))

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
STB_LANG_ALLOC_REGISTER(freg); \
if (size == 4) { \
    STB_LANG_EMIT_CODE("\tmov %s, #%s\n", STB_LANG_REGISTER(freg, 4), instr->src); \
    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(freg, 4), offset); \
}  \
else if (size == 8) { \
    STB_LANG_EMIT_CODE("\tmov %s, #%s\n", STB_LANG_REGISTER(freg, 8), instr->src); \
    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(freg, 4), offset); \
} \
STB_LANG_FREE_REGISTER(freg);


#define STB_LANG_ARM_DECLARE() STB_LANG_ARM_ASSIGN()
// ^ Yes, they are the same thing for now, but later on I will add a second symbol table with offsets and then they'll be different

#define STB_LANG_ARM_REG_MAP(num) \
    STB_LANG_CODEGEN_REGISTER_MATCH(REG_X##num, "x"#num, "w"#num, NULL, NULL)

#define STB_LANG_ARM_REGISTERS() \
STB_LANG_CODEGEN_REGISTERS( \
    REG_X0, \
    REG_X1, \
    REG_X2, \
    REG_X3, \
    REG_X4, \
    REG_X5, \
    REG_X6, \
    REG_X7, \
    REG_X9, \
    REG_X10, \
    REG_X11, \
    REG_X12, \
    REG_X13, \
    REG_X14, \
    REG_X15 \
)

#define STB_LANG_ARM_REGISTER_MAPS() \
STB_LANG_CODEGEN_REGISTER_NAMES( \
    STB_LANG_ARM_REG_MAP(0) \
    STB_LANG_ARM_REG_MAP(1) \
    STB_LANG_ARM_REG_MAP(2) \
    STB_LANG_ARM_REG_MAP(3) \
    STB_LANG_ARM_REG_MAP(4) \
    STB_LANG_ARM_REG_MAP(5) \
    STB_LANG_ARM_REG_MAP(6) \
    STB_LANG_ARM_REG_MAP(7) \
    STB_LANG_ARM_REG_MAP(9) \
    STB_LANG_ARM_REG_MAP(10) \
    STB_LANG_ARM_REG_MAP(11) \
    STB_LANG_ARM_REG_MAP(12) \
    STB_LANG_ARM_REG_MAP(13) \
    STB_LANG_ARM_REG_MAP(14) \
    STB_LANG_ARM_REG_MAP(15) \
)

#endif
