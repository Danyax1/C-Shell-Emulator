#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sym_table.h"

/* Utility macro to print test result */
#define TEST(name, cond) \
    do { \
        if (cond) printf("[PASS] %s\n", name); \
        else      printf("[[[FAIL]]] %s\n", name); \
    } while (0)

/* Helper to check returned value_record */
int check_int(value_record v, long long expected) {
    return v.valueType == IntType && v.value.IntVal == expected;
}

int check_float(value_record v, double expected) {
    return v.valueType == FloatType && v.value.FloatVal == expected;
}

int check_error(value_record v) {
    return v.valueType == ErrorType;
}

int main(void) {

    printf("===== SYMBOL TABLE TEST =====\n\n");

    sym_table tbl = create_table();

    TEST("Table initial capacity > 0", tbl.capacity > 0);
    TEST("Table initial size == 0", tbl.size == 0);
    TEST("Table data != NULL", tbl.data != NULL);

    long long xv = 42;
    set_variable(&tbl, "x", &xv, IntType);

    TEST("Insert 'x' integer increases size",
         tbl.size == 1);

    value_record r = get_variable(&tbl, "x");
    TEST("Retrieve 'x' integer correct",
         check_int(r, 42));

    double yv = 3.14;
    set_variable(&tbl, "y", &yv, FloatType);
    TEST("Insert 'y' increases size == 2", tbl.size == 2);
    r = get_variable(&tbl, "y");
    TEST("Retrieve 'y' float correct", check_float(r, 3.14));

    long long newx = 77;
    set_variable(&tbl, "x", &newx, IntType);
    TEST("Overwrite 'x' keeps size == 2", tbl.size == 2);
    r = get_variable(&tbl, "x");
    TEST("Overwrite 'x' new value correct", check_int(r, 77));

    double newxf = 9.81;
    set_variable(&tbl, "x", &newxf, FloatType);
    r = get_variable(&tbl, "x");
    TEST("Overwrite 'x' from int->float type change", check_float(r, 9.81));

    r = get_variable(&tbl, "nope");
    TEST("Retrieving non-existent variable returns ErrorType", check_error(r));

    long long z = 100;
    set_variable(&tbl, "", &z, IntType);
    r = get_variable(&tbl, "");
    TEST("Empty string key stored/retrieved", r.valueType == IntType && r.value.IntVal == 100);

    set_variable(&tbl, NULL, &z, IntType);
    r = get_variable(&tbl, NULL);
    TEST("NULL key does not crash and returns ErrorType", check_error(r));

    int del = del_variable(&tbl, "y");
    TEST("Delete 'y' returns 0", del == 0);
    TEST("After delete size == 2", tbl.size == 2);

    r = get_variable(&tbl, "y");
    TEST("Retrieving deleted 'y' yields ErrorType", check_error(r));

    del = del_variable(&tbl, "y");
    TEST("Delete non-existent variable returns -1", del == -1);

    del = del_variable(&tbl, NULL);
    TEST("Delete NULL key returns -1", del == -1);

    long long intx = -999;
    set_variable(&tbl, "x", &intx, IntType);
    r = get_variable(&tbl, "x");
    TEST("Overwrite x float->int correct", check_int(r, -999));

    long long bigInt = __LONG_LONG_MAX__;        
    set_variable(&tbl, "imax", &bigInt, IntType);
    r = get_variable(&tbl, "imax");
    TEST("Store LLONG_MAX", check_int(r, bigInt));

    double bigF = 1e308;                       
    set_variable(&tbl, "fmax", &bigF, FloatType);
    r = get_variable(&tbl, "fmax");
    TEST("Store large double", check_float(r, bigF));

    char *longkey = malloc(50000);
    memset(longkey, 'a', 49999);
    longkey[49999] = '\0';

    long long v999 = 999;
    set_variable(&tbl, longkey, &v999, IntType);
    r = get_variable(&tbl, longkey);
    TEST("Very long key stored and retrieved",
         r.valueType == IntType && r.value.IntVal == 999);

    free(longkey);

    free_table(tbl);

    printf("\n===== TEST COMPLETE =====\n");
    return 0;
}
