#ifndef SYM_TABLE_SHELL_
#define SYM_TABLE_SHELL_

#include <stdbool.h>

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_NONE,
    VAL_ERROR
} ValueType;

typedef struct{
    ValueType valueType;
    union {
        long long   IntVal;
        double      FloatVal;
        bool        BoolVal;
    }value;
}Value;

typedef struct 
{
    char* key;
    Value val;
    
}kv;

typedef struct 
{
    int capacity;
    int size;
    kv** data;
}sym_table;

sym_table create_table(void);
void      free_table(sym_table table);

void         set_variable(sym_table* table, char* varName, void* varValue, ValueType varType);
Value get_variable(sym_table* table, char* varName);
// 0 = success; -1 = var doesn't exist
int          del_variable(sym_table* table, char* varName);

#endif //SYM_TABLE_SHELL_