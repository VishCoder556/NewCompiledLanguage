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
#include "../libraries/typeinfo/typeinfo.h"
#include "../libraries/ir/ir.h"

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
        TOKEN_COMMA,
        TOKEN_EQ
    ),
    STB_LANG_SIMPLE_CASES(
        STB_LANG_TOKEN_CHAR('(', TOKEN_LP)
        STB_LANG_TOKEN_CHAR(')', TOKEN_RP)
        STB_LANG_TOKEN_CHAR('{', TOKEN_LB)
        STB_LANG_TOKEN_CHAR('}', TOKEN_RB)
        STB_LANG_TOKEN_CHAR(',', TOKEN_COMMA)
        STB_LANG_TOKEN_CHAR('=', TOKEN_EQ)

        STB_LANG_SKIP('\n')
    ),
    STB_LANG_ALPHA(TOKEN_ID)
    STB_LANG_NUM(TOKEN_NUM)
    STB_LANG_SKIP(' ')
)



#define CUR_TYPEINFO_NAME Lang_TypeInfo
#define CUR_TYPEINFO_PREFIX lang_typeinfo
STB_LANG_DEFINE_TYPEINFO(
    AST_TYPE_VOID,
    AST_TYPE_INT
)
STB_LANG_TYPEINFO(
    STB_LANG_MATCH_TYPEINFO("void", AST_TYPE_VOID)
    STB_LANG_MATCH_TYPEINFO("int", AST_TYPE_INT)
)




#define CUR_PARSER_NAME Lang_Parser
#define CUR_PARSER_PREFIX lang_parser

STB_LANG_NEW_PARSER(
STB_LANG_ASTS(
    AST_FUNCDEF,
    AST_VAR,
    AST_INT,
    AST_ASSIGN
),
STB_LANG_PARSE_BODY(
    STB_LANG_MATCH_TOKEN(TOKEN_ID, 
        STB_LANG_PARSE_TYPEINFO(typeinfo, match_token){
            STB_LANG_IF_TOKEN(TOKEN_ID, // Function name
                STB_LANG_SAVE(func_name, match_token);
                STB_LANG_PARSE_ARGUMENT_LIST(params, TOKEN_LP, TOKEN_COMMA, TOKEN_RP)
                STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
                Lang_Parser_AST *ast = (Lang_Parser_AST*)GetLinkedListHead(params, Lang_Parser_AST);
                return STB_LANG_AST_FUNCDEF(AST_FUNCDEF, func_name, params, stmnts)
            )
        }
    )
),
STB_LANG_PARSE_AST(
    // No ASTS yet!
    STB_LANG_PARSE_TYPEINFO(typeinfo, token){
        STB_LANG_PARSER_PEEK()
    maybe_assign:
        STB_LANG_IF_TOKEN(TOKEN_ID, 
            STB_LANG_SAVE(varia, match_token)
            STB_LANG_IF_TOKEN(TOKEN_EQ,
                STB_LANG_OPERAND(assign, lang_parser_parse_expr(parser));
                return STB_LANG_AST_ASSIGN(AST_ASSIGN, typeinfo, varia, assign);
            )
        )
    }else {
        goto maybe_assign;
    }
),
STB_LANG_PARSE_EXPR(
    STB_LANG_MATCH_TOKEN(TOKEN_ID,  
        return STB_LANG_AST_LITERAL(AST_VAR, match_token);
    )
    STB_LANG_MATCH_TOKEN(TOKEN_NUM,  
        return STB_LANG_AST_LITERAL(AST_INT, match_token);
    )
)
)

STB_LANG_NEW_TYPEINFO(
    STB_LANG_TYPEINFO_CASE(AST_FUNCDEF){
        STB_LANG_EXPAND_PARAMS();
        STB_LANG_EXPAND_BLOCK();
    }
    STB_LANG_TYPEINFO_CASE(AST_ASSIGN){
        STB_LANG_ASSIGN()
    }
    STB_LANG_TYPEINFO_CASE(AST_INT){
        STB_LANG_ASSUME_TYPE(AST_TYPE_INT);
    }
)

#define CUR_IR_NAME Lang_IR
#define CUR_IR_PREFIX lang_ir

STB_LANG_NEW_IR(
    STB_LANG_IR_INSTRS(
        IR_FUNCDEF_BEGIN,
        IR_FUNCDEF_END,
        IR_ASSIGN,
        IR_POP, // pop the stack
        IR_PUSH // push the stack (for later on, not used yet)
    ),
    STB_LANG_IR_CASES(
        STB_LANG_IR_CASE(AST_FUNCDEF,
            STB_LANG_IR_FUNCDEF(IR_FUNCDEF_BEGIN, IR_POP, IR_FUNCDEF_END)
        )
        STB_LANG_IR_CASE(AST_ASSIGN,
            STB_LANG_IR_ASSIGN(IR_ASSIGN);
        )
        STB_LANG_IR_CASE(AST_INT,
            STB_LANG_IR_RETURN_SELF();
        )
    )
);

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

    // Lang_Parser_AST *ast = GetLinkedListHead((*parser), Lang_Parser_AST);
    // Lang_Parser_AST *left = (Lang_Parser_AST*)ast->left;
    // printf("%s\n", left->value);


    Lang_TypeInfo *typeinfo = lang_typeinfo_init(parser);
    while (lang_typeinfo_check(typeinfo) == 0){
    }
    free(parser);

    Lang_IR *ir = lang_ir_init(typeinfo);
    while (lang_ir_translate(ir) == 0){
    }
    return 0;
}
