#ifndef EVAL_SHELL_
#define EVAL_SHELL_

#include "parser.h"
#include "sym_table.h"

void free_eval(void);

Value eval(const char *input);

#endif // EVAL_SHELL_
