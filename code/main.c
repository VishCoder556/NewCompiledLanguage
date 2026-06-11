/* Programming Language Development
 *
 * -- I haven't worked on this file yet, but I will soon --
 * -- Check 'description' for a description on what I'm doing --
 *
 *
 *
*/

#define STB_LANG_ERROR_IMPLEMENTATION


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../libraries/error/error.h"
#include "../libraries/tokenizer/tokenizer.h"
#include "../libraries/parser/parser.h"

char *HELP = "New Programming Language HELP Manual\n\t"
"-help: creates this page\n";


#define CUR_TOKENIZER_NAME Lang_Tokenizer
#define CUR_TOKENIZER_PREFIX lang_tokenizer

STB_LANG_NEW_TOKENIZER( 
    STB_LANG_TOKENS(
        TOKEN_LP,
        TOKEN_RP,
        TOKEN_LB,
        TOKEN_RB,
        TOKEN_ID,
        TOKEN_NUM,
        TOKEN_COMMA
    ),
    STB_LANG_SIMPLE_CASES(
        STB_LANG_TOKEN_CHAR('(', TOKEN_LP)
        STB_LANG_TOKEN_CHAR(')', TOKEN_RP)
        STB_LANG_TOKEN_CHAR('{', TOKEN_LB)
        STB_LANG_TOKEN_CHAR('}', TOKEN_RB)
        STB_LANG_TOKEN_CHAR(',', TOKEN_COMMA)

        STB_LANG_SKIP('\n')
    ),
    STB_LANG_ALPHA(TOKEN_ID)
    STB_LANG_NUM(TOKEN_NUM)
    STB_LANG_SKIP(' ')
)


#define CUR_PARSER_NAME Lang_Parser
#define CUR_PARSER_PREFIX lang_parser

STB_LANG_NEW_PARSER(
    STB_LANG_ASTS(
        AST_FUNCDEF,
        AST_VAR
    ),
    STB_LANG_PARSE_BODY(
        STB_LANG_MATCH_VALUE(TOKEN_ID, "func", 
            STB_LANG_IF_TOKEN(TOKEN_ID, // Function name
                STB_LANG_SAVE(func_name, match_token);
                STB_LANG_PARSE_EXPR_LIST(params, TOKEN_LP, TOKEN_COMMA, TOKEN_RP)
                STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
                return STB_LANG_AST_FUNCDEF(AST_FUNCDEF, func_name, params, stmnts)
            )
        )
    ),
    STB_LANG_PARSE_AST(
        // No ASTS yet!
        return (Lang_Parser_AST){0};
    ),
    STB_LANG_PARSE_EXPR(
        STB_LANG_MATCH_TOKEN(TOKEN_ID,  
            return STB_LANG_AST_LITERAL(AST_VAR, match_token);
        )
    )
)

int main(int argc, char **argv){

    char *input_file = NULL;
    for (int i=1; i<argc; i++){
        char *str = argv[i];
        if (str == NULL){break;};
        if (strcmp(str, "-help") == 0){
            fprintf(stderr, "%s", HELP);
            exit(-1);
        }else {
            input_file = str;
        }
    }
    if (input_file == NULL){
        stb_lang_error_major_global("ArgsError", "No input file provided");
    }
    Lang_Tokenizer *tokenizer = lang_tokenizer_init("examples/a.lang");
    while (lang_tokenizer_token(tokenizer) == 0){
    }
    // int len = tokenizer->tokens.datalen;
    // for (int i=0; i<len; i++){
    //     printf("%d\n", tokenizer->tokens.data[i].type);
    // }
    Lang_Parser *parser = lang_parser_init(tokenizer);
    while (lang_parser_parse_body(parser) == 0){
    }
    Lang_Parser_AST *ast = GetLinkedListHead((*parser), Lang_Parser_AST);
    Lang_Parser_AST *left = (Lang_Parser_AST*)ast->left;
    printf("%s, %s\n", left->value, ((Lang_Parser_AST*)left->next)->value);
    return 0;
}
