#ifndef AST_H_
#define AST_H_

#include "lexer.h"

typedef enum{
    AST_OPERAND,
    AST_UNARY,
    AST_ASSIGN,
    AST_INT,
    AST_FLOAT, 
    AST_VAR,
}AST_type;

typedef struct ast AST;
typedef struct ast
{   
    AST_type type;
    Token* token;
    AST* left;
    AST* right;
}AST;

AST* make_node(AST_type t);
#endif //AST_H_
