#include "lexer.h"
#include <gtest/gtest.h>
#include <vector>

static std::vector<int> collect_tokens(const char *src) {
    std::vector<int> types;
    Lexer L;
    L.charToRead = (char *)src;
    L.pos = 0;

    while (1) {
        Token t = next_token(&L);
        types.push_back(t.type);
        if (t.type == TOKEN_EOF)
            break;
    }
    return types;
}

static void expect_tokens(const char *src, const std::vector<int> &expected) {
    auto actual = collect_tokens(src);

    ASSERT_EQ(actual.size(), expected.size()) 
        << "Token count mismatch for input: " << src;

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i]) 
            << "Mismatch at token " << i << " for input: " << src;
    }
}


TEST(LexerTests, BasicArithmetic) {
    expect_tokens("-5 + 3.0", {TOKEN_MINUS, TOKEN_INT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF});
    expect_tokens("10 + 3.14", {TOKEN_INT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF});
}

TEST(LexerTests, Assignment) {
    expect_tokens("x = 42", {TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOF});
}

TEST(LexerTests, Parentheses) {
    expect_tokens("2 * (3 + 4)", {
        TOKEN_INT, TOKEN_MULT, TOKEN_LPARENT, TOKEN_INT,
        TOKEN_PLUS, TOKEN_INT, TOKEN_RPARENT, TOKEN_EOF
    });
}

TEST(LexerTests, IntegerDivisionAndModulo) {
    expect_tokens("8 // 3 % 2", {
        TOKEN_INT, TOKEN_DIV_INT, TOKEN_INT,
        TOKEN_MODULO, TOKEN_INT, TOKEN_EOF
    });
}

TEST(LexerTests, LogicalOperators) {
    expect_tokens("not x and y or z", {
        TOKEN_NOT, TOKEN_ID, TOKEN_AND,
        TOKEN_ID, TOKEN_OR, TOKEN_ID, TOKEN_EOF
    });
}

TEST(LexerTests, WhitespaceAndWeirdSpacing) {
    expect_tokens("   not   x // y5 or _z )   ", {
        TOKEN_NOT, TOKEN_ID, TOKEN_DIV_INT,
        TOKEN_ID, TOKEN_OR, TOKEN_ID, TOKEN_RPARENT, TOKEN_EOF
    });
}

TEST(LexerTests, MixedIdentifiers) {
    expect_tokens("a+3.14", {TOKEN_ID, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF});
    expect_tokens("notnot 1+", {TOKEN_ID, TOKEN_INT, TOKEN_PLUS, TOKEN_EOF});
}

TEST(LexerTests, IfStatements) {
    expect_tokens("if x:", {TOKEN_IF, TOKEN_ID, TOKEN_COLON, TOKEN_EOF});

    expect_tokens(
        "if x:\n"
        "    \n"
        "elif y:", 
        {
            TOKEN_IF, TOKEN_ID, TOKEN_COLON, TOKEN_EOL,
            TOKEN_INDENT, TOKEN_EOL, TOKEN_DEDENT,
            TOKEN_ELIF, TOKEN_ID, TOKEN_COLON, TOKEN_EOF
        }
    );

}

TEST(LexerTests, WhileStatements) {
    expect_tokens("while x < 5:", {
        TOKEN_WHILE, TOKEN_ID, TOKEN_LT,
        TOKEN_INT, TOKEN_COLON, TOKEN_EOF
    });
}

TEST(LexerTests, Comparisons) {
    expect_tokens("3 < 5", {TOKEN_INT, TOKEN_LT, TOKEN_INT, TOKEN_EOF});
    expect_tokens("3 <= 5", {TOKEN_INT, TOKEN_LE, TOKEN_INT, TOKEN_EOF});
    expect_tokens("3 == 3", {TOKEN_INT, TOKEN_EQ, TOKEN_INT, TOKEN_EOF});
    expect_tokens("3 != 2", {TOKEN_INT, TOKEN_NEQ, TOKEN_INT, TOKEN_EOF});
    expect_tokens("10 > 3", {TOKEN_INT, TOKEN_GT, TOKEN_INT, TOKEN_EOF});
    expect_tokens("10 >= 10", {TOKEN_INT, TOKEN_GE, TOKEN_INT, TOKEN_EOF});
}

TEST(LexerTests, OperatorPrecedenceTokens) {
    expect_tokens("a and b or c == d", {
        TOKEN_ID, TOKEN_AND, TOKEN_ID, TOKEN_OR,
        TOKEN_ID, TOKEN_EQ, TOKEN_ID, TOKEN_EOF
    });
}

TEST(LexerTests, KeywordsVsIdentifiers) {
    expect_tokens("ifx = 1", {TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOF});
    expect_tokens("whiley = 2", {TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, TOKEN_EOF});
}

TEST(LexerTests, MultiLineBlock) {
    expect_tokens(
        "if(x<5):\n"
        "    y=1", 
        {
            TOKEN_IF, TOKEN_LPARENT, TOKEN_ID, TOKEN_LT, TOKEN_INT,
            TOKEN_RPARENT, TOKEN_COLON, TOKEN_EOL,
            TOKEN_INDENT,
            TOKEN_ID, TOKEN_ASSIGN, TOKEN_INT, 
            TOKEN_DEDENT,
            TOKEN_EOF
        }
    );
}

TEST(LexerTests, ComplexCase) { 
    expect_tokens(
        "while True:\n"
        "    if x == False:\n"
        "        print(y)\n"
        "z = True and False", 
        { 
            TOKEN_WHILE, TOKEN_TRUE, TOKEN_COLON, TOKEN_EOL, 
            TOKEN_INDENT,
            TOKEN_IF, TOKEN_ID, TOKEN_EQ, TOKEN_FALSE, TOKEN_COLON, TOKEN_EOL, 
            TOKEN_INDENT,
            TOKEN_PRINT, TOKEN_LPARENT, TOKEN_ID, TOKEN_RPARENT, TOKEN_EOL, 
            TOKEN_DEDENT,
            TOKEN_DEDENT,
            TOKEN_ID, TOKEN_ASSIGN, TOKEN_TRUE, TOKEN_AND, TOKEN_FALSE, TOKEN_EOF 
        }
    ); 
}

TEST(LexerTests, ErrorToken) {
    expect_tokens("123a\n", {
        TOKEN_ERROR, TOKEN_EOL, TOKEN_EOF
    });
}
