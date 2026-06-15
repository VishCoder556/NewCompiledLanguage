#ifndef STB_LANG_CODEGEN_H
#define STB_LANG_CODEGEN_H

#include <stdarg.h>
#include <stdbool.h>

#define STB_LANG_CODEGEN_LIST(...) __VA_ARGS__
#define STB_LANG_CODEGEN_CASE(typ, ...) else if( instr->type == typ ) {__VA_ARGS__;}

#define STB_LANG_EMIT_CODE(...) STB_CONCAT(CUR_CODEGEN_PREFIX, _add_text)(gen, instr->offset, __VA_ARGS__);
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
STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* _curscope = (STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->current_scope; \
STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find_scope)((*((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)gen->root_scope)), nam, &_curscope); \
gen->current_scope = (STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL))_curscope;

#define STB_LANG_INVOKE_TYPENEW3(a) dymarray_typenew(a, STB_LANG_CODEGEN_REG_MAX, 1)

#define STB_LANG_NEW_CODEGEN(regtypes, matches, prefix, list) \
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
    char *virtual; \
    STB_CONCAT(CUR_CODEGEN_NAME, _Reg) real; \
}STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister); \
STB_LANG_INVOKE_TYPENEW3(STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister)); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    int cursor; \
    dymarray_char code; \
    int function_offset; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) current_scope; \
    STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register) *regs; \
    STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _VirtualRegister) *virtual_regs; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
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
STB_CONCAT(CUR_CODEGEN_NAME, _Reg) STB_CONCAT(CUR_CODEGEN_PREFIX, _alloc_register)(CUR_CODEGEN_NAME *gen, int offset){ \
    STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _Register) *dym = gen->regs; \
    for (int i=0; i<dym->datalen; i++){ \
        if (dym->data[i].available == 1){ \
            dym->data[i].available = 0; \
            return (STB_CONCAT(CUR_CODEGEN_NAME, _Reg))i; \
        } \
    } \
    if (offset == -1) { \
        stb_lang_error_major_global("RegisterError", "Could not allocate a register"); \
    }else { \
        stb_lang_error_minor(gen->file.name, gen->file.contents, offset, "RegisterError", "Could not allocate a register"); \
    } \
    return -1; \
}; \
char *STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg_inner)(STB_CONCAT(CUR_CODEGEN_NAME, _Reg) r, int size){ \
    if(0){}matches; \
    return NULL; \
} \
char *STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg)(CUR_CODEGEN_NAME *gen, int offset, STB_CONCAT(CUR_CODEGEN_NAME, _Reg) r, int size){ \
    char *str = STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg_inner)(r, size); \
    if (str == NULL){ \
        stb_lang_error_minor(gen->file.name, gen->file.contents, offset, "RegisterError", "Invalid register or size accessed"); \
        return NULL; \
    } \
    return str; \
}; \
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
STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init)(CUR_CODEGEN_NAME *gen, char *virtual, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) vreg; \
    STB_LANG_ALLOC_REGISTER(real); \
    vreg.real = real; \
    vreg.virtual = virtual; \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _VirtualRegister), _add)(gen->virtual_regs, vreg); \
    return vreg; \
}; \
STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_find)(CUR_CODEGEN_NAME *gen, char *virtual, STB_CONCAT(CUR_IR_NAME, _Instr) *instr){ \
    for (int i=0; i<gen->virtual_regs->datalen; i++){ \
        if (strcmp(gen->virtual_regs->data[i].virtual, virtual) == 0){ \
            return gen->virtual_regs->data[i]; \
        }; \
    }; \
    if (instr != NULL){ \
        stb_lang_error_minor(gen->file.name, gen->file.contents, instr->offset, "RegisterError", "Could not find virtual register"); \
    } \
    stb_lang_error_major_global("RegisterError", "Could not find virtual register"); \
    return (STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister)){0}; \
}; \
STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init_find)(CUR_CODEGEN_NAME *gen, char *virtual){ \
    for (int i=0; i<gen->virtual_regs->datalen; i++){ \
        if (strcmp(gen->virtual_regs->data[i].virtual, virtual) == 0){ \
            return gen->virtual_regs->data[i]; \
        }; \
    }; \
    return STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init)(gen, virtual, NULL); \
}; \
void STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_free)(CUR_CODEGEN_NAME *gen, STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) virtual){ \
    STB_LANG_FREE_REGISTER(virtual.real); \
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
    gen->virtual_regs = malloc(sizeof(*gen->virtual_regs)); \
    *gen->virtual_regs = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_CODEGEN_NAME, _VirtualRegister), _new)(); \
    gen->file = ir->file; \
    STB_CONCAT(CUR_IR_NAME, _Instr) *instr = (gen->instrs.data + gen->cursor); \
 \
    prefix; \
    return gen; \
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

#define STB_LANG_ALLOC_REGISTER(reg) STB_CONCAT(CUR_CODEGEN_NAME, _Reg) reg; if (instr == NULL){reg = STB_CONCAT(CUR_CODEGEN_PREFIX, _alloc_register)(gen, -1);}else {reg = STB_CONCAT(CUR_CODEGEN_PREFIX, _alloc_register)(gen, instr->offset);}

#define STB_LANG_REGISTER(r, size) STB_CONCAT(CUR_CODEGEN_PREFIX, _register_from_reg)(gen, instr->offset, r, size)

#define STB_LANG_FREE_REGISTER(r) STB_CONCAT(CUR_CODEGEN_PREFIX, _free_register)(gen, r)



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

#define STB_LANG_ARM_ASSIGN(lit, var) \
if (instr->dest->type == var){ \
    STB_LANG_CODEGEN_ASSIGN();  \
    STB_LANG_ALLOC_REGISTER(freg); \
    if (size == 4) { \
        STB_LANG_EMIT_CODE("\tmov %s, #%s\n", STB_LANG_REGISTER(freg, 4), instr->left->value); \
        STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(freg, 4), offset); \
    }  \
    else if (size == 8) { \
        if (instr->left->type == var){ \
            int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value); \
            STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", STB_LANG_REGISTER(freg, 8), newoffset); \
        }else if(instr->left->type == lit){ \
            STB_LANG_EMIT_CODE("\tmov %s, #%s\n", STB_LANG_REGISTER(freg, 8), instr->left->value); \
        } \
        STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(freg, 8), offset); \
    } \
    STB_LANG_FREE_REGISTER(freg); \
}else if (instr->dest->value[0] == 'a'){ \
    int r = atoi(instr->dest->value+1); \
    if (instr->left->type == var){ \
        int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->left->value); \
        STB_LANG_EMIT_CODE("\tstr x%d, [sp, #%d]\n", r, newoffset); \
    }else if(instr->left->type == lit){ \
        STB_LANG_EMIT_CODE("\tmov x%d, #%s\n", r, instr->left->value); \
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
            STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) vreg = STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init_find)(gen, instr->left->value); \
            STB_LANG_EMIT_CODE("\tmov x%d, %s\n", r, STB_LANG_REGISTER(vreg.real, 8)); \
        } \
    } \
} \


#define STB_LANG_ARM_FUNCALL() \
STB_LANG_EMIT_CODE("\tbl %s\n", instr->dest->value);

#define STB_LANG_ARM_MOVE(reg, lit, var, right, ...) {\
char *leftr = __VA_ARGS__; \
if (leftr[0] == 't'){ \
    STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) vreg = STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init)(gen, instr->dest->value, instr); \
    leftr = STB_LANG_REGISTER(vreg.real, 8); \
} \
if (right->type == lit) { \
    STB_LANG_EMIT_CODE("\tmov %s, #%s\n", leftr, right->value); \
}else if(right->type == var){ \
    int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, right->value); \
    STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, newoffset); \
}else if (right->type == reg){ \
    STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) _vreg = STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_find)(gen, right->value, NULL); \
    STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, STB_LANG_REGISTER(_vreg.real, 8)); \
    STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_free)(gen, _vreg); \
} \
}

#define STB_LANG_ARM_PUSH(lit, var, reg) \
    STB_LANG_ALLOC_REGISTER(pushreg); \
    STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(pushreg, 8)); \
    STB_LANG_EMIT_CODE("\tstr %s, [sp, #-16]!\n", STB_LANG_REGISTER(pushreg, 8)); \
    STB_LANG_FREE_REGISTER(pushreg);

// FIXED: Pop reads your target register from the stack memory space and clears 16 alignment bytes
#define STB_LANG_ARM_POP() \
    STB_LANG_EMIT_CODE("\tldr %s, [sp], #16\n", instr->dest->value);


#define STB_LANG_ARM_DECLARE(...) STB_LANG_ARM_ASSIGN(__VA_ARGS__)
// ^ Yes, they are the same thing for now, but later on I will add a second symbol table with offsets and then they'll be different

#define STB_LANG_ARM_REG_MAP(num) \
    STB_LANG_CODEGEN_REGISTER_MATCH(REG_X##num, "x"#num, "w"#num, NULL, NULL)

#define STB_LANG_ARM_REGISTERS() \
STB_LANG_CODEGEN_REGISTERS( \
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
    STB_LANG_ARM_REG_MAP(9) \
    STB_LANG_ARM_REG_MAP(10) \
    STB_LANG_ARM_REG_MAP(11) \
    STB_LANG_ARM_REG_MAP(12) \
    STB_LANG_ARM_REG_MAP(13) \
    STB_LANG_ARM_REG_MAP(14) \
    STB_LANG_ARM_REG_MAP(15) \
)

#define STB_LANG_ARM_PREFIX \
STB_LANG_EMIT_CODE("%s\n", ".global _main"); \
STB_LANG_EMIT_CODE("%s\n", ".align 2");


#define STB_LANG_ARM_BINARY_IMM(op, reg, lit, var) \
if (instr->dest->type == reg) { \
    STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) vreg = STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init)(gen, instr->dest->value, instr); \
\
    if (instr->right->type == lit){ \
        STB_LANG_ALLOC_REGISTER(left); \
        STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(left, 8)); \
    \
        STB_LANG_EMIT_CODE("\t%s %s, %s, #%s\n", op, STB_LANG_REGISTER(vreg.real, 8), STB_LANG_REGISTER(left, 8), instr->right->value); \
        STB_LANG_FREE_REGISTER(left); \
    }else if(instr->right->type == var){ \
        STB_LANG_ALLOC_REGISTER(left); \
        STB_LANG_ALLOC_REGISTER(right); \
        STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(left, 8)); \
        STB_LANG_ARM_MOVE(reg, lit, var, instr->right, STB_LANG_REGISTER(right, 8)); \
    \
        STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(vreg.real, 8), STB_LANG_REGISTER(left, 8), STB_LANG_REGISTER(right, 8)); \
        STB_LANG_FREE_REGISTER(left); \
        STB_LANG_FREE_REGISTER(right); \
    }else if(instr->right->type == reg){ \
        STB_LANG_ALLOC_REGISTER(left); \
        STB_LANG_ALLOC_REGISTER(right); \
        STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(left, 8)); \
        STB_LANG_ARM_MOVE(reg, lit, var,  instr->right, STB_LANG_REGISTER(right, 8)); \
        STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(vreg.real, 8), STB_LANG_REGISTER(left, 8), STB_LANG_REGISTER(right, 8)); \
        STB_LANG_FREE_REGISTER(left); \
        STB_LANG_FREE_REGISTER(right); \
    } \
} \

#define STB_LANG_ARM_BINARY_REG(op, reg, lit, var) \
if (instr->dest->type == reg) { \
    STB_CONCAT(CUR_CODEGEN_NAME, _VirtualRegister) vreg = STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_init)(gen, instr->dest->value, instr); \
\
        STB_LANG_ALLOC_REGISTER(left); \
        STB_LANG_ALLOC_REGISTER(right); \
        STB_LANG_ARM_MOVE(reg, lit, var, instr->left, STB_LANG_REGISTER(left, 8)); \
        STB_LANG_ARM_MOVE(reg, lit, var, instr->right, STB_LANG_REGISTER(right, 8)); \
    \
        STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(vreg.real, 8), STB_LANG_REGISTER(left, 8), STB_LANG_REGISTER(right, 8)); \
        STB_LANG_FREE_REGISTER(left); \
        STB_LANG_FREE_REGISTER(right); \
    \
}
    // STB_CONCAT(CUR_CODEGEN_PREFIX, _virtual_reg_free)(gen, vreg); \


#define STB_LANG_CODEGEN_PREFIX(...) __VA_ARGS__

#endif
