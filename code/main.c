/* Programming Language Development
 *
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
#include "../libraries/preprocessor/preprocessor.h"
#include "../libraries/parser/parser.h"
#include "../libraries/typeinfo/typeinfo.h"
#include "../libraries/ir/ir.h"
#include "../libraries/codegen/codegen.h"
#include "../libraries/driver/driver.h"
#include "../libraries/regalloc/regalloc.h"

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
        TOKEN_LSB,
        TOKEN_RSB,
        TOKEN_ID,
        TOKEN_NUM,
        TOKEN_COMMA,
        TOKEN_EQ,
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_MODULO,
        TOKEN_STRING,
        TOKEN_GT,
        TOKEN_GTE,
        TOKEN_LT,
        TOKEN_LTE,
        TOKEN_DEQ,
        TOKEN_NEQ,
        TOKEN_NOT,
        TOKEN_AND,
        TOKEN_BAND,
        TOKEN_OR,
        TOKEN_BOR,
        TOKEN_CARET,
        TOKEN_DOT,
        TOKEN_HASH
    ),
    STB_LANG_SIMPLE_CASES(
        STB_LANG_TOKEN_CHAR('(', TOKEN_LP)
        STB_LANG_TOKEN_CHAR(')', TOKEN_RP)
        STB_LANG_TOKEN_CHAR('{', TOKEN_LB)
        STB_LANG_TOKEN_CHAR('}', TOKEN_RB)
        STB_LANG_TOKEN_CHAR('[', TOKEN_LSB)
        STB_LANG_TOKEN_CHAR(']', TOKEN_RSB)
        STB_LANG_TOKEN_CHAR(',', TOKEN_COMMA)
        STB_LANG_TOKEN_CHAR('+', TOKEN_ADD)
        STB_LANG_TOKEN_CHAR('-', TOKEN_SUB)
        STB_LANG_TOKEN_CHAR('*', TOKEN_MUL)
        STB_LANG_TOKEN_CHAR('/', TOKEN_DIV)
        STB_LANG_TOKEN_CHAR('%', TOKEN_MODULO)
        STB_LANG_TOKEN_CHAR('^', TOKEN_CARET)
        STB_LANG_TOKEN_CHAR('.', TOKEN_DOT)
        STB_LANG_TOKEN_CHAR('#', TOKEN_HASH)
        STB_LANG_SKIP('\n')
    ),
    STB_LANG_ALPHA(TOKEN_ID)
    STB_LANG_NUM(TOKEN_NUM)
    STB_LANG_STRING('"', TOKEN_STRING)
    STB_LANG_TOKEN_DOUBLE_CHAR(">=", TOKEN_GTE, TOKEN_GT)
    STB_LANG_TOKEN_DOUBLE_CHAR("<=", TOKEN_LTE, TOKEN_LT)
    STB_LANG_TOKEN_DOUBLE_CHAR("==", TOKEN_DEQ, TOKEN_EQ)
    STB_LANG_TOKEN_DOUBLE_CHAR("!=", TOKEN_NEQ, TOKEN_NOT)
    STB_LANG_TOKEN_DOUBLE_CHAR("&&", TOKEN_AND, TOKEN_BAND)
    STB_LANG_TOKEN_DOUBLE_CHAR("||", TOKEN_OR, TOKEN_BOR)
    
    STB_LANG_TOKEN_COMMENT_LINE("//")
    STB_LANG_SKIP(' ')
)


#define CUR_PREPROCESSOR_NAME Lang_Preprocessor
#define CUR_PREPROCESSOR_PREFIX lang_preprocessor
STB_LANG_NEW_PREPROCESSOR(

STB_LANG_PREPROCESSOR_PROCESS(
    if (token.type == TOKEN_HASH){
        STB_LANG_SAVE(oldcursor, processor->cursor);
        STB_LANG_PROCESSOR_ADVANCE();
        if (token.type == TOKEN_NOT){
            STB_LANG_PROCESSOR_ADVANCE();
            if (token.type == TOKEN_ID){
                if (strcmp(token.value, "include") == 0){
                    STB_LANG_PROCESSOR_ADVANCE();
                    if (token.type == TOKEN_STRING){
                STB_LANG_SAVE(name, token.value);
                        STB_LANG_PROCESSOR_TRIM(oldcursor, processor->cursor+1);
                        processor->cursor = oldcursor;
                        STB_LANG_PROCESSOR_UPDATE();

                        Lang_Tokenizer *_tokenizer = lang_tokenizer_init(name);
                        while (lang_tokenizer_token(_tokenizer) == 0){
                        }
                        Lang_Preprocessor *_processor = lang_preprocessor_init(_tokenizer, processor->fl+1);
                        while (lang_preprocessor_token(_processor) == 0){
                        }
                        STB_LANG_PROCESSOR_INSERT(oldcursor, _processor);

                        free(_tokenizer);
                        free(_processor);

                    };
                }
            }
        }else {
            stb_lang_error_minor(processor->file.name, processor->file.contents, token.offset, "SyntaxError", "Unexpected token after `#`");
        }
    }
)
)
        

#define CUR_TYPEINFO_NAME Lang_TypeInfo
#define CUR_TYPEINFO_PREFIX lang_typeinfo


#define CUR_PARSER_NAME Lang_Parser
#define CUR_PARSER_PREFIX lang_parser

STB_LANG_DEFINE_TYPEINFO(
    AST_TYPE_VOID,
    AST_TYPE_INT,
    AST_TYPE_CHAR,
    AST_TYPE_STRING,
    AST_TYPE_ARRAY,
    AST_TYPE_STRUCT
)


STB_LANG_NEW_PARSER(
STB_LANG_BINDING_POWER(
    STB_LANG_MATCH_BINDING_POWER(TOKEN_OR, 1)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_AND, 2)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_BOR, 3)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_CARET, 4)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_BAND, 5)


    STB_LANG_MATCH_BINDING_POWER(TOKEN_LT, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_LTE, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_GT, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_GTE, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DEQ, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_NEQ, 6)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_ADD, 10)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_SUB, 10)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_MUL, 20)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DIV, 20)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_MODULO, 20)


    STB_LANG_MATCH_BINDING_POWER(TOKEN_LSB, 30)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DOT, 30)
),
STB_LANG_ASTS(
    AST_FUNCDEF,
    AST_FUNCDECL,
    AST_VAR,
    AST_INT,
    AST_ASSIGN,
    AST_DECL,
    AST_FUNCALL,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_MODULO,
    AST_IF,
    AST_WHILE,
    AST_LT,
    AST_LTE,
    AST_GT,
    AST_GTE,
    AST_EQ,
    AST_NEQ,
    AST_RET,
    AST_STRING,
    AST_CAST,
    AST_OR,
    AST_AND,
    AST_BOR,
    AST_BAND,
    AST_XOR,
    AST_EXPR,
    AST_REF,
    AST_DEREF,
    AST_STORE,
    AST_INDEX,
    AST_STRUCT,
    AST_ACCESS
),
STB_LANG_PARSE_BODY(
    STB_LANG_IF_VALUE(TOKEN_ID, "struct",
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_SAVE(struct_name, token);
        STB_LANG_PARSER_ADVANCE();
        STB_CONCAT(CUR_PARSER_NAME, _ASTList) fields = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0};
        InitLinkedList(fields, STB_CONCAT(CUR_PARSER_NAME, _AST));
        STB_LANG_PARSE_CUSTOM_LIST(TOKEN_LB, -1, TOKEN_RB,
            STB_LANG_GET_TYPEINFO(argt){
                STB_LANG_IF_TOKEN(TOKEN_ID,
                    STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=AST_VAR, .typeinfo=argt, .value=match_token.value, .offset=offset};
                    AppendToLinkedList(fields, STB_CONCAT(CUR_PARSER_NAME, _AST), ast);
                )
                STB_LANG_PARSER_ADVANCE();
            }
        )
        Lang_TypeInfo_Typeinfo typeinfo = STB_LANG_TYPEINFO(.type=AST_TYPE_STRUCT, .ptrnum=0);
        typeinfo.data.struct1.name = struct_name.value;
        typeinfo.data.struct1.symbol = NULL;
        typeinfo.data.struct1.size = -1;
        return STB_LANG_AST(.type=AST_STRUCT, .typeinfo=typeinfo, .value=struct_name.value, .left=STB_LANG_LINKED_LIST(fields), .right=NULL);
    )
    STB_LANG_GET_TYPEINFO(typeinfo){
        STB_LANG_IF_TOKEN(TOKEN_ID, // Function name
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_SAVE(func_name, match_token);

            STB_CONCAT(CUR_PARSER_NAME, _ASTList) params = (STB_CONCAT(CUR_PARSER_NAME, _ASTList)){0};
            InitLinkedList(params, STB_CONCAT(CUR_PARSER_NAME, _AST));
            STB_LANG_PARSE_CUSTOM_LIST(TOKEN_LP, TOKEN_COMMA, TOKEN_RP,
                STB_LANG_GET_TYPEINFO(argt){
                    STB_LANG_IF_TOKEN(TOKEN_ID,
                        STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=AST_VAR, .typeinfo=argt, .value=match_token.value, .offset=offset};
                        AppendToLinkedList(params, STB_CONCAT(CUR_PARSER_NAME, _AST), ast);
                    )
                    STB_LANG_PARSER_ADVANCE();
                }else {
                    STB_LANG_IF_TOKEN(TOKEN_DOT,
                        STB_LANG_PARSER_ADVANCE()
                        STB_LANG_IF_TOKEN(TOKEN_DOT,
                            STB_LANG_PARSER_ADVANCE()
                            STB_LANG_IF_TOKEN(TOKEN_DOT,
                                STB_LANG_PARSER_ADVANCE()

                                STB_CONCAT(CUR_PARSER_NAME, _AST) ast = (STB_CONCAT(CUR_PARSER_NAME, _AST)){.type=STB_LANG_AST_NONE, .typeinfo=STB_LANG_TYPEINFO_VARIADIC, .value=NULL, .offset=match_token.offset};
                                AppendToLinkedList(params, STB_CONCAT(CUR_PARSER_NAME, _AST), ast);
                            )
                        )
                    )
                }
            )

            if (token.type == TOKEN_LB) {
                STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
                return STB_LANG_AST(.type=AST_FUNCDEF, .typeinfo=typeinfo, .value=func_name.value, .left=STB_LANG_LINKED_LIST(params), .right=STB_LANG_LINKED_LIST(stmnts));
            }else {
                return STB_LANG_AST(.type=AST_FUNCDECL, .typeinfo=typeinfo, .value=func_name.value, .left=STB_LANG_LINKED_LIST(params), .right=NULL);
            }
        )
    }
),
STB_LANG_PARSE_AST(
    // No ASTS yet!
    STB_LANG_IF_VALUE(TOKEN_ID, "if", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_PARSER_EXPECT(TOKEN_LP)
        STB_LANG_OPERAND(ifexpr, lang_parser_parse_expr(parser, 0));
        STB_LANG_PARSER_EXPECT(TOKEN_RP)
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)

        Lang_Parser_AST *ast = STB_LANG_AST(.type=AST_IF, .value=NULL, .left=STB_LANG_AS_AST(ifexpr), .middle=NULL, .right=STB_LANG_LINKED_LIST(stmnts));

        STB_LANG_IF_VALUE(TOKEN_ID, "else",
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_PARSE_STATEMENT_LIST(else1, TOKEN_LB, -1, TOKEN_RB)
            ast->middle = STB_LANG_LINKED_LIST(else1);
        )
        return ast;
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "while", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_PARSER_EXPECT(TOKEN_LP)
        STB_LANG_OPERAND(ifexpr, lang_parser_parse_expr(parser, 0));
        STB_LANG_PARSER_EXPECT(TOKEN_RP)
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
        return STB_LANG_AST(.type=AST_WHILE, .value=NULL, .left=STB_LANG_AS_AST(ifexpr), .middle=NULL, .right=STB_LANG_LINKED_LIST(stmnts));
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "return", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_OPERAND(expr, lang_parser_parse_expr(parser, 0));
        return STB_LANG_AST(.type=AST_RET, .value=NULL, .left=STB_LANG_AS_AST(expr), .middle=NULL, .right=NULL);
    )
    STB_LANG_GET_TYPEINFO(typeinfo){
        goto assign_decl_end;
    maybe_assign:
        if (token.value != NULL){
            if (strcmp(token.value, "deref") == 0){goto not_funcall;}
        }
        STB_LANG_SAVE(varia, token)
        STB_LANG_PARSER_ADVANCE()
        STB_LANG_IF_TOKEN(TOKEN_LP,
            STB_LANG_PARSE_EXPR_LIST(args, TOKEN_LP, TOKEN_COMMA, TOKEN_RP);
            return STB_LANG_AST_FUNCALL(AST_FUNCALL, varia, args)
        ) STB_LANG_ELSE (
            STB_LANG_PARSER_BACK();
not_funcall:
            ;
            STB_LANG_GET_AST_EXPR(lhs, 0);
            STB_LANG_IF_TOKEN(TOKEN_EQ,
                STB_LANG_PARSER_ADVANCE();
                STB_LANG_OPERAND(assign, lang_parser_parse_expr(parser, 0));
                if (lhs->type == AST_INDEX || lhs->type == AST_ACCESS || lhs->type == AST_DEREF){
                    return STB_LANG_AST(.type=AST_STORE, .typeinfo=typeinfo, .value = NULL, .left=STB_LANG_AS_AST(lhs), .middle=NULL, .right=STB_LANG_AS_AST(assign));
                }
                return STB_LANG_AST(.type=AST_ASSIGN, .typeinfo=typeinfo, .value = NULL, .left=STB_LANG_AS_AST(lhs), .middle=NULL, .right=STB_LANG_AS_AST(assign));
            )
        );
    assign_decl_end:
        STB_LANG_IF_TOKEN(TOKEN_ID, 
            STB_LANG_PARSER_ADVANCE()
            STB_LANG_SAVE(varia, match_token)
            STB_LANG_IF_TOKEN(TOKEN_EQ,
                STB_LANG_PARSER_ADVANCE();
                STB_LANG_OPERAND(assign, lang_parser_parse_expr(parser, 0));
                return STB_LANG_AST(.type=AST_DECL, .typeinfo=typeinfo, .value = varia.value, .left=NULL, .middle=NULL, .right=STB_LANG_AS_AST(assign));
            ) STB_LANG_ELSE(
                if (typeinfo.type != -1){
                    return STB_LANG_AST(.type=AST_DECL, .typeinfo=typeinfo, .value = varia.value, .left=NULL, .middle=NULL, .right=NULL);
                }
            )
        )
    }else {
        goto maybe_assign;
    }
),
STB_LANG_PARSE_EXPR(
    STB_LANG_MATCH_TOKEN(TOKEN_ID,  
        STB_LANG_SAVE(thing1, token)
        STB_LANG_SAVE(thing2, match_token)

        if (strcmp(match_token.value, "cast") == 0){
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_TYPEINFO(typeinfo){
                STB_LANG_PARSER_EXPECT(TOKEN_COMMA);
                STB_LANG_GET_AST_EXPR(exp, 0);
                STB_LANG_PARSER_EXPECT(TOKEN_RP);
                left = STB_LANG_AST(.type = AST_CAST, .typeinfo = typeinfo, .left = STB_LANG_AS_AST(exp), .right=NULL, .middle=NULL);
                goto skip;
            }
        }

        if (strcmp(match_token.value, "ref") == 0){
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_AST_EXPR(exp, 0);
            STB_LANG_PARSER_EXPECT(TOKEN_RP);
            left = STB_LANG_AST(.type = AST_REF, .left = STB_LANG_AS_AST(exp), .right=NULL, .middle=NULL);
            goto skip;
        }

        if (strcmp(match_token.value, "deref") == 0){
            STB_LANG_PARSER_EXPECT(TOKEN_LP);
            STB_LANG_GET_AST_EXPR(exp, 0);
            STB_LANG_PARSER_EXPECT(TOKEN_RP);
            left = STB_LANG_AST(.type = AST_DEREF, .left = STB_LANG_AS_AST(exp), .right=NULL, .middle=NULL);
            goto skip;
        }

        STB_LANG_SAVE(name, match_token)
        STB_LANG_IF_TOKEN(TOKEN_LP,
            STB_LANG_PARSE_EXPR_LIST(args, TOKEN_LP, TOKEN_COMMA, TOKEN_RP);
            left = STB_LANG_AST_FUNCALL(AST_FUNCALL, name, args)
        ) STB_LANG_ELSE(
            left = STB_LANG_AST_LITERAL(AST_VAR, name);
            // printf("Here, %d, %s, %d, %s\n", left->type == AST_VAR, left->value, token.type, token.value);
            goto skip;
        )
    )
    STB_LANG_MATCH_TOKEN(TOKEN_LP,  
        STB_LANG_GET_AST_EXPR(l, 2);
        STB_LANG_PARSER_EXPECT(TOKEN_RP);
        left = STB_LANG_AST(.type = AST_EXPR, .left = STB_LANG_AS_AST(l))
    )
    STB_LANG_MATCH_TOKEN(TOKEN_NUM,  
        left = STB_LANG_AST_LITERAL(AST_INT, match_token);
    )
    STB_LANG_MATCH_TOKEN(TOKEN_STRING,  
        left = STB_LANG_AST_LITERAL(AST_STRING, match_token);
    )
skip:
    STB_LANG_PRATT_PARSER(
        STB_LANG_TOKEN_MATCH_AST(TOKEN_ADD, AST_ADD)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_SUB, AST_SUB)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_MUL, AST_MUL)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_DIV, AST_DIV)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_MODULO, AST_MODULO)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_LT, AST_LT)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_LTE, AST_LTE)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_GT, AST_GT)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_GTE, AST_GTE)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_DEQ, AST_EQ)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_NEQ, AST_NEQ)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_OR, AST_OR)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_BOR, AST_BOR)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_AND, AST_AND)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_BAND, AST_BAND)
        STB_LANG_TOKEN_MATCH_AST(TOKEN_CARET, AST_XOR)
        STB_LANG_TOKEN_MATCH_AST_CUSTOM(TOKEN_LSB, AST_INDEX,
            STB_LANG_PARSER_ADVANCE();
            STB_LANG_GET_AST_EXPR(new, 10);
            parent->right = STB_LANG_AS_AST(new);
            STB_LANG_PARSER_EXPECT(TOKEN_RSB);
        )
        STB_LANG_TOKEN_MATCH_AST_CUSTOM(TOKEN_DOT, AST_ACCESS,
            STB_LANG_PARSER_ADVANCE();
            parent->value = token.value;
            STB_LANG_PARSER_ADVANCE();
        )
    )
),
STB_LANG_PARSE_TYPEINFO(
    Lang_TypeInfo_Typeinfo typeinfo = (Lang_TypeInfo_Typeinfo){.type=STB_LANG_TYPEINFO_NONE, .ptrnum=0};
    STB_LANG_IF_VALUE(TOKEN_ID, "ptr",
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_PARSER_EXPECT(TOKEN_LT);
        STB_LANG_GET_TYPEINFO(old){
            old.ptrnum++;
        };
        STB_LANG_PARSER_EXPECT(TOKEN_GT);
        return old;
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "struct",
        typeinfo.type = AST_TYPE_STRUCT;
        STB_LANG_PARSER_ADVANCE();
        typeinfo.data.struct1.name = token.value;
        typeinfo.data.struct1.symbol = NULL;
        typeinfo.data.struct1.size = -1;
        STB_LANG_PARSER_ADVANCE();
        return typeinfo;
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "void",
        typeinfo.type = AST_TYPE_VOID;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "int",
        typeinfo.type = AST_TYPE_INT;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "char",
        typeinfo.type = AST_TYPE_CHAR;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "string",
        typeinfo.type = AST_TYPE_STRING;
        STB_LANG_PARSER_ADVANCE();
    )
    STB_LANG_IF_TOKEN(TOKEN_LSB,
        STB_LANG_PARSER_EXPECT(TOKEN_LSB);
        STB_LANG_SAVE(num, token)
        STB_LANG_PARSER_EXPECT(TOKEN_NUM);
        Lang_TypeInfo_Typeinfo typeinf = (Lang_TypeInfo_Typeinfo){.type=AST_TYPE_ARRAY, .ptrnum=0};
        typeinf.data.array.size = atoi(num.value);
        typeinf.data.array.elem_type = malloc(sizeof(Lang_TypeInfo_Typeinfo));
        *(Lang_TypeInfo_Typeinfo*)typeinf.data.array.elem_type = typeinfo;
        STB_LANG_PARSER_EXPECT(TOKEN_RSB);
        return typeinf;
    )
    return typeinfo;
)
)


STB_LANG_TYPEINFO_SIZE(
    if (typeinfo.ptrnum > 0) return 8;
    switch(typeinfo.type){
        case AST_TYPE_VOID: return 0;
        case AST_TYPE_INT: return 8;
        case AST_TYPE_CHAR: return 1;
        case AST_TYPE_STRING: return 8;
        case AST_TYPE_ARRAY: return STB_LANG_LOOKUP_SIZE(root_scope, typeinfo.data.array.elem_type) * typeinfo.data.array.size;
        case AST_TYPE_STRUCT: {
            STB_LANG_FIND_DATA(root_scope, typeinfo.data.struct1.name,
                int size = 0;
                Lang_Parser_AST *structdef = STB_LANG_LHS(STB_LANG_GET_AST(symnew.data.struct1.structdef));
                STB_LANG_ITERATE_LINKED_LIST(structdef, field, Lang_Parser_AST,
                    size += STB_LANG_LOOKUP_SIZE(root_scope, &field->typeinfo);
                )
                return size;
            );
        }
        default: stb_lang_error_major_global("TypeError", "Size of type '%d' is unknown", typeinfo.type); return -1;
    }
)


STB_LANG_NEW_TYPEINFO(
    STB_LANG_TYPEINFO_CASE(AST_FUNCDECL, 
        STB_LANG_MAKE_SCOPE(ast->value);
        STB_LANG_ADD_FUNCTION(ast->value,
            STB_LANG_FUNCTION_RETURN(ast->typeinfo);
            STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
        )
        STB_LANG_EXPAND_PARAMS();
    )
    STB_LANG_TYPEINFO_CASE(AST_FUNCDEF, 
        STB_LANG_MAKE_SCOPE(ast->value);
        STB_LANG_ADD_FUNCTION(ast->value,
            STB_LANG_FUNCTION_RETURN(ast->typeinfo);
            STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
        )
        STB_LANG_EXPAND_PARAMS();
        STB_LANG_EXPAND_BLOCK();
    )
    STB_LANG_TYPEINFO_CASE(AST_STRUCT, 
        int size = 0;
        STB_LANG_ITERATE_LINKED_LIST(STB_LANG_GET_AST(ast->left), head, Lang_Parser_AST,
            STB_LANG_EXPAND(head);
            size += STB_LANG_LOOKUP_SIZE(checker->root_scope, &head->typeinfo);
        )
        ast->typeinfo.data.struct1.size = size;
        ast->typeinfo.type = AST_TYPE_STRUCT;
        STB_LANG_ADD_DATA(ast->value, 
            // STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
            symnew.data.struct1.structdef = STB_LANG_AS_AST(ast);
            STB_LANG_SET_SYMBOL(ast->typeinfo.data.struct1.symbol, STB_LANG_CURRENT_SYMBOL());
        )
    )
    STB_LANG_TYPEINFO_CASE(AST_STORE, 
        STB_LANG_EXPAND_LHS();
        STB_LANG_EXPAND_RHS();
        STB_LANG_INFER_TYPE(ast->value);
        ast->typeinfo = STB_LANG_LHS(ast)->typeinfo;
        int typ = STB_LANG_LHS(ast)->type;
        if (typ == AST_ACCESS || typ == AST_DEREF || typ == AST_INDEX){
            ast->typeinfo = STB_LANG_LHS(STB_LANG_LHS(ast))->typeinfo;
        }
        if (ast->typeinfo.ptrnum != 0){
            ast->typeinfo.ptrnum--;
        }else if (ast->typeinfo.type == AST_TYPE_ARRAY){
            ast->typeinfo = *(Lang_TypeInfo_Typeinfo*)(ast->typeinfo.data.array.elem_type);
        }else if (STB_LANG_LHS(ast)->type == AST_ACCESS){
            ast->typeinfo = STB_LANG_LHS(ast)->typeinfo;
        }else if (STB_LANG_LHS(ast)->type == AST_DEREF){
            // Deref already did the typeinfo--
        }else {
            STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "DerefError", "Could not dereference anything that's not a pointer or array");
        };
        if (STB_LANG_LHS(ast)->type == AST_INDEX || STB_LANG_LHS(ast)->type == AST_ACCESS){
            // STB_LANG_EXPECT_TYPE_EQ(STB_LANG_LHS(ast), STB_LANG_RHS(ast));
        }else {
            STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        }
    )
    STB_LANG_TYPEINFO_CASE(AST_ASSIGN, 
        STB_LANG_EXPAND_LHS();
        STB_LANG_EXPAND_RHS();
        if (STB_LANG_OF_AST(ast->left, type) == AST_VAR){
            STB_LANG_INFER_TYPE(STB_LANG_OF_AST(ast->left, value));
            STB_LANG_REGISTER_VARIABLE(STB_LANG_OF_AST(ast->left, value), ast->typeinfo)
        }
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
    )
    STB_LANG_TYPEINFO_CASE(AST_DECL,
        if (STB_LANG_RHS(ast) != NULL){
            STB_LANG_EXPAND_RHS();
            STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        }
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);
        STB_LANG_VARIABLE(ast);
    )
    STB_LANG_TYPEINFO_CASE(AST_VAR,
        STB_LANG_INFER_TYPE(ast->value);
    )
    STB_LANG_TYPEINFO_CASE(AST_INT,
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_TYPEINFO(.type=AST_TYPE_INT, .ptrnum=0));
    )
    STB_LANG_TYPEINFO_CASE(AST_STRING,
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_TYPEINFO(.type=AST_TYPE_STRING, .ptrnum=0));
    )
    STB_LANG_TYPEINFO_6CASES(AST_LT, AST_LTE, AST_GT, AST_GTE, AST_EQ, AST_NEQ,
        STB_LANG_EXPAND_RHS();
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_RHS(ast)->typeinfo);
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
    )
    STB_LANG_TYPEINFO_5CASES(AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MODULO,
        STB_LANG_EXPAND_LHS();
        STB_LANG_EXPAND_RHS();
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
    )
    STB_LANG_TYPEINFO_5CASES(AST_AND, AST_OR, AST_BAND, AST_BOR, AST_XOR,
        STB_LANG_EXPAND_RHS();
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_RHS(ast)->typeinfo);
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
    )
    STB_LANG_TYPEINFO_CASE(AST_FUNCALL,
        STB_LANG_EXPAND_ARGS();
        STB_LANG_FIND_FUNCTION(checker->root_scope, ast->value, 
            STB_LANG_TYPEINFO_ASSUME_TYPE(symnew.typeinfo);
            STB_LANG_FUNCTION_CHECK_LIST(ast->left);
        )
    )
    STB_LANG_TYPEINFO_CASE(AST_IF,
        STB_LANG_EXPAND_ARGS();
        STB_LANG_EXPAND_BLOCK();
        STB_LANG_EXPAND_LIST(ast->middle);
    )
    STB_LANG_TYPEINFO_CASE(AST_WHILE,
        STB_LANG_EXPAND_ARGS();
        STB_LANG_EXPAND_BLOCK();
    )
    STB_LANG_TYPEINFO_CASE(AST_RET,
        STB_LANG_EXPAND_ARGS();
    )
    STB_LANG_TYPEINFO_CASE(AST_CAST,
        STB_LANG_EXPAND_LHS();
    )
    STB_LANG_TYPEINFO_CASE(AST_REF,
        STB_LANG_EXPAND_LHS();

        Lang_TypeInfo_Typeinfo typinf = STB_LANG_OF_AST(ast->left, typeinfo);
        typinf.ptrnum++;
        ast->typeinfo = typinf;
    )
    STB_LANG_TYPEINFO_CASE(AST_DEREF,
        STB_LANG_EXPAND_LHS();

        Lang_TypeInfo_Typeinfo typinf = STB_LANG_OF_AST(ast->left, typeinfo);
        if (typinf.ptrnum == 0 && typinf.type != AST_TYPE_ARRAY){
            STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "DerefError", "Could not dereference anything that's not a pointer or array");
        }
        typinf.ptrnum--;
        ast->typeinfo = typinf;
    )
    STB_LANG_TYPEINFO_CASE(AST_INDEX,
        STB_LANG_EXPAND_LHS();

        Lang_TypeInfo_Typeinfo typinf = STB_LANG_OF_AST(ast->left, typeinfo);
        ast->typeinfo = typinf;
        if (ast->typeinfo.ptrnum != 0){
            ast->typeinfo.ptrnum--;
        }else if (ast->typeinfo.type == AST_TYPE_ARRAY){
            ast->typeinfo = *(Lang_TypeInfo_Typeinfo*)(ast->typeinfo.data.array.elem_type);
        }else {
            STB_LANG_TYPEINFO_ERROR_MINOR(ast->offset, ast->file, "DerefError", "Could not dereference anything that's not a pointer or array");
        };
    )
    STB_LANG_TYPEINFO_CASE(AST_ACCESS,
        STB_LANG_EXPAND_LHS();
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_LHS(ast)->typeinfo);

        STB_LANG_FIND_DATA(checker->root_scope, STB_LANG_LHS(ast)->typeinfo.data.struct1.name,
            STB_LANG_FIND_DATA(checker->root_scope, STB_LANG_LHS(ast)->typeinfo.data.struct1.name,
                Lang_Parser_AST *structdef = STB_LANG_LHS(STB_LANG_GET_AST(symnew.data.struct1.structdef));
                STB_LANG_ITERATE_LINKED_LIST(structdef, field, Lang_Parser_AST,
                    if (field->type == AST_VAR){
                        if (strcmp(field->value, ast->value) == 0){
                            ast->typeinfo = field->typeinfo;
                        }
                    }
                )
            )

            Lang_TypeInfo_Typeinfo *typeinfo = &(ast->typeinfo);
            STB_LANG_SET_SYMBOL(typeinfo->data.struct1.symbol, symnew);
        )
    )
    STB_LANG_TYPEINFO_CASE(AST_EXPR,
        STB_LANG_EXPAND_LHS();
    )
)



#define CUR_IR_NAME Lang_IR
#define CUR_IR_PREFIX lang_ir

STB_LANG_NEW_IR(
    STB_LANG_IR_OPERANDS(
        IR_INT,
        IR_VAR,
        IR_REG,
        IR_MEM
    ),
    STB_LANG_IR_INSTRS(
        IR_FUNCDEF_BEGIN,
        IR_FUNCDEF_END,
        IR_ASSIGN,
        IR_DECL,
        IR_POP, // pop the stack
        IR_PUSH,
        IR_CALL,
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_MOD,
        IR_LT,
        IR_LTE,
        IR_GT,
        IR_GTE,
        IR_EQ,
        IR_NEQ,
        IR_JUMP_IF_FALSE,
        IR_JUMP,
        IR_LABEL,
        IR_RET,
        IR_CAST,
        IR_BOR,
        IR_BAND,
        IR_AND,
        IR_OR,
        IR_XOR,
        IR_ADDR,
        IR_LOAD,
        IR_STORE
    ),
    STB_LANG_IR_CASES(
        STB_LANG_IR_CASE(AST_FUNCDECL,
            // Skip
        )
        STB_LANG_IR_CASE(AST_FUNCDEF,
            STB_LANG_IR_EMIT(IR_FUNCDEF_BEGIN, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL);
int idx = 0;
int argidx = 0;
STB_LANG_ITERATE_LINKED_LIST(ast->left, _args, Lang_Parser_AST,
    if (_args->flags != STB_LANG_TYPEINFO_VARIADIC) {
        char str[10];snprintf(str, 10, "a%d", idx++);
        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_VAR, _args->value), STB_LANG_IR_OPERAND(IR_REG, strdup(str)), NULL);
    }else {
        char str[10];snprintf(str, 10, ".arg%d", argidx++);
        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_VAR, _args->value), STB_LANG_IR_OPERAND(IR_VAR, _args->value), NULL);
    }
)
            STB_LANG_IR_BLOCK()
            STB_LANG_IR_EMIT(IR_FUNCDEF_END, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL);
        )
        STB_LANG_IR_CASE(AST_ASSIGN,
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), NULL);
        )
        STB_LANG_IR_CASE(AST_STRUCT,
        )

        STB_LANG_IR_CASE(AST_STORE,
            if (STB_LANG_LHS(ast)->type == AST_INDEX){
                STB_LANG_IR_NEW_TEMP(addr_reg);
                STB_LANG_IR_NEW_TEMP(offset_reg);

                char str[32]; snprintf(str, 32, "%d", STB_LANG_LOOKUP_SIZE(ir->root_scope, &STB_LANG_LHS(ast)->typeinfo));
                // a[0] = 5
                STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_OPERAND(IR_REG, offset_reg), STB_LANG_IR_RHS(STB_LANG_LHS(ast)), STB_LANG_IR_OPERAND(IR_INT, strdup(str)));
                STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(STB_LANG_LHS(ast)), NULL);
                STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, offset_reg));
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }else if (STB_LANG_LHS(ast)->type == AST_ACCESS) {
                Lang_TypeInfo_Symbol *symbol = STB_LANG_GET_SYMBOL(STB_LANG_LHS(ast)->typeinfo.data.struct1.symbol);
                Lang_Parser_AST *structdef = STB_LANG_GET_AST(symbol->data.struct1.structdef);
                int offset = 0;
                int found = 0;
                STB_LANG_ITERATE_LINKED_LIST(STB_LANG_LHS(structdef), field, Lang_Parser_AST,
                    if (field->type == AST_VAR){
                        if (strcmp(field->value, STB_LANG_LHS(ast)->value) == 0){
                            ast->typeinfo = field->typeinfo;
                            found = 1;
                            break;
                        }else {
                            offset += STB_LANG_LOOKUP_SIZE(ir->root_scope, &field->typeinfo);
                        }
                    }
                )

                if (found == 0){
                    STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "StructError", "Could not find field \"%s\" in \"struct %s\"", STB_LANG_LHS(ast)->value, symbol->name);
                }
                STB_LANG_IR_NEW_TEMP(addr_reg);
                STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(STB_LANG_LHS(ast)), NULL);
                char str[32]; snprintf(str, 32, "%d", offset);
                STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_INT, strdup(str)));
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }else if (STB_LANG_LHS(ast)->type == AST_DEREF) {
                STB_LANG_IR_NEW_TEMP(secure_addr_reg);

                STB_CONCAT(CUR_IR_NAME, _Operand) *addr = STB_LANG_IR_LHS(STB_LANG_LHS(ast));
                STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), addr, NULL);
                
                STB_CONCAT(CUR_IR_NAME, _Operand) *value = STB_LANG_IR_RHS(ast);
                
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_OPERAND(IR_REG, secure_addr_reg), value, NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }else {
                STB_LANG_IR_EMIT(IR_STORE, STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast), NULL, .typeinfo=STB_LANG_LHS(ast)->typeinfo);
            }
        )

        STB_LANG_IR_CASE(AST_ACCESS,
            STB_LANG_IR_LHS(ast);
            Lang_TypeInfo_Symbol *symbol = STB_LANG_GET_SYMBOL(ast->typeinfo.data.struct1.symbol);
            Lang_Parser_AST *structdef = STB_LANG_GET_AST(symbol->data.struct1.structdef);
            int offset = 0;
            int found = 0;
            STB_LANG_ITERATE_LINKED_LIST(STB_LANG_LHS(structdef), field, Lang_Parser_AST,
                if (field->type == AST_VAR){
                    if (strcmp(field->value, ast->value) == 0){
                        ast->typeinfo = field->typeinfo;
                        found = 1;
                        break;
                    }else {
                        offset += STB_LANG_LOOKUP_SIZE(ir->root_scope, &field->typeinfo);
                    }
                }
            )
            if (found == 0){
                STB_LANG_IR_ERROR_MINOR(ast->offset, ast->file, "StructError", "Could not find field \"%s\" in \"struct %s\"", ast->value, symbol->name);
            }
            STB_LANG_IR_NEW_TEMP(addr_reg);
            STB_LANG_IR_NEW_TEMP(dest_reg);
            STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_LHS(ast), NULL);
            char str[32]; snprintf(str, 32, "%d", offset);
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), STB_LANG_IR_OPERAND(IR_INT, strdup(str)));
            STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_OPERAND(IR_REG, dest_reg), STB_LANG_IR_OPERAND(IR_REG, addr_reg), NULL);
            return STB_LANG_IR_OPERAND(IR_REG, dest_reg);
        )

        STB_LANG_IR_CASE(AST_VAR,
            STB_LANG_IR_RETURN_SELF(IR_VAR);
        )
        STB_LANG_IR_CASE(AST_STRING,
            long offset = STB_CONCAT(CUR_IR_PREFIX, _symbol_new)(ir, ast->value, strlen(ast->value));
            return STB_LANG_IR_OPERAND(IR_MEM, (char*)offset);
        )
        STB_LANG_IR_CASE(AST_DECL,
            if (STB_LANG_RHS(ast) != NULL) {
                STB_LANG_IR_EMIT(IR_DECL, STB_LANG_IR_OPERAND(IR_VAR, ast->value), STB_LANG_IR_RHS(ast), NULL);
            }
        )
        STB_LANG_IR_CASE(AST_INT,
            STB_LANG_IR_RETURN_SELF(IR_INT);
        )
        STB_LANG_IR_CASE(AST_FUNCALL,
            Lang_Parser_AST *params[64]; int paramslen = 0;
            STB_LANG_ITERATE_LINKED_LIST(ast->left, arg, Lang_Parser_AST,
                params[paramslen++] = arg;
            )
            int idx = -1;
            int varidx = -1;
            for (int i = 0; i < paramslen; i++) {
                if (params[i]->flags != STB_LANG_TYPEINFO_VARIADIC) {
                    idx++;
                } else {
                    varidx++;
                }
            }

            for (int i = paramslen - 1; i >= 0; i--) {
                Lang_Parser_AST *param = params[i];

                STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, param);
                if (param->flags != STB_LANG_TYPEINFO_VARIADIC) {
                    char str[32];
                    snprintf(str, 32, "a%d", idx--); 
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, strdup(str)), operand, NULL);
                } else {
                    char str[32];
                    snprintf(str, 32, ".arg%d", varidx--);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, strdup(str)), operand, NULL);
                }
            }
            STB_LANG_IR_EMIT(IR_CALL, STB_LANG_IR_OPERAND(IR_VAR, ast->value), NULL, NULL);
            return STB_LANG_IR_AS_TEMP(IR_REG, "v0")
        )
        STB_LANG_IR_CASE(AST_ADD,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_SUB,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_SUB, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_MUL,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_DIV,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_DIV, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_MODULO,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_MOD, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_BOR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_BOR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_BAND,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_BAND, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_AND,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_AND, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_OR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_OR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_XOR,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_XOR, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_LT,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_LT, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_LTE,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_LTE, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_GT,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_GT, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_GTE,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_GTE, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_EQ,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_EQ, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_NEQ,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_NEQ, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(ast), STB_LANG_IR_RHS(ast));
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_IF,
            STB_LANG_IR_NEW_LABEL(label)
            STB_LANG_IR_NEW_LABEL(end)

            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            char *temp = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, temp), operand, NULL);

            STB_LANG_IR_EMIT(IR_JUMP_IF_FALSE, STB_LANG_IR_LABEL(IR_VAR, label), operand, STB_LANG_IR_OPERAND(IR_REG, temp));
            STB_LANG_IR_BLOCK()
            STB_LANG_IR_EMIT(IR_JUMP, STB_LANG_IR_LABEL(IR_VAR, end), NULL, NULL);
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label), operand, NULL);
            STB_LANG_IR_RUN(ast->middle);
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, end), operand, NULL);
        )
        STB_LANG_IR_CASE(AST_RET,
            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            STB_LANG_IR_EMIT(IR_RET, NULL, operand, NULL);
        )
        STB_LANG_IR_CASE(AST_CAST,
            return STB_LANG_IR_GET_OPERAND(ast->left);
        )
        STB_LANG_IR_CASE(AST_EXPR,
            return STB_LANG_IR_GET_OPERAND(ast->left);
        )
        STB_LANG_IR_CASE(AST_REF,
            STB_LANG_IR_NEW_TEMP(dest);
            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_AS_TEMP(IR_REG, dest), operand, NULL);
            return STB_LANG_IR_AS_TEMP(IR_REG, dest);
        )
        STB_LANG_IR_CASE(AST_DEREF,
            STB_LANG_IR_NEW_TEMP(dest);
            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_AS_TEMP(IR_REG, dest), operand, NULL);
            return STB_LANG_IR_AS_TEMP(IR_REG, dest);
        )
        STB_LANG_IR_CASE(AST_INDEX,
            STB_LANG_IR_NEW_TEMP(dest);
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_NEW_TEMP(temp2);
            char str[32]; 
            int typ = STB_LANG_LHS(ast)->typeinfo.type;
            if (typ == AST_TYPE_ARRAY) {
                snprintf(str, 32, "%d", STB_LANG_LOOKUP_SIZE(ir->root_scope, STB_LANG_LHS(ast)->typeinfo.data.array.elem_type));
            } else {
                snprintf(str, 32, "%d", STB_LANG_LOOKUP_SIZE(ir->root_scope, &ast->typeinfo));
            }
            STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_OPERAND(IR_REG, temp_reg), STB_LANG_IR_RHS(ast), STB_LANG_IR_OPERAND(IR_INT, strdup(str)));
            if (STB_LANG_LHS(ast)->typeinfo.type == AST_TYPE_ARRAY) {
                STB_LANG_IR_EMIT(IR_ADDR, STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_LHS(ast), NULL);
            }else {
                STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_LHS(ast), NULL);
            }
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_OPERAND(IR_REG, temp2), STB_LANG_IR_OPERAND(IR_REG, temp_reg));
            STB_LANG_IR_EMIT(IR_LOAD, STB_LANG_IR_AS_TEMP(IR_REG, dest), STB_LANG_IR_OPERAND(IR_REG, temp2), NULL);
            return STB_LANG_IR_AS_TEMP(IR_REG, dest);
        )
        STB_LANG_IR_CASE(AST_WHILE,
            STB_LANG_IR_NEW_LABEL(label)
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label), NULL, NULL);

            STB_LANG_IR_NEW_LABEL(label1)

            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS(ast);
            char *temp = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND(IR_REG, temp), operand, NULL);
            STB_LANG_IR_EMIT(IR_JUMP_IF_FALSE, STB_LANG_IR_LABEL(IR_VAR, label1), operand, STB_LANG_IR_OPERAND(IR_REG, temp));


            STB_LANG_IR_BLOCK()


            STB_LANG_IR_EMIT(IR_JUMP, STB_LANG_IR_LABEL(IR_VAR, label), operand, NULL);

            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label1), NULL, NULL);
        )
    )
);

#define CUR_REGALLOC_NAME Lang_RegAlloc
#define CUR_REGALLOC_PREFIX lang_regalloc
STB_LANG_NEW_REGALLOC(
    STB_LANG_ARM_REGISTERS(),
    STB_LANG_ARM_REGISTER_MAPS(),
    STB_LANG_REGALLOC_LIST(
        STB_LANG_REGALLOC_CASE(IR_FUNCDEF_BEGIN,
        )
        STB_LANG_REGALLOC_CASE(IR_FUNCDEF_END,
        )
        STB_LANG_REGALLOC_2CASES(IR_ASSIGN, IR_DECL,
            STB_LANG_SAVE_REG(phys[0]);
        )
        STB_LANG_REGALLOC_CASE(IR_STORE,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
            STB_LANG_EXPAND_OPERAND(right, IR_REG)
        )
        STB_LANG_REGALLOC_CASE(IR_JUMP_IF_FALSE,
            STB_LANG_EXPAND_OPERAND(right, IR_REG)
        )
        STB_LANG_REGALLOC_CASE(IR_LABEL,
        )

        STB_LANG_REGALLOC_CASE(IR_PUSH,
            STB_LANG_SAVE_REG(phys[0]);
        )
        STB_LANG_REGALLOC_CASE(IR_POP,
        )

        STB_LANG_REGALLOC_2CASES(IR_ADD, IR_SUB,
            STB_CONCAT(CUR_REGALLOC_NAME, _Reg) right;
            STB_LANG_SAVE_REG(phys[0], {
                if (instr->right->type != IR_INT){
                    STB_LANG_SAVE_REG(phys[1]);
                }else {
                    STB_LANG_REGALLOC_NEGATE(phys[1]);
                };
            });
        )
        STB_LANG_REGALLOC_6CASES(IR_MUL, IR_DIV, IR_BOR, IR_BAND, IR_AND, IR_OR,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
        )
        STB_LANG_REGALLOC_CASE(IR_XOR,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
        )
        STB_LANG_REGALLOC_CASE(IR_ADDR,
            STB_LANG_EXPAND_OPERAND(dest, IR_REG)
        )
        STB_LANG_REGALLOC_CASE(IR_LOAD,
            STB_LANG_SAVE_REG(phys[0]);
            STB_LANG_EXPAND_OPERAND(dest, IR_REG)
            STB_LANG_EXPAND_OPERAND(left, IR_REG)
            STB_LANG_EXPAND_OPERAND(right, IR_REG)
        )
        STB_LANG_REGALLOC_CASE(IR_MOD,
            STB_CONCAT(CUR_REGALLOC_NAME, _Reg) right;
            STB_LANG_SAVE_REG(phys[1], {
                STB_LANG_SAVE_REG(phys[0], {
                    STB_LANG_SAVE_REG(phys[2]); // Extra register for msub temp
                });
            });
        )
        STB_LANG_REGALLOC_6CASES(IR_LT, IR_LTE, IR_GT, IR_GTE, IR_EQ, IR_NEQ,
            STB_CONCAT(CUR_REGALLOC_NAME, _Reg) right;
            STB_LANG_SAVE_REG(phys[0], {
                if (instr->right->type != IR_INT){
                    STB_LANG_SAVE_REG(phys[1]);
                }else {
                    STB_LANG_REGALLOC_NEGATE(phys[1]);
                };
            });
        )

        STB_LANG_REGALLOC_CASE(IR_CALL,
        )

        STB_LANG_REGALLOC_CASE(IR_RET,
            STB_LANG_EXPAND_OPERAND(left, IR_REG)
        )

        STB_LANG_REGALLOC_CASE(IR_JUMP,
        )
    ),
    IR_REG
)

#define STB_LANG_ARM_MOVE(size, right, ...) {\
if (right != NULL) { \
    char *leftr = __VA_ARGS__; \
    if (leftr[0] == 't'){ \
        leftr = STB_LANG_REGISTER(instr->dest->phys, 8); \
    } \
    if (right->type == IR_INT) { \
        STB_LANG_EMIT_CODE("\tmov %s, #%s\n", leftr, right->value); \
    }else if(right->type == IR_VAR){ \
        int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, right->value); \
        if (newoffset == 0){ \
            STB_LANG_EMIT_CODE("\tldr %s, [sp]\n", leftr); \
        }else { \
            STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, newoffset); \
        } \
    }else if (right->type == IR_REG){ \
        if (right->value[0] == 'a'){ \
            STB_LANG_EMIT_CODE("\tmov %s, x%d\n", leftr, atoi(right->value + 1)); \
        }else if (right->value[0] == '.'){ \
            int n = atoi(instr->dest->value + 4); \
            STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, n * 8); \
        }else if (right->value[0] == 'v'){ \
            STB_LANG_EMIT_CODE("\tmov %s, x0\n", leftr); \
        }else { \
            STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, STB_LANG_REGISTER(right->phys, size)); \
        } \
    }else if (right->type == IR_MEM){ \
        STB_LANG_EMIT_CODE("\tadrp %s, mem_%ld@PAGE\n", leftr, (long)right->value); \
        STB_LANG_EMIT_CODE("\tadd %s, %s, mem_%ld@PAGEOFF\n", leftr, leftr, (long)right->value); \
    } \
} \
}



#define STB_LANG_ARM_BINARY(op) \
STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], 8)); \
if (instr->phys[1] != -1){ \
    STB_LANG_ARM_MOVE(8, instr->right, STB_LANG_REGISTER(instr->phys[1], 8)); \
    STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys[0], 8), STB_LANG_REGISTER(instr->phys[1], 8)); \
}else { \
    STB_LANG_EMIT_CODE("\t%s %s, %s, #%s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys[0], 8), instr->right->value); \
}


#define STB_LANG_ARM_COMPARISON(op) \
STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], 8)); \
if (instr->phys[1] != -1){ \
    STB_LANG_ARM_MOVE(8, instr->right, STB_LANG_REGISTER(instr->phys[1], 8)); \
    STB_LANG_EMIT_CODE("\tcmp %s, %s\n", STB_LANG_REGISTER(instr->phys[0], 8), STB_LANG_REGISTER(instr->phys[1], 8)); \
}else { \
    STB_LANG_EMIT_CODE("\tcmp %s, #%s\n", STB_LANG_REGISTER(instr->phys[0], 8), instr->right->value); \
} \
STB_LANG_EMIT_CODE("\tcset %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), op);


#define CUR_CODEGEN_NAME Lang_CodeGen
#define CUR_CODEGEN_PREFIX lang_codegen
STB_LANG_NEW_CODEGEN(
    STB_LANG_CODEGEN_PREFIX(
        STB_LANG_EMIT_CODE("%s\n", ".global _main");
        STB_LANG_EMIT_CODE("%s\n", ".align 2");
    ),
    STB_LANG_CODEGEN_SUFFIX(
        STB_LANG_EMIT_CODE(".data\n");
        STB_LANG_EMIT_CODE(".align 3\n");
        STB_LANG_ITERATE(gen->symbols, Lang_IR_Symbol, 
// This looks wrong, but there are variables idx and iter used in the iterate function
// This is not meant to be used that seriously, so take it with a grain of salt.
            STB_LANG_EMIT_CODE("mem_%d: .asciz \"%s\\0\"\n", idx, iter.data);
        )
    ),
    STB_LANG_CODEGEN_LIST(
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_BEGIN,
            STB_LANG_EMIT_CODE("_%s:\n", instr->dest->value);
            STB_LANG_EMIT_CODE("\tstp x29, x30, [sp, #-16]!\n");
            STB_LANG_EMIT_CODE("\tmov x29, sp\n");

            // STB_LANG_FUNCTION_ALLOCATE(gen, 8);
            int offset = 16;
            STB_LANG_GO_TO_FUNC(instr->dest->value);
            STB_LANG_ITERATE(STB_LANG_CURRENT_SCOPE()->symbols, Lang_TypeInfo_Symbol,
                if (iter.kind == STB_LANG_SYMBOL_VARIABLE){
                    offset = iter.data.variable.offset;
                };
            );
            STB_LANG_EMIT_CODE("\tsub sp, sp, #%d\n", (offset + 15 + 16) & ~15);
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_END,
            STB_LANG_EMIT_CODE("\tmov sp, x29\n");
            STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n");
            STB_LANG_EMIT_CODE("\tret\n");
        )
        STB_LANG_CODEGEN_CASE(IR_RET,
            STB_LANG_ARM_MOVE(8, instr->left, "x0")

            STB_LANG_EMIT_CODE("\tmov sp, x29\n");
            STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n");
            STB_LANG_EMIT_CODE("\tret\n");
        )
        STB_LANG_CODEGEN_CASE(IR_LABEL,
            STB_LANG_EMIT_CODE(".L_label_%s:\n", instr->dest->value)
        )
        STB_LANG_CODEGEN_CASE(IR_ADDR,
            char *dest = STB_LANG_REGISTER(instr->dest->phys, 8);
            if (instr->left->type == IR_VAR){
                int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->left->value);
                if (newoffset == 0) {
                    STB_LANG_EMIT_CODE("\tmov %s, sp\n", dest);
                } else {
                    STB_LANG_EMIT_CODE("\tadd %s, sp, #%d\n", dest, newoffset);
                }
            }
        )
        STB_LANG_CODEGEN_CASE(IR_LOAD,
            char *dest = STB_LANG_REGISTER(instr->dest->phys, 8);
            char *tmp = STB_LANG_REGISTER(instr->phys[0], 8);
            STB_LANG_ARM_MOVE(8, instr->left, tmp);
            
            int val_size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->left->value);
            
            if (val_size == 1) {
                STB_LANG_EMIT_CODE("\tldrb %s, [%s]\n", dest, tmp);
            } else {
                STB_LANG_EMIT_CODE("\tldr %s, [%s]\n", dest, tmp);
            }
        )
        STB_LANG_CODEGEN_CASE(IR_STORE,
            int size = STB_LANG_LOOKUP_SIZE(gen->root_scope, &instr->typeinfo);

            char *val_reg = STB_LANG_REGISTER(instr->phys[0], size);
            STB_LANG_ARM_MOVE(size, instr->left, val_reg);

            char *addr_reg = STB_LANG_REGISTER(instr->phys[1], 8);
            STB_LANG_ARM_MOVE(8, instr->dest, addr_reg);


            if (size == 1) {
                STB_LANG_EMIT_CODE("\tstrb %s, [%s]\n", val_reg, addr_reg);
            }else {
                STB_LANG_EMIT_CODE("\tstr %s, [%s]\n", val_reg, addr_reg);
            }
        )
        STB_LANG_CODEGEN_2CASES(IR_ASSIGN, IR_DECL,
            // TODO: merge this entirely with STB_LANG_ARM_MOVE
            ;if (instr->dest->type == IR_VAR){
                int size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->dest->value);
                int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value);

                if (instr->left != NULL){
                    STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], size))
                }
                if (offset == 0){
                    STB_LANG_EMIT_CODE("\tstr %s, [sp]\n", STB_LANG_REGISTER(instr->phys[0], size));
                }else {
                    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys[0], size), offset);
                }
            }else if (instr->dest->type == IR_REG){
                char string[32];
                if (instr->dest->value[0] == 'a'){
                    snprintf(string, 32, "x%d", atoi(instr->dest->value + 1));
                    STB_LANG_ARM_MOVE(8, instr->left, string)
                }else if (instr->dest->value[0] == 't'){
                    snprintf(string, 32, "%s", instr->dest->value);
                    STB_LANG_ARM_MOVE(8, instr->left, string)
                }else if (instr->dest->value[0] == '.'){
                    int n = atoi(instr->dest->value + 4);
                    STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], 8))
                    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys[0], 8), n*8);
                }
            };
        )
        STB_LANG_CODEGEN_CASE(IR_PUSH,
            STB_LANG_ARM_MOVE(8, instr->left, STB_LANG_REGISTER(instr->phys[0], 8));
            STB_LANG_EMIT_CODE("\tstr %s, [sp, #-16]!\n", STB_LANG_REGISTER(instr->phys[0], 8));
        )
        STB_LANG_CODEGEN_CASE(IR_POP,
            STB_LANG_EMIT_CODE("\tldr %s, [sp], #16\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_JUMP_IF_FALSE,
            STB_LANG_EMIT_CODE("\tcbz %s, .L_label_%s\n", 
                STB_LANG_REGISTER(instr->right->phys, 8), 
                instr->dest->value
            );
        )
        STB_LANG_CODEGEN_CASE(IR_CALL,
            STB_LANG_EMIT_CODE("\tbl _%s\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_JUMP,
            STB_LANG_EMIT_CODE("\tb .L_label_%s\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_ADD,
            STB_LANG_ARM_BINARY("add")
        )
        STB_LANG_CODEGEN_CASE(IR_SUB,
            STB_LANG_ARM_BINARY("sub")
        )
        STB_LANG_CODEGEN_CASE(IR_MUL,
            STB_LANG_ARM_BINARY("mul")
        )
        STB_LANG_CODEGEN_CASE(IR_DIV,
            STB_LANG_ARM_BINARY("sdiv")
        )
        STB_LANG_CODEGEN_CASE(IR_MOD,
            char *left = STB_LANG_REGISTER(instr->phys[0], 8);
            char *right = STB_LANG_REGISTER(instr->phys[1], 8);
            char *tmp = STB_LANG_REGISTER(instr->phys[2], 8);


            STB_LANG_ARM_MOVE(8, instr->left, left);
            STB_LANG_ARM_MOVE(8, instr->right, right);

            ;
            STB_LANG_EMIT_CODE("\tsdiv %s, %s, %s\n", tmp, left, right);
            {
                STB_LANG_EMIT_CODE("\tmsub %s, %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), tmp, right, left);
            }
        )
        STB_LANG_CODEGEN_CASE(IR_BOR,
            STB_LANG_ARM_BINARY("orr")
        )
        STB_LANG_CODEGEN_CASE(IR_BAND,
            STB_LANG_ARM_BINARY("and")
        )
        STB_LANG_CODEGEN_CASE(IR_XOR,
            STB_LANG_ARM_BINARY("eor")
        )
        STB_LANG_CODEGEN_CASE(IR_AND,
            char *left = STB_LANG_REGISTER(instr->phys[0], 8);
            char *right = STB_LANG_REGISTER(instr->phys[1], 8);
            STB_LANG_ARM_MOVE(8, instr->left, left);
            STB_LANG_ARM_MOVE(8, instr->right, right);

            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", left);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", left);


            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", right);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", right);

            STB_LANG_EMIT_CODE("\tand %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), left, right);
        )
        STB_LANG_CODEGEN_CASE(IR_OR,
            char *left = STB_LANG_REGISTER(instr->phys[0], 8);
            char *right = STB_LANG_REGISTER(instr->phys[1], 8);
            STB_LANG_ARM_MOVE(8, instr->left, left);
            STB_LANG_ARM_MOVE(8, instr->right, right);

            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", left);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", left);


            STB_LANG_EMIT_CODE("\tcmp %s, #0\n", right);
            STB_LANG_EMIT_CODE("\tcset %s, ne\n", right);

            STB_LANG_EMIT_CODE("\torr %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), left, right);
        )
        STB_LANG_CODEGEN_CASE(IR_LT,
            STB_LANG_ARM_COMPARISON("lt")
        )
        STB_LANG_CODEGEN_CASE(IR_LTE,
            STB_LANG_ARM_COMPARISON("le")
        )
        STB_LANG_CODEGEN_CASE(IR_GT,
            STB_LANG_ARM_COMPARISON("gt")
        )
        STB_LANG_CODEGEN_CASE(IR_GTE,
            STB_LANG_ARM_COMPARISON("ge")
        )
        STB_LANG_CODEGEN_CASE(IR_EQ,
            STB_LANG_ARM_COMPARISON("eq")
        )
        STB_LANG_CODEGEN_CASE(IR_NEQ,
            STB_LANG_ARM_COMPARISON("ne")
        )
    )
);

#define CUR_DRIVER_PREFIX lang_driver

STB_LANG_NEW_DRIVER(
    char *asm_path = "res/main.s";
    char *obj_path = "res/main.o";
    char *exec_path = "res/main.out";
    STB_LANG_DRIVER_WRITE_DATA(asm_path);
    STB_LANG_DRIVER_RUN_SCRIPT("clang -c %s -o %s", asm_path, obj_path);
    STB_LANG_DRIVER_RUN_SCRIPT("clang -O0 -fno-stack-protector -fno-common -fno-exceptions -arch arm64 %s -o %s -e _main -Wl,-w -Wl,-platform_version,macos,11.0,11.0 -lc", obj_path, exec_path);
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
    Lang_Tokenizer *tokenizer = lang_tokenizer_init(input_file);
    while (lang_tokenizer_token(tokenizer) == 0){
    }
    // int len = tokenizer->tokens.datalen;
    // for (int i=0; i<len; i++){
    //     printf("%d, %s\n", tokenizer->tokens.data[i].type, tokenizer->tokens.data[i].value);
    // }
    
    Lang_Preprocessor *processor = lang_preprocessor_init(tokenizer, 0);
    while (lang_preprocessor_token(processor) == 0){
    }


    Lang_Parser *parser = lang_parser_init(processor);
    while (lang_parser_parse_body(parser) == 0){
    }



    Lang_TypeInfo *checker = lang_typeinfo_init(parser);
    while (lang_typeinfo_check(checker) == 0){
    }


    Lang_IR *ir = lang_ir_init(checker);
    while (lang_ir_translate(ir) == 0){
    }

    Lang_RegAlloc *regalloc = lang_regalloc_init(ir);
    lang_regalloc_backtrace(regalloc);
    while (lang_regalloc_alloc(regalloc) == 0){
    }


    Lang_CodeGen *gen = lang_codegen_init(regalloc);
    while (lang_codegen_ir(gen) == 0){
    }

    STB_LANG_INVOKE_DRIVER(gen);
    // printf("-------- ASSEMBLY CODE --------\n");
    // printf("%s", gen->code.data);
    // printf("-------------------------------\n");
    return 0;
}
