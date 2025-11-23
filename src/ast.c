#include "ast.h"
#include <stdlib.h>

static inline AST *make_node(AST_type t)
{
    AST *n = calloc(1, sizeof(AST));
    if(!n){
        printf("CALLOC AST FAIL\n");
        exit(EXIT_FAILURE);
    }
    n->type = t;
    return n;
}

AST *ast_make_int(int v)
{
    AST *n = make_node(AST_INT);
    n->as.int_val = v;
    return n;
}

AST *ast_make_float(float v)
{
    AST *n = make_node(AST_FLOAT);
    n->as.float_val = v;
    return n;
}

AST *ast_make_bool(bool v)
{
    AST *n = make_node(AST_BOOL);
    n->as.bool_val = v;
    return n;
}

AST *ast_make_var(const char* name)
{
    AST *n = make_node(AST_VAR);
    n->as.var_name = strdup(name);
    return n;
}
AST* ast_make_del(const char* name)
{
    AST *n = make_node(AST_DEL);
    n->as.var_name = strdup(name);
    return n;
}

AST* ast_make_unary(TokenType op, AST* rhs)
{
    AST *n = make_node(AST_UNARY);
    n->as.unary.op = op;
    n->as.unary.right = rhs;
    return n;
}

AST* ast_make_binary(AST* lhs, TokenType op, AST* rhs)
{
    AST *n = make_node(AST_BINARY);
    n->as.binary.left = lhs;
    n->as.binary.op = op;
    n->as.binary.right = rhs;
    return n;
}

AST *ast_make_assign(const char* name, AST* rhs)
{
    AST *n = make_node(AST_ASSIGN);
    n->as.assign.var_name = strdup(name);
    n->as.assign.value = rhs;
    return n;
}

AST *ast_make_grouping(AST* expr)
{
    AST *n = make_node(AST_GROUPING);
    n->as.grouping.expr = expr;
    return n;
}
AST *ast_make_expr_stmt(AST* expr)
{
    AST *n = make_node(AST_EXPR_STMT);
    n->as.expr_stmt.expr = expr;
    return n;
}
AST *ast_make_print(AST* expr)
{
    AST *n = make_node(AST_PRINT);
    n->as.print.expr = expr;
    return n;
}

AST *ast_make_block(void) {
    AST *n = make_node(AST_BLOCK);
    n->as.block.count = 0;
    n->as.block.stmts = NULL;
    return n;
}

void ast_block_append(AST *block, AST *stmt) {
    int n = block->as.block.count;
    block->as.block.stmts =
        realloc(block->as.block.stmts, sizeof(AST*) * (n + 1));
    block->as.block.stmts[n] = stmt;
    block->as.block.count++;
}

AST *ast_make_if(
    AST* cond, AST* then_block,
    AST** elif_conds, AST** elif_blocks, int elif_count,
    AST* else_block
){
    AST *n = make_node(AST_IF);

    n->as.ifnode.cond = cond;
    n->as.ifnode.then_block = then_block;

    n->as.ifnode.elifs.conds = elif_conds;
    n->as.ifnode.elifs.blocks = elif_blocks;
    n->as.ifnode.elifs.count = elif_count;

    n->as.ifnode.else_block = else_block;

    return n;
}

AST *ast_make_while(AST *cond, AST *body)
{
    AST *n = make_node(AST_WHILE);
    n->as.whilenode.cond = cond;
    n->as.whilenode.body = body;
    return n;
}

void ast_free(AST* n)
{
    if (!n) return;

    switch (n->type) {

        case AST_VAR:
            free(n->as.var_name);
            break;

        case AST_UNARY:
            ast_free(n->as.unary.right);
            break;

        case AST_BINARY:
            ast_free(n->as.binary.left);
            ast_free(n->as.binary.right);
            break;
            
        case AST_DEL:
            free(n->as.var_name);
            break;

        case AST_ASSIGN:
            free(n->as.assign.var_name);
            ast_free(n->as.assign.value);
            break;

        case AST_GROUPING:
            ast_free(n->as.grouping.expr);
            break;

        case AST_EXPR_STMT:
            ast_free(n->as.expr_stmt.expr);
            break;

        case AST_PRINT:
            ast_free(n->as.print.expr);
            break;

        case AST_BLOCK:
            for (int i = 0; i < n->as.block.count; i++)
                ast_free(n->as.block.stmts[i]);
            free(n->as.block.stmts);
            break;

        case AST_IF:
            ast_free(n->as.ifnode.cond);
            ast_free(n->as.ifnode.then_block);

            for (int i = 0; i < n->as.ifnode.elifs.count; i++) {
                ast_free(n->as.ifnode.elifs.conds[i]);
                ast_free(n->as.ifnode.elifs.blocks[i]);
            }
            free(n->as.ifnode.elifs.conds);
            free(n->as.ifnode.elifs.blocks);

            ast_free(n->as.ifnode.else_block);
            break;

        case AST_WHILE:
            ast_free(n->as.whilenode.cond);
            ast_free(n->as.whilenode.body);
            break;

        default:
            break;
    }
}

#include "ast.h"
#include <stdio.h>

static void indent(int n) {
    while (n-- > 0) printf("  ");
}

static void ast_print_indent(AST *n, int level);

static void print_elifs(ElifList *elifs, int level) {
    if (elifs->count == 0)
        return;

    indent(level);
    printf("ElifList(%d items):\n", elifs->count);

    for (int i = 0; i < elifs->count; i++) {
        indent(level + 1);
        printf("elif %d cond:\n", i);
        ast_print_indent(elifs->conds[i], level + 2);

        indent(level + 1);
        printf("elif %d block:\n", i);
        ast_print_indent(elifs->blocks[i], level + 2);
    }
}

static void ast_print_indent(AST *n, int level)
{
    if (!n) {
        indent(level);
        printf("NULL\n");
        return;
    }

    switch (n->type)
    {
        case AST_INT:
            indent(level);
            printf("Int(%d)\n", n->as.int_val);
            break;

        case AST_FLOAT:
            indent(level);
            printf("Float(%f)\n", n->as.float_val);
            break;

        case AST_BOOL:
            indent(level);
            printf("Bool(%s)\n", n->as.bool_val ? "true" : "false");
            break;

        case AST_VAR:
            indent(level);
            printf("Var(%s)\n", n->as.var_name);
            break;


        case AST_UNARY:
            indent(level);
            printf("Unary(op=%d)\n", n->as.unary.op);
            ast_print_indent(n->as.unary.right, level + 1);
            break;

        case AST_BINARY:
            indent(level);
            printf("Binary(op=%d)\n", n->as.binary.op);

            indent(level + 1);
            printf("left:\n");
            ast_print_indent(n->as.binary.left, level + 2);

            indent(level + 1);
            printf("right:\n");
            ast_print_indent(n->as.binary.right, level + 2);
            break;

        case AST_ASSIGN:
            indent(level);
            printf("Assign(%s)\n", n->as.assign.var_name);
            ast_print_indent(n->as.assign.value, level + 1);
            break;

        case AST_GROUPING:
            indent(level);
            printf("Group(\n");
            ast_print_indent(n->as.grouping.expr, level + 1);
            indent(level);
            printf(")\n");
            break;


        case AST_EXPR_STMT:
            indent(level);
            printf("ExprStmt\n");
            ast_print_indent(n->as.expr_stmt.expr, level + 1);
            break;

        case AST_PRINT:
            indent(level);
            printf("Print\n");
            ast_print_indent(n->as.print.expr, level + 1);
            break;

        case AST_BLOCK:
            indent(level);
            printf("Block(%d stmts):\n", n->as.block.count);
            for (int i = 0; i < n->as.block.count; i++) {
                ast_print_indent(n->as.block.stmts[i], level + 1);
            }
            break;

        case AST_IF: {
            indent(level);
            printf("If\n");

            indent(level + 1);
            printf("cond:\n");
            ast_print_indent(n->as.ifnode.cond, level + 2);

            indent(level + 1);
            printf("then:\n");
            ast_print_indent(n->as.ifnode.then_block, level + 2);

            print_elifs(&n->as.ifnode.elifs, level + 1);

            if (n->as.ifnode.else_block) {
                indent(level + 1);
                printf("else:\n");
                ast_print_indent(n->as.ifnode.else_block, level + 2);
            }

            break;
        }

        case AST_WHILE:
            indent(level);
            printf("While\n");

            indent(level + 1);
            printf("cond:\n");
            ast_print_indent(n->as.whilenode.cond, level + 2);

            indent(level + 1);
            printf("body:\n");
            ast_print_indent(n->as.whilenode.body, level + 2);
            break;

        default:
            indent(level);
            printf("Unknown AST node type %d\n", n->type);
            break;
    }
}

void ast_print(AST *n)
{
    ast_print_indent(n, 0);
}

