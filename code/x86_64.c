#define STB_LANG_X86_64_FORMAT(size) size == 4 ? "dword" : size == 1 ? "byte" : "qword"

#define STB_LANG_X86_64_MOVE(size, right, ...) { \
char *leftr = __VA_ARGS__; \
if (leftr[0] == 't'){ \
    leftr = STB_LANG_REGISTER(instr->dest->phys, 8); \
} \
if (right->type == IR_INT){ \
    STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, right->value);\
}else if(right->type == IR_MEM){ \
    STB_LANG_EMIT_CODE("\tlea %s, qword [mem_%d]\n", leftr, (long)right->value); \
}else if (right->type == IR_VAR){ \
    int _size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, right->value); \
    int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, right->value); \
    STB_LANG_EMIT_CODE("\tmov %s, %s [rbp - %d]\n", leftr, STB_LANG_X86_64_FORMAT(_size), offset);\
}else if (right->type == IR_REG){ \
    if (right->value[0] == 'a'){ \
        STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr, STB_LANG_X86_64_ARGS[atoi(right->value+1)]); \
    }else if (right->value[0] == '.'){ \
        STB_LANG_EMIT_CODE("\tmov %s, rax\n", leftr); \
    }else if (right->value[0] == 't'){ \
        STB_LANG_EMIT_CODE("\tmov %s, %s\n", leftr,  STB_LANG_REGISTER(right->phys, 8)); \
    } \
} \
}

#define STB_LANG_X86_64_FORMAT_STRING(str) \
STB_LANG_EMIT_CODE("\'"); \
int control = 0; \
for (int i=0; i<strlen(str); i++){ \
    if (str[i] == '\\'){ \
        control = 1; \
    }else { \
        if (control == 1){ \
            switch (str[i]){ \
                case 'n': STB_LANG_EMIT_CODE("', 10, '"); break; \
                case '0': STB_LANG_EMIT_CODE("', 0, '"); break; \
            } \
            control = 0; \
        }else { \
            STB_LANG_EMIT_CODE("%c", str[i]); \
        } \
    }; \
} \
STB_LANG_EMIT_CODE("', 0");

char* STB_LANG_X86_64_ARGS[] = {
    "rdi",
    "rsi",
    "rcx"
};

#define STB_LANG_X86_64_BINARY(op) \
STB_LANG_X86_64_MOVE(8, instr->left, STB_LANG_REGISTER(instr->dest->phys, 8)); \
if (instr->right->type != IR_INT){ \
    STB_LANG_X86_64_MOVE(8, instr->right, STB_LANG_REGISTER(instr->phys[1], 8)); \
    STB_LANG_EMIT_CODE("\t%s %s, %s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), STB_LANG_REGISTER(instr->phys[1], 8)); \
}else { \
    STB_LANG_EMIT_CODE("\t%s %s, %s\n", op, STB_LANG_REGISTER(instr->dest->phys, 8), instr->right->value); \
}



STB_LANG_NEW_CODEGEN(
    STB_LANG_CODEGEN_PREFIX(
        STB_LANG_EMIT_CODE("global _main\n");
        STB_LANG_EMIT_CODE("default rel\n");
    ),
    STB_LANG_CODEGEN_SUFFIX(
        STB_LANG_EMIT_CODE("section .data\n");
        STB_LANG_EMIT_CODE("align 8\n");
        STB_LANG_ITERATE(gen->symbols, Lang_IR_Symbol, 
// This looks wrong, but there are variables idx and iter used in the iterate function
// This is not meant to be used that seriously, so take it with a grain of salt.
            STB_LANG_EMIT_CODE("mem_%d: db ", idx);
            STB_LANG_X86_64_FORMAT_STRING(iter.data);
            STB_LANG_EMIT_CODE("\n");
        )
    ),
    STB_LANG_CODEGEN_LIST(

        STB_LANG_CODEGEN_CASE(IR_EXTERN,
            STB_LANG_EMIT_CODE("extern _%s\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_BEGIN,
            STB_LANG_EMIT_CODE("_%s:\n", instr->dest->value);
            STB_LANG_EMIT_CODE("\tpush rbp\n");
            STB_LANG_EMIT_CODE("\tmov rbp, rsp\n");


            int offset = 16;
            STB_LANG_GO_TO_FUNC(instr->dest->value);
            STB_LANG_ITERATE(STB_LANG_CURRENT_SCOPE()->symbols, Lang_TypeInfo_Symbol,
                if (iter.kind == STB_LANG_SYMBOL_VARIABLE){
                    offset = iter.data.variable.offset;
                };
            );
            STB_LANG_EMIT_CODE("\tsub rsp, %d\n", (offset + 15 + 16) & ~15);
        )
        STB_LANG_CODEGEN_CASE(IR_CALL,
            STB_LANG_EMIT_CODE("\tcall _%s\n", instr->dest->value);
        )
        STB_LANG_CODEGEN_CASE(IR_ADD,
            STB_LANG_X86_64_BINARY("add");
        )
        STB_LANG_CODEGEN_CASE(IR_SUB,
            STB_LANG_X86_64_BINARY("sub");
        )
        STB_LANG_CODEGEN_CASE(IR_MUL,
            STB_LANG_X86_64_BINARY("imul");
        )
        STB_LANG_CODEGEN_CASE(IR_ADDR,
            char *dest = STB_LANG_REGISTER(instr->dest->phys, 8);
            if (instr->left->type == IR_VAR){
                int _size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->left->value);
                int newoffset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->left->value);
                STB_LANG_EMIT_CODE("\tlea %s, %s [rbp - %d]\n", dest, STB_LANG_X86_64_FORMAT(_size), newoffset);
            }
        )
        STB_LANG_CODEGEN_CASE(IR_STORE,
            int _size = STB_LANG_LOOKUP_SIZE(gen->root_scope, &instr->typeinfo);

            char *val_reg = STB_LANG_REGISTER(instr->phys[0], _size);
            STB_LANG_X86_64_MOVE(size, instr->left, val_reg);

            char *addr_reg = STB_LANG_REGISTER(instr->dest->phys, 8);
            STB_LANG_X86_64_MOVE(8, instr->dest, addr_reg);

            STB_LANG_EMIT_CODE("\tmov %s [%s], %s\n", STB_LANG_X86_64_FORMAT(_size), addr_reg, val_reg);
        )
        STB_LANG_CODEGEN_CASE(IR_LOAD,
            char *val_reg = STB_LANG_REGISTER(instr->phys[0], 8);
            STB_LANG_X86_64_MOVE(size, instr->left, val_reg);

            char *entr_reg = STB_LANG_REGISTER(instr->dest->phys, 8);
            STB_LANG_X86_64_MOVE(8, instr->dest, entr_reg);


            STB_LANG_EMIT_CODE("\tmov %s, qword [%s]\n", entr_reg, val_reg);
        )
        STB_LANG_CODEGEN_2CASES(IR_ASSIGN, IR_DECL,
            if (instr->dest->type == IR_VAR){
                int size = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_size_from_var)(gen, instr->dest->value);
                int offset = STB_CONCAT(CUR_CODEGEN_PREFIX, _get_offset_from_var)(gen, instr->dest->value);
                char str[32]; snprintf(str, 32, "%s [rbp - %d]", STB_LANG_X86_64_FORMAT(size), offset);
                STB_LANG_X86_64_MOVE(8, instr->left, strdup(str));
            }else if (instr->dest->type == IR_REG){
                if (instr->dest->value[0] == 'a'){
                    STB_LANG_X86_64_MOVE(8, instr->left, STB_LANG_X86_64_ARGS[atoi(instr->dest->value+1)]);
                }else if (instr->dest->value[0] == '.'){
                    STB_LANG_X86_64_MOVE(8, instr->left, "rax");
                }else if (instr->dest->value[0] == 't'){
                    STB_LANG_X86_64_MOVE(8, instr->left, instr->dest->value);
                }
            }

        )
        STB_LANG_CODEGEN_CASE(IR_RET,
            STB_LANG_X86_64_MOVE(8, instr->left, "rax");
            STB_LANG_EMIT_CODE("\tmov rsp, rbp\n");
            STB_LANG_EMIT_CODE("\tpop rbp\n");
            STB_LANG_EMIT_CODE("\tret\n");
        )
        STB_LANG_CODEGEN_CASE(IR_FUNCDEF_END,
            STB_LANG_EMIT_CODE("\tmov rsp, rbp\n");
            STB_LANG_EMIT_CODE("\tpop rbp\n");
            STB_LANG_EMIT_CODE("\tret\n");
        )
    )
)
