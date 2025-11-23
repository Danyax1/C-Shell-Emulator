#ifndef PARSER_C_PY_SHELL_
#define PARSER_C_PY_SHELL_

#include "lexer.h"
#include "ast.h"

typedef struct{
    Lexer* L;
    Token current;
}Parser;

typedef enum{
    SyntaxError = 6
}parser_errors;

typedef struct{
    AST* root;
    int return_code;        // 0 - OK, any other - error code
    int line;               // 0 - if no errors, otherwise - line where error occured
    Token* error;           // NULL - if no syntax error, otherwise can contain useful info
}ParsingRes;

ParsingRes parse_programm(Parser* P);
#endif //PARSER_C_PY_SHELL_