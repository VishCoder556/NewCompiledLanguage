#ifndef STB_LANG_DRIVER_H
#define STB_LANG_DRIVER_H

#include <stdio.h>
#include <stdlib.h>

#define STB_LANG_DRIVER_WRITE_DATA(path) \
FILE *f = fopen(path, "w"); \
if (!f) { \
    stb_lang_error_major_global("DriverError", "Could not open target path \"%s\"", path); \
    return -1; \
} \
fprintf(f, "%s", gen->code.data); \
if (ferror(f)){ \
    stb_lang_error_major_global("DriverError", "Error occured when writing to \"%s\"", path); \
    return -1; \
} \
fclose(f);

#define STB_LANG_DRIVER_RUN_SCRIPT(...) STB_CONCAT(CUR_DRIVER_PREFIX, _run)(__VA_ARGS__);

#define STB_LANG_NEW_DRIVER(...) \
void STB_CONCAT(CUR_DRIVER_PREFIX, _run)(char *fmt, ...){ \
    va_list args; \
    va_start(args, fmt); \
    int addcap = 256; \
    char *add = malloc(addcap); \
    if (add == NULL){ \
        printf("\x1b[1;31m%s\x1b[0m: %s\n", "DriverError", "Could not run script"); \
        exit(-1); \
    } \
    int n = vsnprintf(add, addcap, fmt, args); \
    if (n > addcap){ \
        addcap = n; \
        add = realloc(add, addcap); \
        if (add == NULL){ \
            printf("\x1b[1;31m%s\x1b[0m: %s\n", "ErrorGeneratorError", "Not enough space for script"); \
            exit(-1); \
        } \
        snprintf(add, n, fmt, args); \
    } \
    va_end(args); \
    system(add); \
} \
char STB_CONCAT(CUR_DRIVER_PREFIX, _init)(CUR_CODEGEN_NAME *gen){ \
    __VA_ARGS__; \
    return 0; \
};\

#define STB_LANG_INVOKE_DRIVER(gen) do {\
    if (STB_CONCAT(CUR_DRIVER_PREFIX, _init)(gen) == -1){ \
        stb_lang_error_major_global("DriverError", "Something went wrong"); \
    } \
}while(0);

#endif
