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


#define STB_LANG_MATCH_TOKEN(type, ...) case type: __VA_ARGS__; break;
#define STB_LANG_MATCH_VALUE(type, val, ...) case type: {if (strcmp(token.value, val) == 0){__VA_ARGS__;break;}; break;}


#define STB_LANG_NEW_PARSER(types, _body) \
typedef enum { \
    types \
}STB_CONCAT(CUR_PARSER_NAME, _ASTType); \
typedef struct { \
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *left;\
    struct STB_CONCAT(CUR_PARSER_NAME, _AST) *right;\
}STB_CONCAT(CUR_PARSER_NAME, _AST); \
typedef struct { \
    STB_CONCAT3(dymarray_, CUR_TOKENIZER_NAME, _Token) tokens; \
    LinkedList(STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    int cursor; \
}CUR_PARSER_NAME; \
CUR_PARSER_NAME *STB_CONCAT(CUR_PARSER_PREFIX, _init)(CUR_TOKENIZER_NAME *tokenizer) { \
    CUR_PARSER_NAME *parser = malloc(sizeof(*parser)); \
    parser->cursor = 0; \
    parser->tokens = tokenizer->tokens; \
    InitLinkedList((*parser), STB_CONCAT(CUR_PARSER_NAME, _AST)); \
    return parser; \
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _peek)(CUR_PARSER_NAME *parser) { \
    if (parser->cursor >= parser->tokens.datalen - 1){ \
        return -1; \
    } \
    parser->cursor++; \
    return 0;\
} \
char STB_CONCAT(CUR_PARSER_PREFIX, _parse_body)(CUR_PARSER_NAME *parser) { \
    STB_CONCAT(CUR_TOKENIZER_NAME, _Token) token = parser->tokens.data[parser->cursor]; \
    switch(token.type){ \
        _body \
        default: break; \
    } \
    return STB_CONCAT(CUR_PARSER_PREFIX, _peek)(parser);\
}


// STB_LANG_NEW_PARSER(
//     STB_LANG_ASTS(
//         STB_LANG_AST(AST_FUNCALL, 10),
//         STB_LANG_AST(AST_INT, 20)
//     ),
//     STB_LANG_PARSE_BODY(
//         STB_LANG_MATCH_TOKEN(TOKEN_KEYWORD,
//             STB_LANG_CAPTURE_TOKEN(),
//             STB_LANG_IF_TYPE(TOKEN_ID,
//                 STB_LANG_BECOME_TYPE(AST_FUNCALL)
//             )
//         )
//     )
// )
