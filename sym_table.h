#ifndef SYM_TABLE_SHELL_
#define SYM_TABLE_SHELL_

typedef enum {
    IntType,
    FloatType,
    ErrorType       // type of entry, that doesn't exist
}type;

typedef struct{
    type valueType;
    union {
        long long IntVal;
        double FloatVal;
    }value;
}value_record;

typedef struct 
{
    char* key;
    value_record val;
    
}kv;

typedef struct 
{
    int capacity;
    int size;
    kv** data;
}sym_table;

sym_table create_table(void);
void      free_table(sym_table table);

void         set_variable(sym_table* table, char* varName, void* varValue, type varType);
value_record get_variable(sym_table* table, char* varName);
// 0 = success; -1 = var doesn't exist
int          del_variable(sym_table* table, char* varName);

#endif //SYM_TABLE_SHELL_