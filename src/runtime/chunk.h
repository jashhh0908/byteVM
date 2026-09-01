#ifndef CHUNK_H
#define CHUNK_H
#include "bytecode.h"
#include "value.h"
typedef struct {
    OpCode opcode;
    int operand;
} Instruction;

typedef struct {
    Value *values;
    int count;
    int capacity;
} ConstantPool;


typedef struct {
    Instruction *code;
    int count;
    int capacity;
    ConstantPool constants;
} Chunk;

struct ObjFunction {
    char *name; //name of the function
    int argCount; //no of parameters
    Chunk chunk; //allocate a separate chunk for instruction list of the function
};

void initChunk(Chunk *chunk);
int addConstant(Value v, Chunk *chunk);
int emitInstruction(Chunk *chunk, OpCode opcode, int operand);
#endif