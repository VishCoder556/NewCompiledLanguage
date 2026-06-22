#ifndef STB_LANG_TYPEINFO_H
#define STB_LANG_TYPEINFO_H

// Some hacky macro stuff
#define STB_CONCAT_RAW(a, b) a##b
#define STB_CONCAT3_RAW(a, b, c) a##b##c

#define STB_CONCAT_EVAL(a, b) STB_CONCAT_RAW(a, b)
#define STB_CONCAT3_EVAL(a, b, c) STB_CONCAT3_RAW(a, b, c)

#define STB_CONCAT(a, b) STB_CONCAT_EVAL(a, b)
#define STB_CONCAT3(a, b, c) STB_CONCAT3_EVAL(a, b, c)


#define STB_LANG_TYPEINFO_CASE(typ, ...) else if(ast->type == typ){__VA_ARGS__;}
#define STB_LANG_TYPEINFO_2CASES(typ, typ2, ...) else if(ast->type == typ || ast->type == typ2){__VA_ARGS__;}
#define STB_LANG_TYPEINFO_3CASES(typ, typ2, typ3, ...) else if(ast->type == typ || ast->type == typ2 || ast->type == typ3){__VA_ARGS__;}
#define STB_LANG_TYPEINFO_4CASES(typ, typ2, typ3, typ4, ...) else if(ast->type == typ || ast->type == typ2 || ast->type == typ3 || ast->type == typ4) {__VA_ARGS__;}
#define STB_LANG_TYPEINFO_5CASES(typ, typ2, typ3, typ4, typ5, ...) else if(ast->type == typ || ast->type == typ2 || ast->type == typ3 || ast->type == typ4 || ast->type == typ5){__VA_ARGS__;}
#define STB_LANG_TYPEINFO_6CASES(typ, typ2, typ3, typ4, typ5, typ6, ...) else if(ast->type == typ || ast->type == typ2 || ast->type == typ3 || ast->type == typ4 || ast->type == typ5 || ast->type == typ6){__VA_ARGS__;}

#undef STB_LANG_INVOKE_TYPENEW
#define STB_LANG_INVOKE_TYPENEW(a) dymarray_typenew(a, 10, 3)

#define STB_LANG_ADD_VARIABLE(typeinf, scp, ...) do {if(typeinf.type != -1) { \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) symbol = __VA_ARGS__; \
    symbol.data.variable.offset = checker->entry_offset; \
    checker->entry_offset += STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_size)(symbol.typeinfo); \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol), _add)(scp, symbol);} \
}while(0);

#define STB_LANG_ADD_FUNCTION(nam, ...) do { \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) function = (STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)) {.name=nam, .kind = STB_LANG_SYMBOL_FUNCTION}; \
    function.data.function.args = malloc(sizeof(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Typeinfo))); \
    *function.data.function.args = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Typeinfo), _new)(); \
    __VA_ARGS__; \
    STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol), _add)(&(((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)checker->root_scope)->symbols), function); \
}while(0);


#define STB_LANG_FIND_FUNCTION(nam, ...) do { \
STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) *funcs = &(((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)checker->root_scope)->symbols); \
int found = 0; \
for (int i=0; i<funcs->datalen; i++){ \
    if (funcs->data[i].kind == STB_LANG_SYMBOL_FUNCTION){ \
        if (strcmp(funcs->data[i].name, nam) == 0){ \
            found = 1; \
            STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) function = funcs->data[i]; \
            __VA_ARGS__; \
        }; \
    } \
} \
if (found == 0){ \
    stb_lang_error_minor(checker->file.name, checker->file.contents, ast->offset, "FunctionError", "Function \"%s\" could not be found", nam); \
} \
}while(0);

#define STB_LANG_ITERATE_LINKED_LIST(start, head, type, ...) \
type *head = (type*)start; \
while (head != NULL){ \
    __VA_ARGS__; \
    head = (type*)head->next; \
}



#define STB_LANG_FUNCTION_ADD_PARAM(...) \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Typeinfo), _add)(function.data.function.args, (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo))__VA_ARGS__);

#define STB_LANG_FUNCTION_RETURN(...) \
function.typeinfo = (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo))__VA_ARGS__;

#define STB_LANG_GET_AST(what) (STB_CONCAT(CUR_PARSER_NAME, _AST)*)what

#define STB_LANG_AST_LINKED_LIST(what) STB_LANG_AS_AST(GetLinkedListHead(STB_LANG_AST_LINKED_LIST(what, STB_CONCAT(CUR_PARSER_NAME, _AST)))


#define STB_LANG_FUNCTION_ADD_PARAMS(params) \
STB_CONCAT(CUR_PARSER_NAME, _AST) *param = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)params; \
while (param != NULL){ \
    STB_LANG_FUNCTION_ADD_PARAM(param->typeinfo); \
    param = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)param->next; \
}

#define STB_LANG_FUNCTION_ALLOCATE(from, bytes) \
STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) *symbols = &((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)(from->current_scope))->symbols;\
for (int v=0; v<symbols->datalen; v++){ \
    if (symbols->data[v].kind == STB_LANG_SYMBOL_VARIABLE){ \
        symbols->data[v].data.variable.offset += bytes; \
    } \
}

#define STB_LANG_FUNCTION_CHECK_LIST(params) \
STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Typeinfo) *typeinfos = function.data.function.args; \
STB_CONCAT(CUR_PARSER_NAME, _AST) *param = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)params; \
for (int i=0; i<typeinfos->datalen; i++){ \
    if (typeinfos->data[i].type == STB_LANG_TYPEINFO_VARIADIC){ \
        int argidx = 0; \
        while (param != NULL) { \
            param->flags = STB_LANG_TYPEINFO_VARIADIC; \
            argidx++; \
            param = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)param->next; \
        } \
        STB_LANG_FUNCTION_ALLOCATE(checker, argidx * 8); \
        goto escape; \
        break; \
    } \
    if (param == NULL){ \
        stb_lang_error_minor(checker->file.name, checker->file.contents, ast->offset, "FunctionError", "Too few function arguments provided to function \"%s\"", function.name); \
    } \
    STB_LANG_EXPECT_TYPEINFO_EQ(param->typeinfo, typeinfos->data[i], ast->offset); \
    param = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)param->next; \
} \
if (param != NULL){ \
    stb_lang_error_minor(checker->file.name, checker->file.contents, ast->offset, "FunctionError", "Too many function arguments provided to function \"%s\"", function.name); \
} \
escape: \



#define STB_LANG_REGISTER_VARIABLE(nam, typeinf)  \
STB_LANG_ADD_VARIABLE(typeinf, \
    &(((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)checker->current_scope)->symbols),  \
    ((STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)) {.name=nam, .typeinfo=(STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo))typeinf}) \
);




#define STB_LANG_VARIABLE(ast) \
STB_LANG_REGISTER_VARIABLE(ast->value, ast->typeinfo)




#define STB_LANG_INFER_TYPE(nam) \
STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) symbol = STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find)((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)(checker->current_scope), nam); \
if (symbol.typeinfo.type != -1) {ast->typeinfo = symbol.typeinfo;}

#define STB_LANG_SCOPE_MAKE_CHILD(paren, curscope, ...) \
STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* news = ((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)paren); \
STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _ScopeL), _add)(&(news->children), __VA_ARGS__); \
curscope = (STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL))__VA_ARGS__;


#define STB_LANG_NEW_TYPEINFO(...) \
typedef enum { \
    STB_LANG_SYMBOL_VARIABLE, \
    STB_LANG_SYMBOL_FUNCTION \
}STB_CONCAT(CUR_TYPEINFO_NAME, _SymbolKind); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)); \
typedef struct { \
    char *name; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo) typeinfo; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _SymbolKind) kind; \
    union {; \
        struct { \
            int offset; \
        }variable; \
        struct { \
            STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Typeinfo) *args; \
        }function; \
    }data; \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol);\
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)); \
struct STB_CONCAT(CUR_TYPEINFO_NAME, _Scope); \
typedef struct STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL); \
STB_LANG_INVOKE_TYPENEW(STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL)); \
typedef struct { \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) parent; \
    STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _ScopeL) children; \
    STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) symbols; \
    char *name; \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Scope); \
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *head; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *tail; \
\
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) root_scope; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) current_scope; \
    int cursor; \
    \
    int entry_offset; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
}CUR_TYPEINFO_NAME; \
STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) STB_CONCAT(CUR_TYPEINFO_PREFIX, _scope_new)(STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) paren, char *name){ \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)* scope = malloc(sizeof(*scope)); \
    scope->parent = paren; \
    scope->children = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _ScopeL), _new)(); \
    scope->symbols = STB_CONCAT(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol), _new)(); \
    scope->name = name; \
    return (STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL))scope; \
} \
int STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find_from_symbols)(STB_CONCAT3(dymarray_, CUR_TYPEINFO_NAME, _Symbol) symbol_table, char *query, STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) *sym){ \
    if (query == NULL) {goto exit;} \
    for (int i=0; i<symbol_table.datalen; i++){\
        STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) symbol = symbol_table.data[i]; \
        if (symbol.name != NULL){ \
            if (strcmp(symbol.name, query) == 0){ \
                if (sym != NULL) \
                    *sym = symbol; \
                return 1; \
            }; \
        } \
    }; \
exit: \
    return 0; \
} \
STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) * STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find_scope)(STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *scope, char *query){ \
\
    \
    if (query == NULL) {goto exit;} \
    for (int i=0; i<scope->children.datalen; i++){\
        STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *a = (STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)scope->children.data[i]; \
        STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *scope1 = ((STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)scope->children.data[i]); \
        if (scope1->name != NULL){ \
            if (strcmp(scope1->name, query) == 0){ \
                return scope1; \
            }; \
        } \
    }; \
exit: \
    return NULL; \
} \
STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find)(STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *scope, char *query){ \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Scope) *sc = scope; \
    STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol) sym = (STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)){.typeinfo = -1}; \
    while (sc != NULL){ \
        if (STB_CONCAT(CUR_TYPEINFO_PREFIX, _symbol_find_from_symbols)(sc->symbols, query, &sym)) { \
            return sym; \
        } \
        sc = (STB_CONCAT(CUR_TYPEINFO_NAME, _Scope)*)sc->parent; \
    } \
    return (STB_CONCAT(CUR_TYPEINFO_NAME, _Symbol)){.typeinfo = -1}; \
} \
CUR_TYPEINFO_NAME *STB_CONCAT(CUR_TYPEINFO_PREFIX, _init)(CUR_PARSER_NAME *parser) { \
    CUR_TYPEINFO_NAME *typeinfo = malloc(sizeof(*typeinfo)); \
    typeinfo->cursor = 0; \
    typeinfo->head = GetLinkedListHead((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    typeinfo->tail = typeinfo->head; \
    typeinfo->root_scope = STB_CONCAT(CUR_TYPEINFO_PREFIX, _scope_new)(NULL, NULL); \
    typeinfo->current_scope = typeinfo->root_scope; \
    typeinfo->entry_offset = 0; \
    typeinfo->file = parser->file; \
    return typeinfo; \
} \
int a = 0; \
void STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(CUR_TYPEINFO_NAME *checker, STB_CONCAT(CUR_PARSER_NAME, _AST) *ast) { \
    if (ast == NULL){ \
        stb_lang_error_major_global("ParserError", "Found NULL AST"); \
    } \
    if (0){} __VA_ARGS__ else { \
        stb_lang_error_minor(checker->file.name, checker->file.contents, ast->offset, "TypecheckerError", "Could not typecheck ast with type '%d'", ast->type); \
    }; \
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
    if (_params->typeinfo.type != STB_LANG_TYPEINFO_VARIADIC){ \
        STB_LANG_VARIABLE(_params); \
    } \
    _params = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_params->next; \
}}while(0);


#define STB_LANG_EXPAND_LIST(where) do {\
STB_CONCAT(CUR_PARSER_NAME, _AST) *_block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)where; \
while (_block != NULL){ \
    STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, _block); \
    _block = (STB_CONCAT(CUR_PARSER_NAME, _AST)*)_block->next; \
}}while(0);

#define STB_LANG_EXPAND_ARGS() do {\
STB_LANG_EXPAND_LIST(ast->left); \
}while(0);


#define STB_LANG_EXPAND_BLOCK() do {\
STB_LANG_EXPAND_LIST(ast->right); \
}while(0);


#define STB_LANG_MAKE_SCOPE(nam) do {\
    STB_CONCAT(CUR_TYPEINFO_NAME, _ScopeL) new_scope = STB_CONCAT(CUR_TYPEINFO_PREFIX, _scope_new)(NULL, nam); \
    STB_LANG_SCOPE_MAKE_CHILD(checker->root_scope, checker->current_scope, new_scope); \
    checker->entry_offset = 0; \
}while(0);

#define STB_LANG_TYPEINFO_ASSUME_TYPE(typ) if (ast->typeinfo.type == -1) { \
    ast->typeinfo.type = typ.type; \
    ast->typeinfo.ptrnum = typ.ptrnum; \
}

#define STB_LANG_TYPEINFO(...) (STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo)){.type=__VA_ARGS__, .ptrnum=0}

#define STB_LANG_LHS(ast) ((STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->left)
#define STB_LANG_RHS(ast) ((STB_CONCAT(CUR_PARSER_NAME, _AST)*)ast->right)

#define STB_LANG_EXPAND(thing) do{STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, thing);}while(0);


#define STB_LANG_EXPAND_RHS() do{ if (STB_LANG_RHS(ast) != NULL){ STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, STB_LANG_RHS(ast));}}while(0);
#define STB_LANG_EXPAND_LHS() do{ STB_CONCAT(CUR_TYPEINFO_PREFIX, _check_ast)(checker, STB_LANG_LHS(ast));}while(0);

#define STB_LANG_EXPECT_TYPEINFO_EQ(left, right, offset) if (left.type != right.type || left.ptrnum != right.ptrnum){ \
stb_lang_error_minor(checker->file.name, checker->file.contents, offset, "TypeinfoError", "Expected types to be equal"); \
}

#define STB_LANG_TYPEINFO_ERROR_MINOR(where, type, nam) \
stb_lang_error_minor(checker->file.name, checker->file.contents, ((STB_CONCAT(CUR_PARSER_NAME, _AST)*)where)->offset, type, nam); \


#define STB_LANG_EXPECT_TYPE_EQ(left, right) if (left != NULL && right != NULL){ \
    STB_LANG_EXPECT_TYPEINFO_EQ(left->typeinfo, right->typeinfo, left->offset) \
}


#endif
