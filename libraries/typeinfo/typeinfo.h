#ifndef STB_LANG_TYPEINFO_H
#define STB_LANG_TYPEINFO_H

// Some hacky macro stuff
#define STB_CONCAT_RAW(a, b) a##b
#define STB_CONCAT3_RAW(a, b, c) a##b##c

#define STB_CONCAT_EVAL(a, b) STB_CONCAT_RAW(a, b)
#define STB_CONCAT3_EVAL(a, b, c) STB_CONCAT3_RAW(a, b, c)

#define STB_CONCAT(a, b) STB_CONCAT_EVAL(a, b)
#define STB_CONCAT3(a, b, c) STB_CONCAT3_EVAL(a, b, c)


#define STB_LANG_TYPEINFO_CASE(typ) else if(ast->type == typ)

#undef STB_LANG_INVOKE_TYPENEW
#define STB_LANG_INVOKE_TYPENEW(a) dymarray_typenew(a, 10, 3)

#define STB_LANG_REGISTER_SYMBOL(nam, typeinf) do {if(typeinf != -1) {STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol), _add)(&(checker->symbol_table), (STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)){.name=nam, .typeinfo=typeinf});}}while(0);

#define STB_LANG_SYMBOL(ast) STB_LANG_REGISTER_SYMBOL(ast->value, ast->typeinfo)

#define STB_LANG_INFER_TYPE(nam) \
STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) symbol = STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find)(checker, nam); \
if (symbol.typeinfo != -1) {ast->typeinfo = symbol.typeinfo;}

#define STB_LANG_NEW_TYPEINFO(...) \
typedef struct { \
    char *name; \
    int typeinfo; \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol);\
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)); \
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *head; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *tail; \
\
    STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) symbol_table; \
    int cursor; \
}CUR_TYPEINFO_NAME; \
STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find)(CUR_TYPEINFO_NAME *checker, char *query){ \
    if (query == NULL) {goto exit;} \
    for (int i=0; i<checker->symbol_table.datalen; i++){\
        STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) symbol = checker->symbol_table.data[i]; \
        if (symbol.name != NULL){ \
            if (strcmp(symbol.name, query) == 0){ \
                return symbol; \
            }; \
        } \
    }; \
exit: \
    return (STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)){.typeinfo = STB_LANG_AST_TYPE_NONE}; \
} \
CUR_TYPEINFO_NAME *STB_CONCAT(CUR_TYPEINFO_PREFIX, _init)(CUR_PARSER_NAME *parser) { \
    CUR_TYPEINFO_NAME *typeinfo = malloc(sizeof(*typeinfo)); \
    typeinfo->cursor = 0; \
    typeinfo->head = GetLinkedListHead((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    typeinfo->tail = typeinfo->head; \
    typeinfo->symbol_table = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol), _new)(); \
    return typeinfo; \
} \
int a = 0; \
void STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(CUR_TYPEINFO_NAME *checker, STB_CONCAT(CUR_PARSER_NAME, _AST) *ast) { \
    if (0){} __VA_ARGS__; \
} \
char STB_CONCAT(CUR_TYPEINFO_PREFIX, _check)(CUR_TYPEINFO_NAME *checker) { \
    if (checker->tail == NULL){return -1;} \
    STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, checker->tail); \
    checker->tail = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)checker->tail->next; \
    return (checker->tail == NULL) ? -1 : 0; \
}

#define STB_LANG_EXPAND_PARAMS() do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_params = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->left; \
while (_params != NULL){ \
    STB_LANG_SYMBOL(_params); \
    _params = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_params->next; \
}}while(0);
#define STB_LANG_EXPAND_BLOCK() do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->right; \
while (_block != NULL){ \
    STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, _block); \
    _block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_block->next; \
}}while(0);


#define STB_LANG_ASSUME_TYPE(typ) if (ast->typeinfo == -1) {ast->typeinfo = typ;}

#define STB_LANG_LHS(ast) ((STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->left)
#define STB_LANG_RHS(ast) ((STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->right)

#define STB_LANG_EXPAND(thing) do{STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, thing);}while(0);


#define STB_LANG_EXPAND_RHS() do{ STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, STB_LANG_RHS(ast));}while(0);

#define STB_LANG_EXPECT_TYPE_EQ(left, right) if (left != NULL && right != NULL){if (left->typeinfo != right->typeinfo){ \
stb_lang_error_major_global("TypeinfoError", "Expected types to be equal"); \
}}
// ^ Yes, the error messages look bad as of right now; I will fix them eventually.

#define STB_LANG_ASSIGN() \
STB_LANG_EXPAND_RHS(); \
STB_LANG_INFER_TYPE(ast->value); \
STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast)) \
STB_LANG_SYMBOL(ast);


#endif
