/* PARSER.H library
 * Yes, there's documentation for this library and not the tokenizer library.
 * This library is going to be a bit more complicated so it needs some planning
*/
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

#define STB_LANG_ASTS(...) __VA_ARGS__
#define STB_LANG_PARSE_BODY(...) __VA_ARGS__
#define STB_LANG_PARSE_AST(...) __VA_ARGS__
#define STB_LANG_PARSE_EXPR(...) __VA_ARGS__

#define STB_LANG_PARSER_UPDATE() token = parser->tokens.data[parser->cursor];
#define STB_LANG_PARSER_PEEK(...) if (STB_CONCAT(CUR_PARSER_PREFIX, _peek)(parser) == -1){stb_lang_error_major_global("OutOfBoundsError", "Went out of bounds to peek");}else {__VA_ARGS__; STB_LANG_PARSER_UPDATE();};

#define STB_LANG_MATCH_TOKEN(type, ...) case type: match_token = token; STB_LANG_PARSER_PEEK();__VA_ARGS__; break;
#define STB_LANG_MATCH_VALUE(typ, val, ...) case typ: \
{ \
    if (strcmp(token.value, val) == 0){\
        match_token = token; STB_LANG_PARSER_PEEK();__VA_ARGS__; \
        break; \
    }; \
break; \
}

#define STB_LANG_SAVE(name, thing) typeof(thing) name = thing;

#define STB_LANG_IF_TOKEN(typ, ...) if (token.type == typ) {match_token = token; STB_LANG_PARSER_PEEK();__VA_ARGS__; break;}
#define STB_LANG_IF_VALUE(typ, val, ...) if (token.type == typ) {if (strcmp(token.value, val) == 0){match_token = token;STB_LANG_PARSER_PEEK();__VA_ARGS__;}; break;}



// STB_LANG_EXPECT_TOKEN: yes, the error messages are bland and weird, this is a TODO for later
#define STB_LANG_EXPECT_TOKEN(typ) \
if (token.type != typ){ \
    stb_lang_error_major_global("ExpectError", "Expected token '%d', got '%d'\n", typ, token.type); \
}else {STB_LANG_PARSER_PEEK();}


#define STB_LANG_NEW_PARSER(types, _body, _ast, _expr) \
typedef enum { \
    types \
}STB_CONCAT(CUR_PARSER_NAME, _ASTType); \
typedef struct { \
    STB_CONCAT(CUR_PARSER_NAME, _ASTType) type;\
    char *value; \
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *left;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *right;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *next; \
}STB_CONCAT(CUR_PARSER_NAME, _AST); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token) tokens; \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    int cursor; \
}CUR_PARSER_NAME; \
typedef struct { \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
}STB_CONCAT(CUR_PARSER_NAME, _ASTList); \
CUR_PARSER_NAME *STB_CONCAT(CUR_PARSER_PREFIX, _init)(CUR_TOKENIZER_NAME *tokenizer) { \
    CUR_PARSER_NAME *parser = malloc(sizeof(*parser)); \
    parser->cursor = 0; \
    parser->tokens = tokenizer->tokens; \
    InitLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    return parser; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _peek)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen){ \
        return -1; \
    } \
    parser->cursor++; \
    return 0;\
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(CUR_PARSER_NAME *parser) { \
    int split; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    switch(token.type){ \
        _expr \
        default: break; \
    } \
    return (STB_CONCAT(CUR_PARSER_NAME, _AST)){0}; \
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(CUR_PARSER_NAME *parser) { \
    int split; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    switch(token.type){ \
        _ast \
        default: break; \
    } \
    return (STB_CONCAT(CUR_PARSER_NAME, _AST)){0}; \
} \
STB_CONCAT(CUR_PARSER_NAME, _AST) STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(CUR_PARSER_NAME *parser) { \
    int split; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) match_token = parser->tokens.data[parser->cursor]; \
    switch(token.type){ \
        _body \
        default: break; \
    } \
    return (STB_CONCAT(CUR_PARSER_NAME, _AST)){0}; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _parse_body)(CUR_PARSER_NAME *parser) { \
    int oldCursor = parser->cursor; \
    AppendToLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST), STB_CONCAT(CUR_PARSER_PREFIX, _parse_body_ast)(parser)); \
    if (parser->cursor == oldCursor){ \
        return STB_CONCAT(CUR_PARSER_PREFIX, _peek)(parser);\
    }else if (parser->cursor >= parser->tokens.datalen){ \
        return -1; \
    } \
    return 0; \
}

// You'll notice an `int split` in some of these functions, it's because we use them for statement and AST lists

#define STB_LANG_AST_LITERAL(typ, token) (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type = typ, .value = token.value}
#define STB_LANG_AST_FUNCDEF(typ, name, params, block) \
(STB_CONCAT(CUR_PARSER_NAME, _AST)){ \
    .type = typ,  \
    .left=(struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)GetLinkedListHead(params, STB_CONCAT(CUR_PARSER_NAME, _AST)), \
    .right=(struct STB_CONCAT(CUR_PARSER_NAME, _AST)*)GetLinkedListHead(block, STB_CONCAT(CUR_PARSER_NAME, _AST)) \
}

#define STB_LANG_PARSE_EXPR_LIST(into, starttok, splitA, endtok) \
STB_LANG_EXPECT_TOKEN(starttok) \
split = 0; \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
while (token.type != endtok){ \
    STB_LANG_SAVE(old_tok, token); \
    STB_CONCAT(CUR_PARSER_NAME, _AST) ast = STB_CONCAT(CUR_PARSER_PREFIX, _parse_expr)(parser); \
    AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), ast); \
    STB_LANG_PARSER_UPDATE(); \
    if (token.type == splitA && splitA != -1) { \
        split = 1; \
        STB_LANG_PARSER_PEEK(); \
    }else {split = 0;} \
} \
if (split == 1){ \
    stb_lang_error_major_global("ExprListError", "Split found before end"); \
} \
STB_LANG_EXPECT_TOKEN(endtok)


#define STB_LANG_PARSE_STATEMENT_LIST(into, starttok, splitA, endtok) \
STB_LANG_EXPECT_TOKEN(starttok) \
split = 0; \
STB_CONCAT(CUR_PARSER_NAME, _ASTList) into = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0}; \
InitLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST)); \
while (token.type != endtok){ \
    STB_LANG_SAVE(old_tok, token); \
    STB_CONCAT(CUR_PARSER_NAME, _AST) ast = STB_CONCAT(CUR_PARSER_PREFIX, _parse_ast)(parser); \
    AppendToLinkedList(into, STB_CONCAT(CUR_PARSER_NAME, _AST), ast); \
    STB_LANG_PARSER_UPDATE(); \
    if (token.type == splitA && splitA != -1) { \
        split = 1; \
        STB_LANG_PARSER_PEEK(); \
    }else {split = 0;} \
} \
if (split == 1){ \
    stb_lang_error_major_global("ExprListError", "Split found before end"); \
} \
STB_LANG_EXPECT_TOKEN(endtok)
