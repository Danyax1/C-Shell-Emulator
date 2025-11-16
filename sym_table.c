#include "sym_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Problem with linear probing forces to introduce могили for correct work of del
 */
#define TOMBSTONE ((kv*) -1)


void* my_calloc(int n, int size){
    void* ptr = calloc(n, size);
    if(!ptr) exit(EXIT_FAILURE);
    return ptr;
}

int hash_func(char* str, int table_size){
    int hash_res = 0;
    for(int i = 0; str[i] != '\0'; ++i){
        hash_res = (hash_res * 31 + (int)str[i]) % table_size;
    }
    return hash_res;
}
void resize_table(sym_table* table);

sym_table create_table(void){
    sym_table t;
    t.capacity = 8;     // why 8? because orig Python used 8 for dict
    t.size = 0;
    t.data = my_calloc(t.capacity, sizeof(kv*));
    return t;
}
void free_table(sym_table table)
{    
    if(!table.data) return;
    for(int i = 0; i < table.capacity; ++i){
        kv* entry = table.data[i];
        if(entry != NULL && entry != TOMBSTONE){
            free(entry->key);
            free(entry);
        }
    }
    free(table.data);
}

void set_variable(sym_table* table, char* varName, void* varValue, type varType){
    if (!varName) return;

    float load = (float)table->size / (float)table->capacity;
    if(load > 0.7) resize_table(table);

    int hash = hash_func(varName, table->capacity);
    int first_tombstone = -1;

    for(int i = 0; i < table->capacity; ++i){
        int idx = (hash + i) % table->capacity;
        kv* entry = table->data[idx];

        /* remember first tombstone */
        if(entry == TOMBSTONE && first_tombstone == -1){
            first_tombstone = idx;
        }

        if(entry == NULL){
            int insert_idx = (first_tombstone != -1) ? first_tombstone : idx;

            kv* newkv = my_calloc(1, sizeof(kv));
            newkv->key = strdup(varName);
            newkv->val.valueType = varType;

            if(varType == IntType)
                newkv->val.value.IntVal = *(long long*)varValue;
            else
                newkv->val.value.FloatVal = *(double*)varValue;

            table->data[insert_idx] = newkv;
            table->size++;
            return;
        }

        /* update existing */
        if(entry != TOMBSTONE && strcmp(entry->key, varName) == 0){
            entry->val.valueType = varType;

            if(varType == IntType)
                entry->val.value.IntVal = *(long long*)varValue;
            else
                entry->val.value.FloatVal = *(double*)varValue;

            return;
        }
    }
}

value_record get_variable(sym_table* table, char* varName){
    if(!varName) return (value_record){.valueType = ErrorType};
    int hash = hash_func(varName, table->capacity);

    for(int i = 0; i < table->capacity; ++i){
        int idx = (hash + i) % table->capacity;
        kv* entry = table->data[idx];

        if(entry == NULL)
            return (value_record){.valueType = ErrorType};

        if(entry != TOMBSTONE && strcmp(entry->key, varName) == 0)
            return entry->val;
    }
    return (value_record){.valueType = ErrorType};
}

int del_variable(sym_table* table, char* varName){
    if(!varName) return -1;

    int hash = hash_func(varName, table->capacity);

    for(int i = 0; i < table->capacity; ++i){
        int idx = (hash + i) % table->capacity;
        kv* entry = table->data[idx];
        if(entry == NULL)
            return -1;
        if(entry != TOMBSTONE && strcmp(entry->key, varName) == 0){
            free(entry->key);
            free(entry);
            table->data[idx] = TOMBSTONE;  // important
            table->size--;
            return 0;
        }
    }

    return -1;
}

void resize_table(sym_table* table){
    int old_cap = table->capacity;
    kv** old_data = table->data;

    int new_cap = old_cap * 2;
    kv** new_data = my_calloc(new_cap, sizeof(kv*));

    table->capacity = new_cap;
    table->data = new_data;
    table->size = 0;

    /* reinsert old entries */
    for(int i = 0; i < old_cap; ++i){
        kv* entry = old_data[i];
        if(entry != NULL && entry != TOMBSTONE){
            set_variable(table, entry->key,
                         (entry->val.valueType == IntType ?
                           (void*)&entry->val.value.IntVal :
                           (void*)&entry->val.value.FloatVal),
                         entry->val.valueType);

            free(entry->key);
            free(entry);
        }
    }
    free(old_data);
}
