#ifndef VALUE_H
#define VALUE_H

typedef struct ObjFunction ObjFunction;

typedef enum {
    VALUE_INT,
    VALUE_STRING,
    VALUE_BOOL,
    VALUE_FUNC
} ValueType;

typedef struct {
    ValueType type;
    union {
        int val;
        char *str;
        int bool_val;
        ObjFunction* func;
    };
} Value;

Value value_int(int x);
Value value_string(const char *str);
Value value_bool(int b);
Value value_copy(const Value *v);
void free_value(Value *v);

#endif