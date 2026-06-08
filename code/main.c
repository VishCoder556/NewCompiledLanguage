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
#include "../libraries/error/error.h"
#include "../libraries/tokenizer/tokenizer.h"
#include "../libraries/parser/parser.h"

int main(int argc, char **argv){
    stb_lang_tokenizer_init("examples/a.lang");
    printf("Hello World\n");
    return 0;
}
