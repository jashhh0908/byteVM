#ifndef VM_H
#define VM_H

#include<stdint.h>
#include "chunk.h"
#define SIZE 256
#define FRAMES_MAX 64

typedef struct {
    Instruction *ip; //pointer to the first instruction of the active function's chunk
    ObjFunction *func; //pointer to the function object being called
    int slots; //index for to store the function's local variables inside the VM stack
} CallFrame;

typedef struct {
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    Value stack[SIZE];
    Value *top;  
    Value globals[SIZE];
} VM;

void initVM(VM *vm, Chunk *chunk);
void run(VM *vm);
#endif