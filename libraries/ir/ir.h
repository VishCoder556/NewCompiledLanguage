// Simple TAC IR
#ifndef STB_LANG_IR_H
#define STB_LANG_IR_H

#define STB_LANG_IR_INSTRS(...)  __VA_ARGS__
#define STB_LANG_IR_CASES(...) __VA_ARGS__
#define STB_LANG_IR_CASE(typ, ...) else if (ast->type == typ){__VA_ARGS__;}

#undef STB_LANG_INVOKE_TYPENEW
#define STB_LANG_INVOKE_TYPENEW(a) dymarray_typenew(a, 10, 5)

#define STB_LANG_IR_EMIT(typ, des, sr) \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr), _add)(&ir->instrs, (STB_CONCAT(CUR_IR_NAME, _Instr)){.type=typ, .str=sr, .dest=des})


#define STB_LANG_IR_BLOCK() do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->right; \
while (_block != NULL){ \
    STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, _block); \
    _block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_block->next; \
}}while(0);

#define STB_LANG_IR_PARAMS(pop) do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_args = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->left; \
while (_args != NULL){ \
    if (_args->type == STB_LANG_AST_TYPEINFO){ \
        STB_LANG_IR_EMIT(pop, _args->value, NULL); \
    } \
    _args = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_args->next; \
}}while(0);
// ^ popping from function args stack
// no function call implemented yet, we won't have to worry about pushing


#define STB_LANG_IR_RHS() STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_RHS(ast))

#define STB_LANG_IR_RETURN_SELF() return ast->value;

#define STB_LANG_IR_FUNCDEF(begin, pop, end) \
STB_LANG_IR_EMIT(begin, ast->value, NULL); \
STB_LANG_IR_PARAMS(pop) \
STB_LANG_IR_BLOCK() \
STB_LANG_IR_EMIT(end, ast->value, NULL);

#define STB_LANG_IR_ASSIGN(code)\
STB_LANG_IR_EMIT(code, ast->value, STB_LANG_IR_RHS())


#define STB_LANG_NEW_IR(types, cases) \
typedef enum{types}STB_CONCAT(CUR_IR_NAME, _InstrType); \
typedef struct { \
    STB_CONCAT(CUR_IR_NAME, _InstrType) type; \
    char *str; \
    char *dest; \
}STB_CONCAT(CUR_IR_NAME, _Instr); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_IR_NAME, _Instr));\
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *head; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *tail; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
}CUR_IR_NAME; \
CUR_IR_NAME *STB_CONCAT(CUR_IR_PREFIX, _init)(CUR_TYPEINFO_NAME *checker){ \
    CUR_IR_NAME *ir = malloc(sizeof(*ir)); \
    ir->head = checker->head; \
    ir->tail = ir->head; \
    ir->instrs = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr), _new)(); \
    return ir; \
} \
char *STB_CONCAT(CUR_IR_PREFIX, _ast)(CUR_IR_NAME *ir, STB_CONCAT(CUR_PARSER_NAME, _AST) *ast){ \
    if (0){}cases; \
    return NULL; \
}; \
char STB_CONCAT(CUR_IR_PREFIX, _translate)(CUR_IR_NAME *ir){ \
    if (ir->tail == NULL) return -1; \
 \
    STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, ir->tail); \
 \
    ir->tail = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ir->tail->next; \
    return 0; \
} \


#endif
