#include <gtest/gtest.h>
#include "sym_table.h"
#include <string.h>
#include <stdlib.h>


static bool check_int(Value v, long long expected) {
    return v.valueType == VAL_INT && v.value.IntVal == expected;
}

static bool check_float(Value v, double expected) {
    return v.valueType == VAL_FLOAT && v.value.FloatVal == expected;
}

static bool check_error(Value v) {
    return v.valueType == VAL_ERROR;
}


TEST(SymbolTable, InitialState) {
    sym_table tbl = create_table();

    EXPECT_GT(tbl.capacity, 0);
    EXPECT_EQ(tbl.size, 0);
    EXPECT_NE(tbl.data, nullptr);

    free_table(tbl);
}

TEST(SymbolTable, InsertAndGetInteger) {
    sym_table tbl = create_table();

    long long xv = 42;
    set_variable(&tbl, "x", &xv, VAL_INT);

    EXPECT_EQ(tbl.size, 1);

    Value r = get_variable(&tbl, "x");
    EXPECT_TRUE(check_int(r, 42));

    free_table(tbl);
}

TEST(SymbolTable, InsertAndGetFloat) {
    sym_table tbl = create_table();

    double yv = 3.14;
    set_variable(&tbl, "y", &yv, VAL_FLOAT);

    EXPECT_EQ(tbl.size, 1);

    Value r = get_variable(&tbl, "y");
    EXPECT_TRUE(check_float(r, 3.14));

    free_table(tbl);
}

TEST(SymbolTable, OverwriteExistingVariable) {
    sym_table tbl = create_table();

    long long x = 42;
    set_variable(&tbl, "x", &x, VAL_INT);

    long long newx = 77;
    set_variable(&tbl, "x", &newx, VAL_INT);

    EXPECT_EQ(tbl.size, 1);

    Value r = get_variable(&tbl, "x");
    EXPECT_TRUE(check_int(r, 77));

    double newxf = 9.81;
    set_variable(&tbl, "x", &newxf, VAL_FLOAT);

    r = get_variable(&tbl, "x");
    EXPECT_TRUE(check_float(r, 9.81));

    free_table(tbl);
}

TEST(SymbolTable, NonExistentVariable) {
    sym_table tbl = create_table();

    Value r = get_variable(&tbl, "nope");
    EXPECT_TRUE(check_error(r));

    free_table(tbl);
}

TEST(SymbolTable, EmptyStringKey) {
    sym_table tbl = create_table();

    long long z = 100;
    set_variable(&tbl, "", &z, VAL_INT);

    Value r = get_variable(&tbl, "");
    EXPECT_TRUE(check_int(r, 100));

    free_table(tbl);
}

TEST(SymbolTable, NullKeyHandling) {
    sym_table tbl = create_table();

    long long z = 100;

    set_variable(&tbl, NULL, &z, VAL_INT);

    Value r = get_variable(&tbl, NULL);
    EXPECT_TRUE(check_error(r));

    free_table(tbl);
}

TEST(SymbolTable, DeleteVariable) {
    sym_table tbl = create_table();

    double yv = 3.14;
    set_variable(&tbl, "y", &yv, VAL_FLOAT);

    EXPECT_EQ(tbl.size, 1);

    int del = del_variable(&tbl, "y");
    EXPECT_EQ(del, 0);


    Value r = get_variable(&tbl, "y");
    EXPECT_TRUE(check_error(r));

    del = del_variable(&tbl, "y");
    EXPECT_EQ(del, -1);

    del = del_variable(&tbl, NULL);
    EXPECT_EQ(del, -1);

    free_table(tbl);
}

TEST(SymbolTable, TypeChangeOverwrite) {
    sym_table tbl = create_table();

    double xf = 1.23;
    set_variable(&tbl, "x", &xf, VAL_FLOAT);

    long long xi = -999;
    set_variable(&tbl, "x", &xi, VAL_INT);

    Value r = get_variable(&tbl, "x");
    EXPECT_TRUE(check_int(r, -999));

    free_table(tbl);
}

TEST(SymbolTable, LargeValues) {
    sym_table tbl = create_table();

    long long bigInt = __LONG_LONG_MAX__;
    set_variable(&tbl, "imax", &bigInt, VAL_INT);

    Value r = get_variable(&tbl, "imax");
    EXPECT_TRUE(check_int(r, bigInt));

    double bigF = 1e308;
    set_variable(&tbl, "fmax", &bigF, VAL_FLOAT);

    r = get_variable(&tbl, "fmax");
    EXPECT_TRUE(check_float(r, bigF));

    free_table(tbl);
}

TEST(SymbolTable, VeryLongKey) {
    sym_table tbl = create_table();

    char *longkey = (char *)malloc(50000);
    memset(longkey, 'a', 49999);
    longkey[49999] = '\0';

    long long v = 999;
    set_variable(&tbl, longkey, &v, VAL_INT);

    Value r = get_variable(&tbl, longkey);
    EXPECT_TRUE(check_int(r, 999));

    free(longkey);
    free_table(tbl);
}
