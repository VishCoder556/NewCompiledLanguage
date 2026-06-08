#ifndef STB_LANG_TOKENIZER_H
#define STB_LANG_TOKENIZER_H

typedef struct {
    FILE *file;

    char *name;

    char *contents;
    int contentslen;
}STB_Lang_Tokenizer_File;

typedef struct {
    STB_Lang_Tokenizer_File file;
    ;
}STB_Lang_Tokenizer;

STB_Lang_Tokenizer *stb_lang_tokenizer_init(char *name){
    FILE *file = fopen(name, "r");
    if (file == NULL){
        stb_lang_error_major_global("FileError", "Could not find file '%s'", name);
    }
    return NULL;
};


#endif
