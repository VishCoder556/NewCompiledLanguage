#ifndef STB_LANG_ERROR_H
#define STB_LANG_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void stb_lang_error_major_global(char *type, char *fmt, ...);



#ifdef STB_LANG_ERROR_IMPLEMENTATION

void stb_lang_error_major_global(char *type, char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    int addcap = 256;
    char *add = malloc(addcap);
    if (add == NULL){
        printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to genereate errors");
        exit(-1);
    }

    int n = vsnprintf(add, addcap, fmt, args);

    if (n > addcap){
        addcap = n;
        add = realloc(add, addcap);
        if (add == NULL){
            printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to genereate errors");
            exit(-1);
        }
    }
    va_end(args);


    printf("\x1b[1;31m%s\x1b[0m: %s\n", type, add);
    exit(-1);
}

void stb_lang_error_minor(char *file, char *contents, int offset, char *type, char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    int addcap = 256;
    char *add = malloc(addcap);
    if (add == NULL){
        printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to genereate errors");
        exit(-1);
    }

    int n = vsnprintf(add, addcap, fmt, args);

    if (n > addcap){
        addcap = n;
        add = realloc(add, addcap);
        if (add == NULL){
            printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space to genereate errors");
            exit(-1);
        }
    }
    va_end(args);

    int row = 0;
    int col=0;
    int last = 0;
    int i=0;
    for (; i<offset; i++){
        if (contents[i] == '\n') {row++;col=i;last=i;};
        if (contents[i] == '\0') {break;};
    };
    char *line = strdup(contents + col);
    char *oldline = line;
    if (line[0] == '\n'){line++;}

    i=last + 1;
    while(1){
        if (contents[i] == '\n') {break;};
        if (contents[i] == '\0') {break;};
        i++;
    };
    line[i-(last + 1)] = '\0';

    col = (offset - col - 1);


    int newrow = row;
    int count = 0;
    do {
        count++;
        newrow /= 10;
    } while (newrow != 0);

    // printf("\x1b[1;37m%s:%d:%d: \x1b[1;31m%s\x1b[0m: %s\n%s\n", file, row, col, type, add, line);
    printf("\x1b[1;37m%s:%d:%d: \x1b[1;31m%s\x1b[0m: %s\n%d |  %s\n", file, row, col, type, add, row, line);
    for (int i=0; i<count+1; i++){printf(" ");}
    printf("|  ");
    for (int i=0; i<col; i++){printf(" ");}
    printf("^");
    printf("\n");
    free(oldline);
    exit(-1);
}

#endif

#endif
