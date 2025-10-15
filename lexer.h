#ifndef LEXER_IMPL_H
#define LEXER_IMPL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TOKEN_TEXT_MAX 64
typedef enum {
    //data types
    TOKEN_EOL = 0,
    TOKEN_INT = 10,
    TOKEN_FLOAT,
    TOKEN_IDENT,
    // ariphmetic
    TOKEN_PLUS = 20,
    TOKEN_MINUS, 
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_MODULO,
    TOKEN_DIV_INT,  // python-specific "//" token
    // logical
    TOKEN_NOT = 30,
    TOKEN_AND,
    TOKEN_OR,
    // control operators
    TOKEN_LPARENT=40,
    TOKEN_RPARENT,
    // assignment
    TOKEN_ASSIGN = 100
} TokenType;

typedef struct
{
    int type;
    char name[TOKEN_TEXT_MAX];
    union {
        int IntVal;
        float FloatVal;
    } val;
    
}Token;

typedef struct 
{
    char *charToRead;
    unsigned int pos;
}Lexer;

Token next_token(Lexer *L);
#endif //LEXER_IMPL_H