#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "sym_table.h"
#include "eval.h"

static sym_table main_t;
static bool is_programm_running = false;

Value make_int(int x)   { return (Value){VAL_INT,   .value.IntVal=x}; }
Value make_float(float x){ return (Value){VAL_FLOAT, .value.FloatVal=x}; }
Value make_bool(bool x) { return (Value){VAL_BOOL,  .value.BoolVal=x}; }

bool is_true(Value v) {
    switch (v.valueType) {
        case VAL_BOOL: return v.value.BoolVal;
        case VAL_INT:  return v.value.IntVal != 0;
        case VAL_FLOAT:return v.value.FloatVal != 0.0f;
        case VAL_NONE: return false;
        case VAL_ERROR: return false;
    }
    return false;
}

Value eval_expr(sym_table T, AST *node) {
    if (!node) {
        printf("Runtime error: null expression\n");
        return make_int(0);
    }


    switch (node->type) {

        case AST_INT:
            return make_int(node->as.int_val);

        case AST_FLOAT:
            return make_float(node->as.float_val);

        case AST_BOOL:
            return make_bool(node->as.bool_val);

        case AST_VAR:
            Value temp = get_variable(&T, node->as.var_name);
            if(temp.valueType == VAL_ERROR){
                printf("Unknown variable %s\n", node->as.var_name);
            }
            return temp;


        case AST_GROUPING:
            return eval_expr(T, node->as.grouping.expr);

        case AST_ASSIGN: {
            Value v = eval_expr(T, node->as.assign.value);
            switch (v.valueType)
            {
            case VAL_INT:
                set_variable(&T, node->as.assign.var_name, &v.value.IntVal ,v.valueType);
                break;
            case VAL_FLOAT:
                set_variable(&T, node->as.assign.var_name, &v.value.FloatVal ,v.valueType);
                break;
            case VAL_BOOL:
                set_variable(&T, node->as.assign.var_name, &v.value.BoolVal ,v.valueType);
                break;
            case VAL_NONE:
            case VAL_ERROR:
                break;
            
            }
            return v;
        }

        case AST_UNARY: {
            Value r = eval_expr(T, node->as.unary.right);

            if (node->as.unary.op == TOKEN_MINUS) {
                if (r.valueType == VAL_INT)   return make_int(-r.value.IntVal);
                if (r.valueType == VAL_FLOAT) return make_float(-r.value.FloatVal);
            }

            if (node->as.unary.op == TOKEN_NOT) {
                return make_bool(!is_true(r));
            }

            printf("Runtime error: unsupported unary operator\n");
            return make_int(0);
        }

        case AST_BINARY: {
            Value L = eval_expr(T, node->as.binary.left);
            Value R = eval_expr(T, node->as.binary.right);
            int op = node->as.binary.op;

            switch (op) {
                case TOKEN_PLUS:
                    if (L.valueType == VAL_FLOAT || R.valueType == VAL_FLOAT)
                        return make_float((L.valueType == VAL_FLOAT ? L.value.FloatVal : L.value.IntVal) + (R.valueType == VAL_FLOAT?R.value.FloatVal:R.value.IntVal));
                    return make_int(L.value.IntVal + R.value.IntVal);

                case TOKEN_MINUS:
                    if (L.valueType == VAL_FLOAT || R.valueType == VAL_FLOAT)
                        return make_float((L.valueType == VAL_FLOAT ? L.value.FloatVal : L.value.IntVal) - (R.valueType == VAL_FLOAT?R.value.FloatVal:R.value.IntVal));
                    return make_int(L.value.IntVal - R.value.IntVal);

                case TOKEN_MULT:
                    if (L.valueType == VAL_FLOAT || R.valueType == VAL_FLOAT)
                        return make_float((L.valueType == VAL_FLOAT ? L.value.FloatVal : L.value.IntVal) * (R.valueType == VAL_FLOAT ? R.value.FloatVal : R.value.IntVal));
                    return make_int(L.value.IntVal * R.value.IntVal);

                case TOKEN_DIV:
                    float divisor = (R.valueType == VAL_FLOAT ? R.value.FloatVal : R.value.IntVal);

                    if (divisor == 0) {
                        printf("Runtime error: division by zero\n");
                        return make_int(0);
                    }
                    return make_float((L.valueType == VAL_FLOAT?L.value.FloatVal:L.value.IntVal) / (float)(R.valueType == VAL_FLOAT?R.value.FloatVal:R.value.IntVal));

                case TOKEN_EQ:
                    return make_bool(L.value.IntVal == R.value.IntVal);

                case TOKEN_NEQ:
                    return make_bool(L.value.IntVal != R.value.IntVal);

                case TOKEN_LT:
                    return make_bool(L.value.IntVal < R.value.IntVal);

                case TOKEN_GT:
                    return make_bool(L.value.IntVal > R.value.IntVal);

                case TOKEN_LE:
                    return make_bool(L.value.IntVal <= R.value.IntVal);

                case TOKEN_GE:
                    return make_bool(L.value.IntVal >= R.value.IntVal);

                case TOKEN_AND:
                    return make_bool(is_true(L) && is_true(R));

                case TOKEN_OR:
                    return make_bool(is_true(L) || is_true(R));
            }

            printf("Runtime error: unsupported binary operator\n");
            return make_int(0);
        }
        
        default:
        printf("Invalid expression node\n");
        return make_int(0);
    }
}

void eval_stmt(sym_table T, AST *node) {
    if (!node) return;

    switch (node->type) {

        case AST_EXPR_STMT:
            eval_expr(T, node->as.expr_stmt.expr);
            break;

        case AST_PRINT: {
            Value v = eval_expr(T, node->as.print.expr);
            if (v.valueType == VAL_INT) printf("%lld\n", v.value.IntVal);
            else if (v.valueType == VAL_FLOAT) printf("%lf\n", v.value.FloatVal);
            else if (v.valueType == VAL_BOOL) printf("%s\n", v.value.BoolVal ? "True" : "False");
            else {
                printf("Runtime error: invalid print statement\n");
                return;
            }
            break;
        }

        case AST_BLOCK:
            for (int i = 0; i < node->as.block.count; i++)
                eval_stmt(T, node->as.block.stmts[i]);
            break;

        case AST_IF: {
            if (is_true(eval_expr(T, node->as.ifnode.cond))) {
                eval_stmt(T, node->as.ifnode.then_block);
                return;
            }

            for (int i = 0; i < node->as.ifnode.elifs.count; i++) {
                if (is_true(eval_expr(T, node->as.ifnode.elifs.conds[i]))) {
                    eval_stmt(T, node->as.ifnode.elifs.blocks[i]);
                    return;
                }
            }

            if (node->as.ifnode.else_block)
                eval_stmt(T, node->as.ifnode.else_block);

            break;
        }

        case AST_WHILE: {
            while (is_true(eval_expr(T, node->as.whilenode.cond))) {
                eval_stmt(T, node->as.whilenode.body);
            }
            break;
        }

        case AST_ASSIGN:
            eval_expr(T, node);
            break;

        case AST_DEL:
            int res = del_variable(&main_t, node->as.var_name);
            if (res == -1) {
                printf("Error: cannot delete '%s' (not found)\n", node->as.var_name);
            }
            break;
        default:
            printf("Unknown statement node: %d\n", node->type);
    }
}


Value eval(const char* programm)
{   

    if (!is_programm_running) {
        main_t = create_table();
        is_programm_running = true; 
    }

    Lexer L;
    L.pos = 0;
    L.charToRead = (char*)programm;

    Parser P;
    P.L = &L;

    ParsingRes res = parse_programm(&P);

    if (!res.root) {
        return (Value){.valueType=VAL_NONE,.value.IntVal=0};
    }

    if (res.root->type == AST_BLOCK) {
        int count = res.root->as.block.count;

        if (count == 0) return (Value){.valueType=VAL_NONE,.value.IntVal=0};

        AST *last = res.root->as.block.stmts[count - 1];

        for (int i = 0; i < count - 1; i++) {
            eval_stmt(main_t, res.root->as.block.stmts[i]);
        }

        if (last->type == AST_EXPR_STMT) {
            AST *expr = last->as.expr_stmt.expr;

            // DO NOT auto-print assignments
            if (expr->type == AST_ASSIGN) {
                eval_expr(main_t, expr);
                return (Value){.valueType=VAL_NONE,.value.IntVal=0};
            }

            return eval_expr(main_t, expr);
        }

        eval_stmt(main_t, last);
        return (Value){.valueType=VAL_NONE,.value.IntVal=0};
    }

    // Fallback 
    eval_stmt(main_t, res.root);
    return (Value){.valueType=VAL_NONE,.value.IntVal=0};
}

void free_eval(void){
    free_table(main_t);
}
