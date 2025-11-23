#ifndef AST_H_
#define AST_H_

#include <stdbool.h>
#include "lexer.h"

typedef enum {
    AST_INT,
    AST_FLOAT,
    AST_BOOL,
    AST_VAR,
    AST_UNARY,
    AST_BINARY,
    AST_ASSIGN,
    AST_GROUPING,

    AST_EXPR_STMT,
    AST_PRINT,
    AST_BLOCK,
    AST_IF,
    AST_WHILE,
    AST_DEL
} AST_type;

typedef struct ast AST;

typedef struct {
    AST** conds;
    AST** blocks;
    int count;
} ElifList;

typedef struct ast {
    AST_type type;

    union {

        int    int_val;
        float  float_val;
        bool   bool_val;

        char *var_name;

        struct {
            int op;
            AST* right;
        } unary;

        struct {
            int op;
            AST* left;
            AST* right;
        } binary;

        struct {
            char* var_name;
            AST* value;
        } assign;

        struct {
            AST* expr;
        } grouping;

        struct {
            AST* expr;
        } expr_stmt;

        struct {
            AST* expr;
        } print;

        struct {
            AST** stmts;
            int count;
        } block;

        struct {
            AST* cond;
            AST* then_block;
            ElifList elifs;
            AST* else_block;
        } ifnode;

        struct {
            AST* cond;
            AST* body;
        } whilenode;

    } as;

} AST;



AST* ast_make_int(int v);
AST* ast_make_float(float v);
AST* ast_make_bool(bool v);
AST* ast_make_var(const char* name);
AST* ast_make_del(const char* name);
AST* ast_make_unary(TokenType op, AST *right);
AST* ast_make_binary(AST* left, TokenType op, AST* right);
AST* ast_make_assign(const char* var, AST* value);
AST* ast_make_grouping(AST* expr);

AST* ast_make_expr_stmt(AST *expr);
AST* ast_make_print(AST *expr);

AST* ast_make_block(void);
void  ast_block_append(AST* block, AST* stmt);

AST* ast_make_if(
    AST* cond, AST* then_block,
    AST** elif_conds, AST** elif_blocks, int elif_count,
    AST* else_block
);

AST* ast_make_while(AST* cond, AST* body);

void ast_free(AST* n);
void ast_print(AST *n);

#endif // AST_H_

