#ifndef STB_LANG_CODEGEN_H
#define STB_LANG_CODEGEN_H

#include <stdarg.h>
#include <stdbool.h>

#define STB_LANG_CODEGEN_LIST(...) __VA_ARGS__
#define STB_LANG_CODEGEN_CASE(typ, ...) else if( instr->type == typ ) {__VA_ARGS__;}

#define STB_LANG_EMIT_CODE(...) STB_CONCAT(CUR_CODEGEN_PREFIX, _add_text)(gen, instr->offset, __VA_ARGS__);


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
STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* _curscope = (STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->current_scope; \
STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find_scope)((*((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->root_scope)), nam, &_curscope); \
gen->current_scope = (STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL))_curscope;


#define STB_LANG_NEW_CODEGEN(prefix, list) \
dymarray_typenew(char, 300, 40); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    int cursor; \
    dymarray_char code; \
    int function_offset; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) current_scope; \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _Register) *regs; \
    STB_CONCAT3(dymarray_, CUR_REGALLOC_NAME, _VirtualRegister) *virtual_regs; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
}CUR_CODEGEN_NAME; \
STB_LANG_SIZE_OFFSET(); \
void STB_CONCAT(CUR_CODEGEN_PREFIX, _add_text)(CUR_CODEGEN_NAME *gen, int offset, char *str, ...){ \
    va_list args; \
    va_start(args, str); \
    char st[256]; \
    int len = vsnprintf(st, 256, str, args); \
    va_end(args); \
    if (len < 0 || len >= sizeof(st)) { \
        stb_lang_error_minor(gen->file.name, gen->file.contents, offset, "CodeGenError", "Too large of a buffer is being written at once"); \
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
            stb_lang_error_minor(gen->file.name, gen->file.contents, offset, "CodeGenError", "Not enough memory to store assembly output"); \
            return; \
        } \
    } \
    memcpy(gen->code.data+gen->code.datalen, st, len); \
    gen->code.datalen += len; \
    ((char*)gen->code.data)[gen->code.datalen] = '\0'; \
} \
CUR_CODEGEN_NAME *STB_CONCAT(CUR_CODEGEN_PREFIX, _init)(CUR_REGALLOC_NAME *regalloc){ \
    CUR_CODEGEN_NAME *gen = malloc(sizeof(*gen)); \
    gen->code = dymarray_char_new(); \
    gen->instrs = regalloc->instrs; \
    gen->cursor = 0; \
    gen->function_offset = 0; \
    gen->root_scope = regalloc->root_scope; \
    gen->current_scope = gen->root_scope; \
    gen->regs = regalloc->regs; \
    gen->virtual_regs = regalloc->virtual_regs; \
    gen->file = regalloc->file; \
    STB_CONCAT(CUR_IR_NAME, _Instr) *instr = (gen->instrs.data + gen->cursor); \
 \
    prefix; \
    return gen; \
} \
char STB_CONCAT(CUR_CODEGEN_PREFIX, _gen)(CUR_CODEGEN_NAME *gen, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    if(0){}list else { \
        stb_lang_error_minor(gen->file.name, gen->file.contents, instr->offset, "CodegenError", "Could not convert IR instruction with type '%d' into code", instr->type); \
    }; \
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
STB_LANG_EMIT_CODE("%s:\n", instr->dest->value); \
STB_LANG_EMIT_CODE("\tstp x29, x30, [sp, #-16]!\n"); \
STB_LANG_EMIT_CODE("\tmov x29, sp\n"); \
STB_LANG_EMIT_CODE("\tsub sp, sp, #32\n"); \
STB_LANG_GO_TO_FUNC(instr->dest->value);


#define STB_LANG_X86_64_PROLOGUE() \
STB_LANG_EMIT_CODE("%s:\n", instr->dest->value); \
STB_LANG_EMIT_CODE("\tpush rbp\n"); \
STB_LANG_EMIT_CODE("\tmov rbp, rsp\n"); \
STB_LANG_EMIT_CODE("\tsub rsp, 32\n"); \
STB_LANG_GO_TO_FUNC(instr->dest->value); \


#define STB_LANG_ARM_EPILOGUE() \
STB_LANG_EMIT_CODE("\tmov sp, x29\n"); \
STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n"); \
STB_LANG_EMIT_CODE("\tret\n");

#define STB_LANG_X86_64_EPILOGUE() \
STB_LANG_EMIT_CODE("\tmov rsp, rbp\n"); \
STB_LANG_EMIT_CODE("\tpop rbp\n"); \
STB_LANG_EMIT_CODE("\tret\n");

#define STB_LANG_CODEGEN_ASSIGN()\
int size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->dest->value); \
int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value);

#define STB_LANG_ARM_ASSIGN(reg, lit, var) \
if (instr->dest->type == var){ \
    STB_LANG_CODEGEN_ASSIGN();  \
    ; \
    if (instr->left->type == var){ \
        int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value); \
        STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys1, size), newoffset); \
    }else if(instr->left->type == lit){ \
        STB_LANG_EMIT_CODE("\tmov %s, #%s\n", STB_LANG_REGISTER(instr->phys1, size), instr->left->value); \
    }else if(instr->left->type == reg){ \
        STB_LANG_EMIT_CODE("\tmov %s, %s\n", STB_LANG_REGISTER(instr->phys1, size), STB_LANG_REGISTER(instr->left->phys, size)); \
    } \
    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys1, size), offset); \
    ; \
}else if (instr->dest->type == reg){ \
    if (instr->left->type == var){ \
        int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->left->value); \
        STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", instr->dest->value, newoffset); \
    }else if(instr->left->type == lit){ \
        STB_LANG_EMIT_CODE("\tmov %s, #%s\n", instr->dest->value, instr->left->value); \
    } \
}

#define STB_LANG_ARM_ASSIGN_REG(reg, lit, var) \
if (instr->dest->type == reg && instr->dest->value[0] == 'a'){ \
    int r = atoi(instr->dest->value+1); \
    if (instr->left != NULL){ \
        if (instr->left->type == lit) { \
            STB_LANG_EMIT_CODE("\tmov x%d, #%s\n", r, instr->left->value); \
        }else if (instr->left->type == var){ \
            int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->left->value); \
            STB_LANG_EMIT_CODE("\tldr x%d, [sp, #%d]\n", r, newoffset); \
        }else if (instr->left->type == reg) { \
            STB_LANG_EMIT_CODE("\tmov x%d, %s\n", r, STB_LANG_REGISTER(instr->left->phys, 8)); \
        } \
    } \
} \


#define STB_LANG_ARM_FUNCALL() \
STB_LANG_EMIT_CODE("\tbl %s\n", instr->dest->value);

#define STB_LANG_ARM_MOVE(reg, lit, var, right, ...) {\
char *leftr = __VA_ARGS__; \
if (leftr[0] == 't'){ \
    leftr = STB_LANG_REGISTER(instr->dest->phys, 8); \
} \
if (right->type == lit) { \
    STB_LANG_EMIT_CODE("\tmov %s, #%s\n", leftr, right->value); \
}else if(right->type == var){ \
    int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, right->value); \
    STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, newoffset); \
}else if (right->type == reg){ \
    STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, STB_LANG_REGISTER(right->phys, 8)); \
} \
}

#define STB_LANG_ARM_PUSH(lit, var, reg) \
    STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(instr->phys1, 8)); \
    STB_LANG_EMIT_CODE("\tstr %s, [sp, #-16]!\n", STB_LANG_REGISTER(instr->phys1, 8)); \


// FIXED: Pop reads your target register from the stack memory space and clears 16 alignment bytes
#define STB_LANG_ARM_POP() \
    STB_LANG_EMIT_CODE("\tldr %s, [sp], #16\n", instr->dest->value);


#define STB_LANG_ARM_DECLARE(...) STB_LANG_ARM_ASSIGN(__VA_ARGS__)
// ^ Yes, they are the same thing for now, but later on I will add a second symbol table with offsets and then they'll be different


#define STB_LANG_ARM_PREFIX \
STB_LANG_EMIT_CODE("%s\n", ".global _main"); \
STB_LANG_EMIT_CODE("%s\n", ".align 2");


#define STB_LANG_ARM_BINARY(op, reg, lit, var) \
if (instr->dest->type == reg) { \
\
    STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(instr->phys1, 8)); \
    STB_LANG_ARM_MOVE(reg, lit, var, instr->right, STB_LANG_REGISTER(instr->phys2, 8)); \
\
    STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys1, 8), STB_LANG_REGISTER(instr->phys2, 8)); \
} \



#define STB_LANG_CODEGEN_PREFIX(...) __VA_ARGS__

#endif
