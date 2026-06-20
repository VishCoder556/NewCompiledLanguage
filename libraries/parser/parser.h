/* PARSER.H library
 * Yes, there's documentation for this library and not the tokenizer library.
 * This library is going to be a bit more complicated so it needs some planning
*/

#ifndef STB_LANG_PARSER_H
#define STB_LANG_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include "../linkedlist/linkedlist.h"

// Some hacky macro stuff
#define STB_CONCAT_RAW(a, b) a##b
#define STB_CONCAT3_RAW(a, b, c) a##b##c

#define STB_CONCAT_EVAL(a, b) STB_CONCAT_RAW(a, b)
#define STB_CONCAT3_EVAL(a, b, c) STB_CONCAT3_RAW(a, b, c)

#define STB_CONCAT(a, b) STB_CONCAT_EVAL(a, b)
#define STB_CONCAT3(a, b, c) STB_CONCAT3_EVAL(a, b, c)

#define STB_LANG_PARSER_ERROR_FALLOUT(where) stb_lang_error_minor(parser->file.name, parser->file.contents, token.offset, "ParserError", "Could not parse %s", where)

#define STB_LANG_ASTS(...) __VA_ARGS__
#define STB_LANG_PARSE_BODY(...) __VA_ARGS__; STB_LANG_PARSER_ERROR_FALLOUT("body statement");
#define STB_LANG_PARSE_AST(...) __VA_ARGS__; STB_LANG_PARSER_ERROR_FALLOUT("statement");
#define STB_LANG_PARSE_EXPR(...) __VA_ARGS__;STB_LANG_PARSER_ERROR_FALLOUT("expression"); 
#define STB_LANG_BINDING_POWER(...) __VA_ARGS__
#define STB_LANG_MATCH_BINDING_POWER(t, v) case t: return v;

#define STB_LANG_PARSER_UPDATE() token = parser->tokens.data[parser->cursor];
#define STB_LANG_PARSER_ADVANCE(...) if (STB_CONCAT(CUR_PARSER_PREFIX, _advance)(parser) == -1){stb_lang_error_minor(parser->file.name, parser->file.contents, token.offset, "OutOfBoundsError", "Went out of bounds to peek");}else {__VA_ARGS__; STB_LANG_PARSER_UPDATE()};


#define STB_LANG_MATCH_TOKEN(typ, ...) else if (token.type == typ) {match_token = token; STB_LANG_PARSER_ADVANCE();__VA_ARGS__;}
#define STB_LANG_MATCH_VALUE(typ, val, ...) else if(token.type == typ) \
{ \
    if (strcmp(token.value, val) == 0){\
        match_token = token; STB_LANG_PARSER_ADVANCE();__VA_ARGS__; \
        break; \
    }; \
break; \
}

#define STB_LANG_SAVE(name, ...) typeof(__VA_ARGS__) name = __VA_ARGS__;

#define STB_LANG_IF_TOKEN(typ, ...) if (token.type == typ) {match_token = token; __VA_ARGS__;}
#define STB_LANG_ELSE_IF_TOKEN(...) else STB_LANG_IF_TOKEN(__VA_ARGS__)
#define STB_LANG_IF_VALUE(typ, val, ...) if (token.type == typ) {if (strcmp(token.value, val) == 0){match_token = token;__VA_ARGS__;};}

#define STB_LANG_ELSE(...) else{__VA_ARGS__;}


// STB_LANG_EXPECT_TOKEN: yes, the error messages are bland and weird, this is a TODO for later
#define STB_LANG_EXPECT_TOKEN(typ) \
if (token.type != typ){ \
    stb_lang_error_minor(parser->file.name, parser->file.contents, token.offset, "ExpectError", "Expected token '%d', got '%d'", typ, token.type); \
}else {STB_LANG_PARSER_ADVANCE();}

#define STB_LANG_TOKEN_MATCH_AST(tok, ast) else if (op_token.type == tok){ \
    parent->type = ast; \
}


#define STB_LANG_PRATT_PARSER(...) \
if (left == NULL) { STB_LANG_PARSER_ERROR_FALLOUT("expression"); } \
while (parser->cursor < parser->tokens.datalen) { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) next_tok = parser->tokens.data[parser->cursor]; \
    int next_bp = STB_CONCAT(CUR_PARSER_PREFIX, _binding_power)(next_tok.type); \
    if (next_bp <= binding_power) { \
        break; \
    } \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *parent = malloc(sizeof(*parent)); \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) op_token = next_tok; \
    if (0){}__VA_ARGS__ else {break;}; \
\
    STB_LANG_PARSER_ADVANCE(); \
 \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *right = STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(parser, next_bp); \
 \
    parent->typeinfo = -1; \
    parent->value = op_token.value; \
    parent->left = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)left; \
    parent->right = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)right; \
    parent->next = NULL; \
    parent->offset = offset; \
 \
    left = parent; \
} \
 \
return left;



// The typeinfo of an AST can be left as -1 if it's unknown

#define STB_LANG_NEW_PARSER(bp_code, types, _body, _ast, _expr) \
typedef enum { \
    STB_LANG_AST_NONE = -2, \
    STB_LANG_AST_TYPEINFO = -1, \
    types \
}STB_CONCAT(CUR_PARSER_NAME, _ASTType); \
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _ASTType) type;\
    char *value; \
    char flags; \
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *left;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *right;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *next; \
    int offset; \
    int typeinfo; \
}STB_CONCAT(CUR_PARSER_NAME, _AST); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token) tokens; \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    int cursor; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _File) file; \
}CUR_PARSER_NAME; \
typedef struct { \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
}STB_CONCAT(CUR_PARSER_NAME, _ASTList); \
int STB_CONCAT(CUR_PARSER_PREFIX, _binding_power)(STB_CONCAT(CUR_TOKENIZER_NAME, _TokenType) type){ \
    switch(type){ \
        bp_code; \
        default: return 0; \
    }; \
    return 0; \
}; \
CUR_PARSER_NAME *STB_CONCAT(CUR_PARSER_PREFIX, _init)(CUR_TOKENIZER_NAME *tokenizer) { \
    CUR_PARSER_NAME *parser = malloc(sizeof(*parser)); \
    parser->cursor = 0; \
    parser->tokens = tokenizer->tokens; \
    InitLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    parser->file = tokenizer->file; \
    return parser; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _advance)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen){ \
        return -1; \
    } \
    parser->cursor++; \
    return 0;\
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(CUR_PARSER_NAME *parser, int binding_power) { \
    if (parser->cursor >= parser->tokens.datalen) {goto exit;} \
    int Generic; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    int offset = token.offset; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *left = NULL; \
    if (0){}_expr; \
    if (left == NULL) {return NULL;}; \
exit: \
    return NULL; \
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen) {goto exit;} \
    int Generic; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    int offset = token.offset; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    if (0){}_ast; \
exit: \
    return NULL; \
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) *STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen) {goto exit;} \
    int Generic; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    int offset = token.offset; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    if (0){}_body; \
exit: \
    return NULL; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _parse_body)(CUR_PARSER_NAME *parser) { \
    int oldCursor = parser->cursor; \
    AppendToLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST), *(STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(parser))); \
    if (parser->cursor == oldCursor){ \
        return STB_CONCAT(CUR_PARSER_PREFIX, _advance)(parser);\
    }else if (parser->cursor >= parser->tokens.datalen){ \
        return -1; \
    } \
    return 0; \
}

// ^ You'll notice an `int Generic` in some of these functions, it's a generic integer that can be used in macros 



#define STB_LANG_MATCH_TYPEINFO(val, typeinfo) if(strcmp(string, val) == 0) {return typeinfo;}

#define STB_LANG_TYPEINFO(...) \
int STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_type)(char *string){ \
    if (string == NULL) return -1; \
    __VA_ARGS__; \
    return -1; \
}

#define STB_LANG_TYPEINFO_SIZE(...) \
int STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_size)(int typeinf){ \
    if (typeinf == -1) return -1; \
    __VA_ARGS__; \
    return -1; \
}

#define STB_LANG_MATCH_TYPEINFO_SIZE(typeinfo, size) if(typeinf==typeinfo) {return size;}

#define STB_LANG_DEFINE_TYPEINFO(...) \
typedef enum { \
    STB_LANG_AST_TYPE_VARIADIC = -2, \
    STB_LANG_AST_TYPE_NONE = -1, \
    __VA_ARGS__ \
}STB_CONCAT(CUR_TYPEINFO_NAME, _Typeinfo);
// ^ some typeinfo stuff for the user

#define STB_LANG_AST_LITERAL(typ, token) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    _n->typeinfo = -1; \
    _n->type = typ; \
    _n->value = token.value; \
    _n->left = NULL; \
    _n->right = NULL; \
    _n->next = NULL; \
    _n->offset = offset; \
    _n; \
})

#define STB_LANG_AST_ASSIGN_DECL(assign, decl, typeinfo, name, eqs) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    _n->typeinfo = typeinfo; \
    _n->type = typeinfo == -1 ? assign : decl; \
    _n->value = name.value; \
    _n->left = NULL; \
    _n->right = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)eqs; \
    _n->next = NULL; \
    _n->offset = offset; \
    _n; \
})

#define STB_LANG_AS_AST(...) (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)__VA_ARGS__
#define STB_LANG_LINKED_LIST(what) STB_LANG_AS_AST(GetLinkedListHead(what, STB_CONCAT(CUR_PARSER_NAME, _AST)))


#define STB_LANG_AST(...) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    *_n = (STB_CONCAT(CUR_PARSER_NAME, _AST)){__VA_ARGS__}; \
    _n->offset = offset; \
    _n->flags = 0; \
    _n; \
})

#define STB_LANG_AST_FUNCALL(typ, name, params) \
({ \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *_n = malloc(sizeof(*_n)); \
    _n->typeinfo = -1; \
    _n->type = typ; \
    _n->value = name.value; \
    _n->left = (struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)GetLinkedListHead(params, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    _n->right = NULL; \
    _n->next = NULL; \
    _n->offset = offset; \
    _n; \
})

#define STB_LANG_OPERAND(var, val) \
    typeof(val) var = val; \
    STB_LANG_PARSER_UPDATE();

#define STB_LANG_PARSE_CUSTOM_LIST(starttok, splitA, endtok, ...) \
STB_LANG_EXPECT_TOKEN(starttok) \
Generic = 0; \
while (token.type != endtok){ \
    STB_LANG_SAVE(old_tok, token); \
    __VA_ARGS__; \
    STB_LANG_PARSER_UPDATE(); \
    if (token.type == splitA && splitA != -1) { \
        Generic = 1; \
        STB_LANG_PARSER_ADVANCE(); \
    }else {Generic = 0;} \
} \
if (Generic == 1){ \
    stb_lang_error_minor(parser->file.name, parser->file.contents, token.offset, "ExprListError", "Split token found before end"); \
} \
STB_LANG_EXPECT_TOKEN(endtok)



#define STB_LANG_PARSE_EXPR_LIST(into, starttok, splitA, endtok) \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
STB_LANG_PARSE_CUSTOM_LIST(starttok, splitA, endtok,  \
    STB_CONCAT(CUR_PARSER_NAME, _AST) *ast = STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(parser, 0); \
    AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), *ast); \
)



#define STB_LANG_PARSE_TYPEINFO(name, tok) \
Generic = STB_CONCAT3(CUR_TYPEINFO_PREFIX, _typeinfo, _lookup_type)(tok.value); \
STB_LANG_SAVE(name, Generic); \
if (name != -1)


#define STB_LANG_PARSE_STATEMENT_LIST(into, starttok, splitA, endtok) \
STB_LANG_EXPECT_TOKEN(starttok) \
Generic = 0; \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
while (token.type != endtok){ \
    STB_LANG_SAVE(old_tok, token); \
    STB_CONCAT(CUR_PARSER_NAME, _AST) ast = *(STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(parser)); \
    if (ast.type == STB_LANG_AST_NONE) {STB_LANG_PARSER_ADVANCE();} \
    AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), ast); \
    STB_LANG_PARSER_UPDATE(); \
    if (token.type == splitA && splitA != -1) { \
        Generic = 1; \
        STB_LANG_PARSER_ADVANCE(); \
    }else {Generic = 0;} \
} \
if (Generic == 1){ \
    stb_lang_error_minor(parser->file.name, parser->file.contents, token.offset, "ExprListError", "Generic found before end"); \
} \
STB_LANG_EXPECT_TOKEN(endtok)


// For C-like arguments
#define STB_LANG_PARSE_ARGUMENT_LIST(into, start, split, end, var) \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
STB_LANG_PARSE_CUSTOM_LIST(start, split, end, \
    STB_LANG_PARSE_TYPEINFO(argt, token){ \
        STB_LANG_SAVE(argtype, token); \
        STB_LANG_PARSER_ADVANCE() \
        STB_LANG_IF_TOKEN(TOKEN_ID, \
            STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=var, .typeinfo=argt, .value=match_token.value, .offset=offset}; \
            AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), ast); \
        )\
        STB_LANG_PARSER_ADVANCE(); \
    }\
)

#endif
