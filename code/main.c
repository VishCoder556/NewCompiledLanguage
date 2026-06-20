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
        TOKEN_NOT
    ),
    STB_LANG_SIMPLE_CASES(
        STB_LANG_TOKEN_CHAR('(', TOKEN_LP)
        STB_LANG_TOKEN_CHAR(')', TOKEN_RP)
        STB_LANG_TOKEN_CHAR('{', TOKEN_LB)
        STB_LANG_TOKEN_CHAR('}', TOKEN_RB)
        STB_LANG_TOKEN_CHAR(',', TOKEN_COMMA)
        STB_LANG_TOKEN_CHAR('+', TOKEN_ADD)
        STB_LANG_TOKEN_CHAR('-', TOKEN_SUB)
        STB_LANG_TOKEN_CHAR('*', TOKEN_MUL)
        STB_LANG_TOKEN_CHAR('/', TOKEN_DIV)
        STB_LANG_TOKEN_CHAR('%', TOKEN_MODULO)
        STB_LANG_SKIP('\n')
    ),
    STB_LANG_ALPHA(TOKEN_ID)
    STB_LANG_NUM(TOKEN_NUM)
    STB_LANG_STRING('"', TOKEN_STRING)
    STB_LANG_TOKEN_DOUBLE_CHAR(">=", TOKEN_GTE, TOKEN_GT)
    STB_LANG_TOKEN_DOUBLE_CHAR("<=", TOKEN_LTE, TOKEN_LT)
    STB_LANG_TOKEN_DOUBLE_CHAR("==", TOKEN_DEQ, TOKEN_EQ)
    STB_LANG_TOKEN_DOUBLE_CHAR("!=", TOKEN_NEQ, TOKEN_NOT)
    STB_LANG_SKIP(' ')
)



#define CUR_TYPEINFO_NAME Lang_TypeInfo
#define CUR_TYPEINFO_PREFIX lang_typeinfo
STB_LANG_DEFINE_TYPEINFO(
    AST_TYPE_VOID,
    AST_TYPE_INT,
    AST_TYPE_STRING
)
STB_LANG_TYPEINFO(
    STB_LANG_MATCH_TYPEINFO("void", AST_TYPE_VOID)
    STB_LANG_MATCH_TYPEINFO("int", AST_TYPE_INT)
    STB_LANG_MATCH_TYPEINFO("string", AST_TYPE_STRING)
)
STB_LANG_TYPEINFO_SIZE(
    STB_LANG_MATCH_TYPEINFO_SIZE(AST_TYPE_VOID, 0)
    STB_LANG_MATCH_TYPEINFO_SIZE(AST_TYPE_INT, 8)
    STB_LANG_MATCH_TYPEINFO_SIZE(AST_TYPE_STRING, 8)
)




#define CUR_PARSER_NAME Lang_Parser
#define CUR_PARSER_PREFIX lang_parser

STB_LANG_NEW_PARSER(
STB_LANG_BINDING_POWER(
    STB_LANG_MATCH_BINDING_POWER(TOKEN_LT, 5)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_LTE, 5)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_GT, 5)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_GTE, 5)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DEQ, 5)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_NEQ, 5)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_ADD, 10)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_SUB, 10)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_MUL, 20)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_DIV, 20)
    STB_LANG_MATCH_BINDING_POWER(TOKEN_MODULO, 20)
),
STB_LANG_ASTS(
    AST_FUNCDEF,
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
    AST_STRING
),
STB_LANG_PARSE_BODY(
    STB_LANG_MATCH_TOKEN(TOKEN_ID, 
        STB_LANG_PARSE_TYPEINFO(typeinfo, match_token){
            STB_LANG_IF_TOKEN(TOKEN_ID, // Function name
                STB_LANG_PARSER_ADVANCE();
                STB_LANG_SAVE(func_name, match_token);
                STB_LANG_PARSE_ARGUMENT_LIST(params, TOKEN_LP, TOKEN_COMMA, TOKEN_RP, AST_VAR)
                STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
                Lang_Parser_AST *ast = (Lang_Parser_AST*)GetLinkedListHead(params, Lang_Parser_AST);
                return STB_LANG_AST(.type=AST_FUNCDEF, .typeinfo=typeinfo, .value=func_name.value, .left=STB_LANG_LINKED_LIST(params), .right=STB_LANG_LINKED_LIST(stmnts));
            )
        }
    )
),
STB_LANG_PARSE_AST(
    // No ASTS yet!
    STB_LANG_IF_VALUE(TOKEN_ID, "if", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_EXPECT_TOKEN(TOKEN_LP)
        STB_LANG_OPERAND(ifexpr, lang_parser_parse_expr(parser, 0));
        STB_LANG_EXPECT_TOKEN(TOKEN_RP)
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
        return STB_LANG_AST(.type=AST_IF, .value=NULL, .left=STB_LANG_AS_AST(ifexpr), .right=STB_LANG_LINKED_LIST(stmnts));
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "while", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_EXPECT_TOKEN(TOKEN_LP)
        STB_LANG_OPERAND(ifexpr, lang_parser_parse_expr(parser, 0));
        STB_LANG_EXPECT_TOKEN(TOKEN_RP)
        STB_LANG_PARSE_STATEMENT_LIST(stmnts, TOKEN_LB, -1, TOKEN_RB)
        return STB_LANG_AST(.type=AST_WHILE, .value=NULL, .left=STB_LANG_AS_AST(ifexpr), .right=STB_LANG_LINKED_LIST(stmnts));
    )
    STB_LANG_IF_VALUE(TOKEN_ID, "return", 
        STB_LANG_PARSER_ADVANCE();
        STB_LANG_OPERAND(expr, lang_parser_parse_expr(parser, 0));
        return STB_LANG_AST(.type=AST_RET, .value=NULL, .left=STB_LANG_AS_AST(expr), .right=NULL);
    )
    STB_LANG_PARSE_TYPEINFO(typeinfo, token){
        STB_LANG_PARSER_ADVANCE()
    maybe_assign:
        STB_LANG_IF_TOKEN(TOKEN_ID, 
            STB_LANG_PARSER_ADVANCE()
            STB_LANG_SAVE(varia, match_token)
            STB_LANG_IF_TOKEN(TOKEN_EQ,
                STB_LANG_PARSER_ADVANCE();
                STB_LANG_OPERAND(assign, lang_parser_parse_expr(parser, 0));
                int type = AST_DECL;
                if (typeinfo == -1) {
                    type = AST_ASSIGN;
                }
                return STB_LANG_AST(.type=type, .typeinfo=typeinfo, .value = varia.value, .left=STB_LANG_AS_AST(assign), .right=STB_LANG_AS_AST(assign));
            ) STB_LANG_ELSE_IF_TOKEN(TOKEN_LP,
                STB_LANG_PARSE_EXPR_LIST(args, TOKEN_LP, TOKEN_COMMA, TOKEN_RP);
                return STB_LANG_AST_FUNCALL(AST_FUNCALL, varia, args)
            )
        )
    }else {
        goto maybe_assign;
    }
),
STB_LANG_PARSE_EXPR(
    STB_LANG_MATCH_TOKEN(TOKEN_ID,  
        STB_LANG_SAVE(name, match_token)
        STB_LANG_IF_TOKEN(TOKEN_LP,
            STB_LANG_PARSE_EXPR_LIST(args, TOKEN_LP, TOKEN_COMMA, TOKEN_RP);
            return STB_LANG_AST_FUNCALL(AST_FUNCALL, name, args)
        ) STB_LANG_ELSE(
            left = STB_LANG_AST_LITERAL(AST_VAR, name);
        )
    )
    STB_LANG_MATCH_TOKEN(TOKEN_NUM,  
        left = STB_LANG_AST_LITERAL(AST_INT, match_token);
    )
    STB_LANG_MATCH_TOKEN(TOKEN_STRING,  
        left = STB_LANG_AST_LITERAL(AST_STRING, match_token);
    )
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
    )
)
)

STB_LANG_NEW_TYPEINFO(
    STB_LANG_TYPEINFO_CASE(AST_FUNCDEF, 
        STB_LANG_ADD_FUNCTION(ast->value, ast->typeinfo,
            STB_LANG_FUNCTION_ADD_PARAMS(STB_LANG_GET_AST(ast->left));
        )
        STB_LANG_MAKE_SCOPE(ast->value);
        STB_LANG_EXPAND_PARAMS();
        STB_LANG_EXPAND_BLOCK();
    )
    STB_LANG_TYPEINFO_CASE(AST_ASSIGN, 
        STB_LANG_EXPAND_RHS();
        STB_LANG_INFER_TYPE(ast->value);
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        STB_LANG_VARIABLE(ast);
    )
    STB_LANG_TYPEINFO_CASE(AST_DECL,
        STB_LANG_EXPAND_RHS();
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
        STB_LANG_INFER_TYPE(ast->value);
        STB_LANG_VARIABLE(ast);
    )
    STB_LANG_TYPEINFO_CASE(AST_VAR,
        STB_LANG_INFER_TYPE(ast->value);
    )
    STB_LANG_TYPEINFO_CASE(AST_INT,
        STB_LANG_TYPEINFO_ASSUME_TYPE(AST_TYPE_INT);
    )
    STB_LANG_TYPEINFO_CASE(AST_STRING,
        STB_LANG_TYPEINFO_ASSUME_TYPE(AST_TYPE_STRING);
    )
    STB_LANG_TYPEINFO_6CASES(AST_LT, AST_LTE, AST_GT, AST_GTE, AST_EQ, AST_NEQ,
        STB_LANG_EXPAND_RHS();
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_RHS(ast)->typeinfo);
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
    )
    STB_LANG_TYPEINFO_5CASES(AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MODULO,
        STB_LANG_EXPAND_RHS();
        STB_LANG_TYPEINFO_ASSUME_TYPE(STB_LANG_RHS(ast)->typeinfo);
        STB_LANG_EXPECT_TYPE_EQ(ast, STB_LANG_RHS(ast));
    )
    STB_LANG_TYPEINFO_CASE(AST_FUNCALL,
        STB_LANG_EXPAND_ARGS();
        STB_LANG_FIND_FUNCTION(ast->value, 
            STB_LANG_TYPEINFO_ASSUME_TYPE(function.typeinfo);
            STB_LANG_FUNCTION_CHECK_LIST(ast->left);
        )
    )
    STB_LANG_TYPEINFO_2CASES(AST_IF, AST_WHILE,
        STB_LANG_EXPAND_ARGS();
        STB_LANG_EXPAND_BLOCK();
    )
    STB_LANG_TYPEINFO_CASE(AST_RET,
        STB_LANG_EXPAND_ARGS();
    )
)
// STB_CONCAT(CUR_IR_PREFIX, _symbol_new)(CUR_IR_NAME *ir, char *data, int length)

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
        IR_RET
    ),
    STB_LANG_IR_CASES(
        STB_LANG_IR_CASE(AST_FUNCDEF,
            STB_LANG_IR_EMIT(IR_FUNCDEF_BEGIN, STB_LANG_IR_OPERAND_NAME(IR_VAR, ast->value), NULL, NULL);
int idx = 0;
int argidx = 0;
STB_LANG_ITERATE_LINKED_LIST(_args, Lang_Parser_AST,
    if (_args->flags != STB_LANG_AST_TYPE_VARIADIC) {
        char str[10];snprintf(str, 10, "a%d", idx++);
        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_VAR, _args->value), STB_LANG_IR_OPERAND_NAME(IR_REG, strdup(str)), NULL);
    }else {
        char str[10];snprintf(str, 10, ".arg%d", argidx++);
        STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_VAR, _args->value), STB_LANG_IR_OPERAND_NAME(IR_VAR, _args->value), NULL);
    }
)
            STB_LANG_IR_BLOCK()
            STB_LANG_IR_EMIT(IR_FUNCDEF_END, STB_LANG_IR_OPERAND_NAME(IR_VAR, ast->value), NULL, NULL);
        )
        STB_LANG_IR_CASE(AST_ASSIGN,
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_VAR, ast->value), STB_LANG_IR_RHS(), NULL);
        )
        STB_LANG_IR_CASE(AST_VAR,
            STB_LANG_IR_RETURN_SELF(IR_VAR);
        )
        STB_LANG_IR_CASE(AST_STRING,
            long offset = STB_CONCAT(CUR_IR_PREFIX, _symbol_new)(ir, ast->value, strlen(ast->value));
            return STB_LANG_IR_OPERAND_NAME(IR_MEM, (char*)offset);
        )
        STB_LANG_IR_CASE(AST_DECL,
            STB_LANG_IR_EMIT(IR_DECL, STB_LANG_IR_OPERAND_NAME(IR_VAR, ast->value), STB_LANG_IR_RHS(), NULL);
        )
        STB_LANG_IR_CASE(AST_INT,
            STB_LANG_IR_RETURN_SELF(IR_INT);
        )
        STB_LANG_IR_CASE(AST_FUNCALL,

            int idx = 0;
            int argidx = 0;
            STB_LANG_ITERATE_LINKED_LIST(_args, Lang_Parser_AST,
                STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_CONCAT(CUR_IR_PREFIX, _ast)(ir, _args);
                if (_args->flags != STB_LANG_AST_TYPE_VARIADIC) {
                    char str[32];snprintf(str, 32, "a%d", idx++);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_REG, strdup(str)), operand, NULL);
                }else {
                    char str[32];snprintf(str, 32, ".arg%d", argidx++);
                    STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_REG, strdup(str)), operand, NULL);
                }
            )

            STB_LANG_IR_EMIT(IR_CALL, STB_LANG_IR_OPERAND_NAME(IR_VAR, ast->value), NULL, NULL);
        )
        STB_LANG_IR_CASE(AST_ADD,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_ADD, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS());
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_SUB,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_SUB, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_MUL,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_MUL, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_DIV,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_DIV, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_MODULO,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_MOD, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_LT,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_LT, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_LTE,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_LTE, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_GT,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_GT, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_GTE,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_GTE, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_EQ,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_EQ, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_NEQ,
            STB_LANG_IR_NEW_TEMP(temp_reg);
            STB_LANG_IR_EMIT(IR_NEQ, STB_LANG_IR_AS_TEMP(IR_REG, temp_reg), STB_LANG_IR_LHS(), STB_LANG_IR_RHS()); \
            return STB_LANG_IR_AS_TEMP(IR_REG, temp_reg)
        )
        STB_LANG_IR_CASE(AST_IF,
            STB_LANG_IR_NEW_LABEL(label)

            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS();
            char *temp = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_REG, temp), operand, NULL);

            STB_LANG_IR_EMIT(IR_JUMP_IF_FALSE, STB_LANG_IR_LABEL(IR_VAR, label), operand, STB_LANG_IR_OPERAND_NAME(IR_REG, temp));
            STB_LANG_IR_BLOCK()
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label), operand, NULL);
        )
        STB_LANG_IR_CASE(AST_RET,
            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS();
            STB_LANG_IR_EMIT(IR_RET, NULL, operand, NULL);
        )
        STB_LANG_IR_CASE(AST_WHILE,
            STB_LANG_IR_NEW_LABEL(label)
            STB_LANG_IR_EMIT(IR_LABEL, STB_LANG_IR_LABEL(IR_VAR, label), NULL, NULL);

            STB_LANG_IR_NEW_LABEL(label1)

            STB_CONCAT(CUR_IR_NAME, _Operand) *operand = STB_LANG_IR_LHS();
            char *temp = STB_CONCAT(CUR_IR_PREFIX, _make_temp_reg_string)(ir);
            STB_LANG_IR_EMIT(IR_ASSIGN, STB_LANG_IR_OPERAND_NAME(IR_REG, temp), operand, NULL);
            STB_LANG_IR_EMIT(IR_JUMP_IF_FALSE, STB_LANG_IR_LABEL(IR_VAR, label1), operand, STB_LANG_IR_OPERAND_NAME(IR_REG, temp));


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
        STB_LANG_REGALLOC_2CASES(IR_MUL, IR_DIV,
            STB_LANG_SAVE_REG(phys[0], {
                STB_LANG_SAVE_REG(phys[1]);
            });
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

#define STB_LANG_ARM_MOVE(right, ...) {\
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
            STB_LANG_EMIT_CODE("\tmov %s, x%d\n", leftr, atoi(right->value+1)); \
        }else if (instr->dest->value[0] == '.'){ \
            int n = atoi(instr->dest->value + 4); \
            STB_LANG_EMIT_CODE("\tldr %s, [sp, #%d]\n", leftr, n * 8); \
        }else { \
            STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, STB_LANG_REGISTER(right->phys, 8)); \
        } \
    }else if (right->type == IR_MEM){ \
        STB_LANG_EMIT_CODE("\tadr %s, mem_%ld\n", leftr, (long)right->value); \
    } \
} \
}

#define STB_LANG_ARM_BINARY(op) \
STB_LANG_ARM_MOVE(instr->left, STB_LANG_REGISTER(instr->phys[0], 8)); \
if (instr->phys[1] != -1){ \
    STB_LANG_ARM_MOVE(instr->right, STB_LANG_REGISTER(instr->phys[1], 8)); \
    STB_LANG_EMIT_CODE("\t%s %s, %s, %s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys[0], 8), STB_LANG_REGISTER(instr->phys[1], 8)); \
}else { \
    STB_LANG_EMIT_CODE("\t%s %s, %s, #%s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys[0], 8), instr->right->value); \
}


#define STB_LANG_ARM_COMPARISON(op) \
STB_LANG_ARM_MOVE(instr->left, STB_LANG_REGISTER(instr->phys[0], 8)); \
if (instr->phys[1] != -1){ \
    STB_LANG_ARM_MOVE(instr->right, STB_LANG_REGISTER(instr->phys[1], 8)); \
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
        STB_LANG_ITERATE(gen->symbols, Lang_IR_Symbol, 
// This looks wrong, but there are variables idx and iter used in the iterate function
// This is not meant to be used that seriously, so take it with a grain of salt.
            STB_LANG_EMIT_CODE("mem_%d: .asciz \"%s\\0\"\n", idx, iter.data);
        )
    ),
    STB_LANG_CODEGEN_LIST(
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_BEGIN,
            STB_LANG_EMIT_CODE("_%s:\n", instr->dest->value); \
            STB_LANG_EMIT_CODE("\tstp x29, x30, [sp, #-16]!\n"); \
            STB_LANG_EMIT_CODE("\tmov x29, sp\n"); \
            STB_LANG_EMIT_CODE("\tsub sp, sp, #64\n"); \
            STB_LANG_GO_TO_FUNC(instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_END,
            STB_LANG_EMIT_CODE("\tmov sp, x29\n"); \
            STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n"); \
            STB_LANG_EMIT_CODE("\tret\n");
        )
        STB_LANG_CODEGEN_CASE(IR_LABEL,
            STB_LANG_EMIT_CODE(".L_label_%s:\n", instr->dest->value)
        )
        STB_LANG_CODEGEN_2CASES(IR_ASSIGN, IR_DECL,
            // TODO: merge this entirely with STB_LANG_ARM_MOVE
            ;if (instr->dest->type == IR_VAR){
                int size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->dest->value);
                int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value);

                if (instr->left != NULL){
                    STB_LANG_ARM_MOVE(instr->left, STB_LANG_REGISTER(instr->phys[0], size))
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
                    STB_LANG_ARM_MOVE(instr->left, string)
                }else if (instr->dest->value[0] == 't'){
                    snprintf(string, 32, "%s", instr->dest->value);
                    STB_LANG_ARM_MOVE(instr->left, string)
                }else if (instr->dest->value[0] == '.'){
                    int n = atoi(instr->dest->value + 4);
                    STB_LANG_ARM_MOVE(instr->left, STB_LANG_REGISTER(instr->phys[0], 8))
                    STB_LANG_EMIT_CODE("\tstr %s, [sp, #%d]\n", STB_LANG_REGISTER(instr->phys[0], 8), n*8);
                }
            };
        )
        STB_LANG_CODEGEN_CASE(IR_PUSH,
            STB_LANG_ARM_MOVE(instr->left, STB_LANG_REGISTER(instr->phys[0], 8));
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
        STB_LANG_CODEGEN_CASE(IR_RET,
            STB_LANG_ARM_MOVE(instr->left, "x0")

            STB_LANG_EMIT_CODE("\tmov sp, x29\n"); \
            STB_LANG_EMIT_CODE("\tldp x29, x30, [sp], #16\n"); \
            STB_LANG_EMIT_CODE("\tret\n");
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


            STB_LANG_ARM_MOVE(instr->left, left);
            STB_LANG_ARM_MOVE(instr->right, right);

            ;
            STB_LANG_EMIT_CODE("\tsdiv %s, %s, %s\n", tmp, left, right);
            {
                STB_LANG_EMIT_CODE("\tmsub %s, %s, %s, %s\n", STB_LANG_REGISTER(instr->dest->phys, 8), tmp, right, left, 8);
            }
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
    Lang_Tokenizer *tokenizer = lang_tokenizer_init("examples/a.lang");
    while (lang_tokenizer_token(tokenizer) == 0){
    }
    // int len = tokenizer->tokens.datalen;
    // for (int i=0; i<len; i++){
    //     printf("%d, %s\n", tokenizer->tokens.data[i].type, tokenizer->tokens.data[i].value);
    // }
    

    Lang_Parser *parser = lang_parser_init(tokenizer);
    while (lang_parser_parse_body(parser) == 0){
    }

    // Lang_Parser_AST *ast = GetLinkedListHead((*parser), Lang_Parser_AST);
    // Lang_Parser_AST *left = (Lang_Parser_AST*)ast->left;
    // printf("%s\n", left->value);


    Lang_TypeInfo *checker = lang_typeinfo_init(parser);
    STB_LANG_ADD_FUNCTION("printf", AST_TYPE_VOID,
        STB_LANG_FUNCTION_ADD_PARAM(AST_TYPE_STRING);
        STB_LANG_FUNCTION_ADD_PARAM(STB_LANG_AST_TYPE_VARIADIC);
    )
    STB_LANG_ADD_FUNCTION("exit", AST_TYPE_VOID,
        STB_LANG_FUNCTION_ADD_PARAM(AST_TYPE_INT);
    )
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
    printf("-------- ASSEMBLY CODE --------\n");
    printf("%s", gen->code.data);
    printf("-------------------------------\n\n");
    return 0;
}
