#include "ast.h"
#include <stdlib.h>

AST *make_node(AST_type t) {
    AST *n = calloc(1, sizeof(AST));
    n->type = t;
    return n;
}
