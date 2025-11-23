#include "parser.h"
#include "ast.h"
#include <stdlib.h>
#include <unistd.h>
AST* parse_stmt_list(Parser* P);

void next_token_p(Parser *P) {
    P->current = next_token(P->L);
}

bool match(Parser *P, TokenType type) {
    if (P->current.type == type) {
        next_token_p(P);
        return true;
    }
    return false;
}

void expect(Parser *P, TokenType type) {
    if (!match(P, type)) {
        // later handle error
        printf("Syntax error: expected %d, got %d\n",
                type, P->current.type);
        // printf("Lexer: %s", 
        // &(P->L->charToRead[P->L->pos]));
        // exit(1);
    }
}

AST *parse_precedence(Parser *P, int min_bp);
AST *nud(Parser *P, Token tok);
AST *led(Parser *P, AST *left, Token op);


int prec(TokenType t)
{
    switch(t) {
        case TOKEN_ASSIGN:return 1;
        case TOKEN_OR:    return 2;
        case TOKEN_AND:   return 3;
        case TOKEN_EQ:
        case TOKEN_NEQ:
        case TOKEN_LT:
        case TOKEN_GT:
        case TOKEN_LE:
        case TOKEN_GE:    return 4;
        case TOKEN_PLUS:
        case TOKEN_MINUS: return 5;
        case TOKEN_MULT:
        case TOKEN_DIV:
        case TOKEN_DIV_INT:
        case TOKEN_MODULO: return 6;
        default:
            return 0;
    }
}

AST *nud(Parser *P, Token tok)
{
    switch (tok.type) {
        case TOKEN_INT:
            return ast_make_int(tok.val.IntVal);
        case TOKEN_FLOAT:
            return ast_make_float(tok.val.FloatVal);
        case TOKEN_TRUE:
            return ast_make_bool(true);
        case TOKEN_FALSE:
            return ast_make_bool(false);
        case TOKEN_ID:
            return ast_make_var(tok.name ? tok.name : "");
        case TOKEN_LPARENT: {
            AST *expr = parse_precedence(P, 0);
            if (P->current.type != TOKEN_RPARENT) {
                printf("Syntax error: expected ')'\n");
                exit(1);
            }
            next_token_p(P); // consume RPARENT
            return ast_make_grouping(expr);
        }
        case TOKEN_MINUS: {
            AST *rhs = parse_precedence(P, 100);
            return ast_make_unary(TOKEN_MINUS, rhs);
        }
        case TOKEN_NOT: {
            AST *rhs = parse_precedence(P, 100);
            return ast_make_unary(TOKEN_NOT, rhs);
        }
        default:
            printf("Unexpected token in expression (nud): %d\n", tok.type);
            exit(1);
    }
}

AST *led(Parser *P, AST *left, Token op)
{
    if (op.type == TOKEN_ASSIGN) {
        if (!left || left->type != AST_VAR) {
            printf("Syntax error: left side of assignment must be a variable\n");
            exit(1);
        }
        AST *right = parse_precedence(P, prec(op.type) - 1);
        AST *assign = ast_make_assign(left->as.assign.var_name, right);
        left->as.assign.var_name = NULL;
        ast_free(left);
        return assign;
    }

    int p = prec(op.type);
    AST *right = parse_precedence(P, p);
    return ast_make_binary(left, op.type, right);
}

AST *parse_precedence(Parser *P, int min_bp)
{
    Token tok = P->current;
    next_token_p(P);
    AST *left = nud(P, tok);
    while (prec(P->current.type) > min_bp) {
        Token op = P->current;
        next_token_p(P);
        left = led(P, left, op);
    }
    return left;
}

AST* parse_expr(Parser* P){
    // printf("CALL: parse_expr\n");
    // start pratt parsing
    return parse_precedence(P, 0);
}

AST* parse_statement(Parser* P){
    // printf("CALL: parse_statement\n");
    
    TokenType type_of_stmn = P->current.type;
    switch (type_of_stmn) {
        case TOKEN_IF:
        {
            expect(P, TOKEN_IF);
            AST *cond = parse_expr(P);
            expect(P, TOKEN_COLON);
            expect(P, TOKEN_EOL);
            expect(P, TOKEN_INDENT);

            AST *then_block = parse_stmt_list(P);
            expect(P, TOKEN_DEDENT);

            // collect elifs
            AST **elif_conds = NULL;
            AST **elif_blocks = NULL;
            int elif_count = 0;

            while (match(P, TOKEN_ELIF)) {
                AST *c = parse_expr(P);
                expect(P, TOKEN_COLON);
                expect(P, TOKEN_EOL);
                expect(P, TOKEN_INDENT);
                AST *blk = parse_stmt_list(P);
                expect(P, TOKEN_DEDENT);

                elif_count++;
                elif_conds = realloc(elif_conds, sizeof(AST*) * elif_count);
                elif_blocks = realloc(elif_blocks, sizeof(AST*) * elif_count);
                elif_conds[elif_count-1] = c;
                elif_blocks[elif_count-1] = blk;
            }

            
            AST *else_block = NULL;
            if (match(P, TOKEN_ELSE)) {
                expect(P, TOKEN_COLON);
                expect(P, TOKEN_EOL);
                expect(P, TOKEN_INDENT);
                else_block = parse_stmt_list(P);
                expect(P, TOKEN_DEDENT);
            }

            return ast_make_if(cond, then_block,
                               elif_conds, elif_blocks, elif_count,
                               else_block);
        }   
        
        case TOKEN_WHILE:
        {
            expect(P, TOKEN_WHILE);
            AST *cond = parse_expr(P);
            expect(P, TOKEN_COLON);
            expect(P, TOKEN_EOL);
            expect(P, TOKEN_INDENT);
            AST *body = parse_stmt_list(P);
            expect(P, TOKEN_DEDENT);
            return ast_make_while(cond, body);
        }
        
        case TOKEN_ID:
        case TOKEN_INT:
        case TOKEN_FLOAT:
        case TOKEN_TRUE:
        case TOKEN_FALSE:
        case TOKEN_MINUS:
        case TOKEN_NOT:
        case TOKEN_LPARENT:
        {
            AST *expr = parse_expr(P);
            expect(P, TOKEN_EOL);
            return ast_make_expr_stmt(expr);
        }
        
        // case TOKEN_DEL:
        //     expect(P, TOKEN_DEL);
        //     expect(P, TOKEN_ID);
        //     break;
        
        case TOKEN_PRINT:
        {
            expect(P, TOKEN_PRINT);
            expect(P, TOKEN_LPARENT);
            AST *expr = parse_expr(P);
            expect(P, TOKEN_RPARENT);
            expect(P, TOKEN_EOL);
            return ast_make_print(expr);
        }
        
        default:
        printf("Unable to parse statement\n");
        printf("%d\n", type_of_stmn);
        // printf("Lexer: %s", 
        // &(P->L->charToRead[P->L->pos]));
        // exit(EXIT_FAILURE);

        case TOKEN_EOL:
        expect(P, TOKEN_EOL);
        return NULL;

    }
}
AST* parse_stmt_list(Parser* P){

    AST *block = ast_make_block();
    
    // printf("CALL: parse_stmt_list\n");
    while(P->current.type != TOKEN_EOF && P->current.type != TOKEN_DEDENT){
        while(P->current.type == TOKEN_EOL){
            next_token_p(P);
        }
        AST *stmt = parse_statement(P);
        if (stmt) ast_block_append(block, stmt);
    }
    return block;
}

ParsingRes parse_programm(Parser* P)
{
    // printf("CALL: parse_programm\n");
    
    next_token_p(P);              // load first token
    AST* root = parse_stmt_list(P);
    expect(P, TOKEN_EOF);

    ParsingRes res;
    res.error = 0;
    res.line = 0;
    res.error = NULL;
    res.root = root;

    return res;
}
