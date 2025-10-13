#include "lexer.h"
#include <assert.h>

void print_token(Token t) {
    printf("Type=%d", t.type);
    if (t.name) printf(", Name='%s'", t.name);
    if (t.type == TOKEN_INT)
        printf(", IntVal=%d", t.val.IntVal);
    else if (t.type == TOKEN_FLOAT)
        printf(", FloatVal=%f", t.val.FloatVal);
    printf("\n");
}

void run_test(const char *src, const int *expected_types, int expected_count) {
    printf("\n--- Running test: \"%s\" ---\n", src);
    Lexer L = { .charToRead = (char *)src, .pos = 0 };
    for (int i = 0; i < expected_count; i++) {
        Token t = next_token(&L);
        print_token(t);
        assert(t.type == expected_types[i]);
    }
    printf("Passed\n");
}

int main(void) {
    int t1[] = {TOKEN_INT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOL};
    run_test("10 + 3.14", t1, 4);

    int t2[] = {TOKEN_IDENT, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOL};
    run_test("x = 42", t2, 4);

    int t3[] = {TOKEN_INT, TOKEN_MULT, TOKEN_LPARENT, TOKEN_INT, TOKEN_PLUS, TOKEN_INT, TOKEN_RPARENT, TOKEN_EOL};
    run_test("2 * (3 + 4)", t3, 8);

    int t4[] = {TOKEN_INT, TOKEN_DIV_INT, TOKEN_INT, TOKEN_MODULO, TOKEN_INT, TOKEN_EOL};
    run_test("8 // 3 % 2", t4, 6);

    int t5[] = {TOKEN_NOT, TOKEN_IDENT, TOKEN_AND, TOKEN_IDENT, TOKEN_OR, TOKEN_IDENT, TOKEN_EOL};
    run_test("not x and y or z", t5, 7);

    int t6[] = {TOKEN_NOT, TOKEN_IDENT, TOKEN_DIV_INT, TOKEN_IDENT, TOKEN_OR, TOKEN_IDENT, TOKEN_RPARENT, TOKEN_EOL};
    run_test("   not   x // y5 or _z )   ", t6, 8);

    int t7[] = {TOKEN_IDENT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOL};
    run_test("a+3.14", t7, 4);


    printf("\nAll lexer tests passed!\n");
    return 0;
}
