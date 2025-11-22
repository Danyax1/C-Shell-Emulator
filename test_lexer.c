#include "lexer.h"
#include <assert.h>

void print_token(Token t) {
    printf("Type=%d", t.type);
    if (t.type == TOKEN_ID || t.type == TOKEN_INT || t.type == TOKEN_FLOAT) 
        printf(", Name='%s'", t.name);
    if (t.type == TOKEN_INT || t.type == TOKEN_TRUE || t.type == TOKEN_FALSE)
        printf(", IntVal=%lld", t.val.IntVal);
    else if (t.type == TOKEN_FLOAT)
        printf(", FloatVal=%lf", t.val.FloatVal);
    printf("\n");
}

void run_test(const char *src, const int *expected_types, int expected_count) {
    printf("\n----- \n Running test: \n \"%s\" \n-----\n", src);
    Lexer L = { .charToRead = (char *)src, .pos = 0 };
    for (int i = 0; i < expected_count; i++) {
        Token t = next_token(&L);
        #ifdef DEBUG
        print_token(t);
        #endif
        assert(t.type == expected_types[i]);
    }
    printf("Passed\n");
}

int main(void) {
    int t0[] = {TOKEN_MINUS, TOKEN_INT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF};
    run_test("-5 + 3.0", t0, 5);

    int t1[] = {TOKEN_INT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF};
    run_test("10 + 3.14", t1, 4);

    int t2[] = {TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOF};
    run_test("x = 42", t2, 4);

    int t3[] = {TOKEN_INT, TOKEN_MULT, TOKEN_LPARENT, TOKEN_INT, TOKEN_PLUS, TOKEN_INT, TOKEN_RPARENT, TOKEN_EOF};
    run_test("2 * (3 + 4)", t3, 8);

    int t4[] = {TOKEN_INT, TOKEN_DIV_INT, TOKEN_INT, TOKEN_MODULO, TOKEN_INT, TOKEN_EOF};
    run_test("8 // 3 % 2", t4, 6);

    int t5[] = {TOKEN_NOT, TOKEN_ID, TOKEN_AND, TOKEN_ID, TOKEN_OR, TOKEN_ID, TOKEN_EOF};
    run_test("not x and y or z", t5, 7);

    int t6[] = {TOKEN_NOT, TOKEN_ID, TOKEN_DIV_INT, TOKEN_ID, TOKEN_OR, TOKEN_ID, TOKEN_RPARENT, TOKEN_EOF};
    run_test("   not   x // y5 or _z )   ", t6, 8);

    int t7[] = {TOKEN_ID, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF};
    run_test("a+3.14", t7, 4);

    int t8[] = {TOKEN_ID, TOKEN_INT, TOKEN_PLUS, TOKEN_EOF};
    run_test("notnot 1+", t8, 4);

    int t9[] = {TOKEN_IF, TOKEN_ID, TOKEN_COLON, TOKEN_EOF};
    run_test("if x:", t9, 4);

    int t10[] = {TOKEN_IF, TOKEN_ID, TOKEN_COLON, TOKEN_EOL,
                TOKEN_ELIF, TOKEN_ID, TOKEN_COLON, TOKEN_EOF};
    run_test("if x:\nelif y:", t10, 8);

    int t11[] = {TOKEN_IF, TOKEN_ID, TOKEN_COLON, TOKEN_EOL,
                TOKEN_ELSE, TOKEN_COLON, TOKEN_EOF};
    run_test("if x:\nelse:", t11, 7);

    int t12[] = {TOKEN_WHILE, TOKEN_ID, TOKEN_LT, TOKEN_INT, TOKEN_COLON, TOKEN_EOF};
    run_test("while x < 5:", t12, 6);

    int t13[] = {TOKEN_INT, TOKEN_LT, TOKEN_INT, TOKEN_EOF};
    run_test("3 < 5", t13, 4);

    int t14[] = {TOKEN_INT, TOKEN_LE, TOKEN_INT, TOKEN_EOF};
    run_test("3 <= 5", t14, 4);

    int t15[] = {TOKEN_INT, TOKEN_EQ, TOKEN_INT, TOKEN_EOF};
    run_test("3 == 3", t15, 4);

    int t16[] = {TOKEN_INT, TOKEN_NEQ, TOKEN_INT, TOKEN_EOF};
    run_test("3 != 2", t16, 4);

    int t17[] = {TOKEN_INT, TOKEN_GT, TOKEN_INT, TOKEN_EOF};
    run_test("10 > 3", t17, 4);

    int t18[] = {TOKEN_INT, TOKEN_GE, TOKEN_INT, TOKEN_EOF};
    run_test("10 >= 10", t18, 4);

    // TODO: bitwise op tests here

    int t27[] = {TOKEN_ID, TOKEN_AND, TOKEN_ID, TOKEN_OR,
                TOKEN_ID, TOKEN_EQ, TOKEN_ID, TOKEN_EOF};
    run_test("a and b or c == d", t27, 8);

    int t28[] = {TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOF};
    run_test("ifx = 1", t28, 4);

    int t29[] = {TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOF};
    run_test("whiley = 2", t29, 4);

    int t30[] = {TOKEN_IF, TOKEN_LPARENT, TOKEN_ID, TOKEN_LT, TOKEN_INT,
                TOKEN_RPARENT, TOKEN_COLON, TOKEN_EOL, TOKEN_ID, TOKEN_ASSIGN,
                TOKEN_INT, TOKEN_EOF};
    run_test("if(x<5):\ny=1", t30, 11);

    int t31[] = {TOKEN_WHILE, TOKEN_TRUE, TOKEN_COLON, TOKEN_EOL, TOKEN_IF, TOKEN_ID, TOKEN_EQ,
                TOKEN_FALSE, TOKEN_COLON, TOKEN_EOL, TOKEN_PRINT, TOKEN_LPARENT, TOKEN_ID, TOKEN_RPARENT,
                TOKEN_EOL, TOKEN_ID, TOKEN_ASSIGN, TOKEN_TRUE, TOKEN_AND, TOKEN_FALSE, TOKEN_EOF};

    run_test("while True:\nif x ==False:\n print(y)\n z=True and False", t31, 21);

    int t32[] = {TOKEN_ERROR, TOKEN_EOL, TOKEN_EOF};

    run_test("123a\n", t32, 3);

    printf("\nAll lexer tests passed!\n");
    return 0;
}
