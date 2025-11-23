#ifndef LEXER_IMPL_H
#define LEXER_IMPL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TOKEN_TEXT_MAX 64
typedef enum {
    // syntax
    TOKEN_EOF = 0,
    TOKEN_EOL = 1,
    TOKEN_INDENT,
    TOKEN_DEDENT,
    TOKEN_COLON,
    TOKEN_LPARENT,
    TOKEN_RPARENT,
    // keywords
    TOKEN_IF = 10,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_PRINT,
    //data types
    TOKEN_INT = 20,
    TOKEN_FLOAT,
    TOKEN_ID,
    TOKEN_TRUE,
    TOKEN_FALSE,
    // ariphmetic
    TOKEN_PLUS = 30,
    TOKEN_MINUS, 
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_MODULO,
    TOKEN_DIV_INT,  // python-specific "//" token
    // comparison
    TOKEN_EQ = 40,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LE,
    TOKEN_GE,
    // logical
    TOKEN_NOT = 50,
    TOKEN_AND,
    TOKEN_OR,

    TOKEN_ASSIGN = 100,

    TOKEN_ERROR = 200
} TokenType;

typedef struct
{
    TokenType type;
    char name[TOKEN_TEXT_MAX];
    union {
        long long IntVal;   // also can store ERROR code
        double FloatVal;
    } val;
    
}Token;

typedef struct 
{
    char *charToRead;
    unsigned int pos;
}Lexer;

typedef enum
{
    INVALID_TOKEN = 1,
    UNKNOWN_SYMBOL,
    UNEXPECTED_INDENT,
    EXPECTED_INDENT,        // always Indentate after colon
    INCONSISTENT_INDENT = 5
}lexer_errors;


Token next_token(Lexer *L);

#ifdef __cplusplus
}
#endif

#endif //LEXER_IMPL_H