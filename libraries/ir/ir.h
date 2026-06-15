// Simple TAC IR
#ifndef STB_LANG_IR_H
#define STB_LANG_IR_H

#define STB_LANG_IR_INSTRS(...)  __VA_ARGS__
#define STB_LANG_IR_OPERANDS(...)  __VA_ARGS__
#define STB_LANG_IR_CASES(...) __VA_ARGS__
#define STB_LANG_IR_CASE(typ, ...) else if (ast->type == typ){__VA_ARGS__;}

#undef STB_LANG_INVOKE_TYPENEW
#define STB_LANG_INVOKE_TYPENEW(a) dymarray_typenew(a, 10, 5)

#define STB_LANG_IR_EMIT(typ, des, lef, righ) \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr), _add)(&ir->instrs, (STB_CONCAT(CUR_IR_NAME, _Instr)){.type=typ, .left=lef, .right=righ, .dest=des, .offset=offset})


#define STB_LANG_IR_BLOCK() do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->right; \
while (_block != NULL){ \
    STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, _block); \
    _block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_block->next; \
}}while(0);


#define STB_LANG_IR_RHS() STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_RHS(ast))
#define STB_LANG_IR_LHS() STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, STB_LANG_LHS(ast))


#define STB_LANG_IR_PARAMS(assign, reg) do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_args = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->left; \
int idx = 0; \
while (_args != NULL){ \
    char str[10];snprintf(str, 10, "a%d", idx); \
    STB_LANG_IR_EMIT(assign, STB_LANG_IR_OPERAND_NAME(reg, strdup(str)), STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, _args), NULL); \
    _args = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_args->next; \
}}while(0);
// ^ popping from function args stack
// no function call implemented yet, we won't have to worry about pushing

#define STB_LANG_IR_ARGS(opcode, reg) do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_args = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->left; \
int idx = 0; \
while (_args != NULL){ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, _args); \
    char str[10];snprintf(str, 10, "a%d", idx); \
    STB_LANG_IR_EMIT(opcode, STB_LANG_IR_OPERAND_NAME(reg, strdup(str)), operand, NULL); \
    idx++; \
    _args = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_args->next; \
}}while(0);

#define STB_LANG_IR_OPERAND_NAME(typ, val) ({ \
    STB_CONCAT(CUR_IR_NAME, _Operand) *op = malloc(sizeof(*op)); \
    op->type = typ; \
    op->value = val; \
    op; \
})

#define STB_LANG_IR_RETURN_SELF(type) return STB_LANG_IR_OPERAND_NAME(type, ast->value);

#define STB_LANG_IR_FUNCDEF(optype, begin, assign, reg, end) \
STB_LANG_IR_EMIT(begin, STB_LANG_IR_OPERAND_NAME(optype, ast->value), NULL, NULL); \
STB_LANG_IR_PARAMS(assign, reg) \
STB_LANG_IR_BLOCK() \
STB_LANG_IR_EMIT(end, STB_LANG_IR_OPERAND_NAME(optype, ast->value), NULL, NULL);


#define STB_LANG_IR_FUNCALL(optype, call, opcode, reg) \
STB_LANG_IR_ARGS(opcode, reg) \
STB_LANG_IR_EMIT(call, STB_LANG_IR_OPERAND_NAME(optype, ast->value), NULL, NULL);

#define STB_LANG_IR_ASSIGN(optype, code)\
STB_LANG_IR_EMIT(code, STB_LANG_IR_OPERAND_NAME(optype, ast->value), STB_LANG_IR_RHS(), NULL)

#define STB_LANG_IR_VAR(optype, code)\
STB_LANG_IR_EMIT(code, STB_LANG_IR_OPERAND_NAME(optype, ast->value), STB_LANG_IR_RHS(), NULL)

#define STB_LANG_IR_BINARY(optype, reg) \
char *temp_reg = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir); \
STB_LANG_IR_EMIT(optype, STB_LANG_IR_OPERAND_NAME(reg, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
return STB_LANG_IR_OPERAND_NAME(reg, temp_reg);



#define STB_LANG_NEW_IR(operands, types, cases) \
typedef enum{operands}STB_CONCAT(CUR_IR_NAME, _OperandType); \
typedef struct { \
    STB_CONCAT(CUR_IR_NAME, _OperandType) type; \
    char *value; \
}STB_CONCAT(CUR_IR_NAME, _Operand); \
typedef enum{types}STB_CONCAT(CUR_IR_NAME, _InstrType); \
typedef struct { \
    STB_CONCAT(CUR_IR_NAME, _InstrType) type; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *left; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *right; \
    STB_CONCAT(CUR_IR_NAME, _Operand) *dest; \
    int offset; \
}STB_CONCAT(CUR_IR_NAME, _Instr); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_IR_NAME, _Instr));\
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *head; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *tail; \
    STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr) instrs; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
    int temp_number; \
}CUR_IR_NAME; \
char *STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(Lang_IR *ir) { \
    char *str = malloc(16); \
    snprintf(str, 16, "t%d", ir->temp_number++); \
    return str; \
} \
CUR_IR_NAME *STB_CONCAT(CUR_IR_PREFIX, _init)(CUR_TYPEINFO_NAME *checker){ \
    CUR_IR_NAME *ir = malloc(sizeof(*ir)); \
    ir->head = checker->head; \
    ir->tail = ir->head; \
    ir->instrs = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_IR_NAME, _Instr), _new)(); \
    ir->root_scope = checker->root_scope; \
    ir->file = checker->file; \
    ir->temp_number = 0; \
    return ir; \
} \
STB_CONCAT(CUR_IR_NAME, _Operand) *STB_CONCAT(CUR_IR_PREFIX, _ast)(CUR_IR_NAME *ir, STB_CONCAT(CUR_PARSER_NAME, _AST) *ast){ \
    int offset = ast->offset; \
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
