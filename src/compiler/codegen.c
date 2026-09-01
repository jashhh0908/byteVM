#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runtime/chunk.h"
#include "syntax/ast.h"
#include "compiler/symbol_table.h"

typedef struct LoopState{ 
    int loopStart; //store starting point of the loop
    int localsAtLoopStart; //stores the amount of local variables that were present on the VM before loop began
    int *breakJumps; //array to store instruction index of each break
    int breakCount;
    int breakCapacity; //used to manage memory of dynamic array *breakJumps 
    struct LoopState *parent; //pointer to outer loop/scope
} LoopState;

typedef struct CompileState {
    SymbolTable *currentScope;
    LoopState *currentLoop;
    int nextLocalSlot; //track next slot to assign incoming local variables
    int globalCount; //track next slot for global variables
} CompileState;

char *opCodeName(Instruction code) {
    switch(code.opcode) {
        case OP_CONST: return "OP_CONST";
        case OP_ADD: return "OP_ADD";
        case OP_SUB: return "OP_SUB";
        case OP_DIV: return "OP_DIV";
        case OP_MUL: return "OP_MUL";
        case OP_LT: return "OP_LT";
        case OP_GT: return "OP_GT";
        case OP_GTE: return "OP_GTE";
        case OP_LTE: return "OP_LTE";
        case OP_EQ: return "OP_EQ";
        case OP_NEQ: return "OP_NEQ";
        case OP_LOAD: return "OP_LOAD";
        case OP_STORE: return "OP_STORE";
        case OP_LOAD_LOCAL: return "OP_LOAD_LOCAL";
        case OP_STORE_LOCAL: return "OP_STORE_LOCAL";
        case OP_POP: return "OP_POP";    
        case OP_JUMP: return "OP_JUMP";
        case OP_JUMP_IF_FALSE: return "OP_JUMP_IF_FALSE";
        case OP_PRINT: return "OP_PRINT";
        case OP_HALT: return "OP_HALT";
        default: printf("Code yet to put"); exit(1);
    }
}

int emitJump(Chunk *chunk, OpCode opcode) {
    return emitInstruction(chunk,opcode, -1);
}

void jumpOffset(Chunk *chunk, int jumpIndex) {
    int offset = chunk->count - jumpIndex - 1;
    chunk->code[jumpIndex].operand = offset;
}

int emitLoop(Chunk *chunk, OpCode opcode, int loopStart) {
    int offset = chunk->count - loopStart + 1;
    return emitInstruction(chunk, opcode, -offset);
}

void compileNode(ASTNode *node, Chunk *chunk, CompileState *compiler ) {
    switch (node->type) {
    case AST_NUMBER: {
        ASTNumber *num = (ASTNumber*)node;

        Value v;
        v.type = VALUE_INT;
        v.val = num->value;

        int index = addConstant(v, chunk);
        emitInstruction(chunk, OP_CONST, index);
        break;        
    }
    
    case AST_BOOL: {
        ASTBool *boolNode = (ASTBool*)node;

        Value v;
        v.type = VALUE_BOOL;
        v.val = boolNode->bool_value;

        int index = addConstant(v, chunk);
        emitInstruction(chunk, OP_CONST, index);
        break;
    }

    case AST_STRING: {
        ASTString *stringNode = (ASTString*)node;

        Value v;
        v.type = VALUE_STRING;
        v.str = stringNode->str;

        int index = addConstant(v, chunk);
        emitInstruction(chunk, OP_CONST, index);
        break;
    }

    case AST_VAR_DECL: {
        ASTVar *var = (ASTVar*)node;
        Symbol *temp = compiler->currentScope->head;
        //check for redeclaration
        while(temp != NULL) {
            if(strcmp(temp->name, var->name) == 0) {
                printf("Error: Redeclaration of variable '%s'", var->name);
                exit(1);
            }
            temp = temp->next;
        }
        //check if assignment or declaration
        if(var->initializer != NULL) {
            compileNode(var->initializer, chunk, compiler);
        } else {
            Value v = value_int(0);
            int index = addConstant(v, chunk);
            emitInstruction(chunk, OP_CONST, index);
        }
        if(compiler->currentScope->depth > 0) {
            int slot = compiler->nextLocalSlot++;
            symbolTable_add(compiler->currentScope, var->name, SYMBOL_LOCAL, slot);
        } else if(compiler->currentScope->depth == 0) {
            int slot = compiler->globalCount++;
            symbolTable_add(compiler->currentScope, var->name, SYMBOL_GLOBAL, slot);
            emitInstruction(chunk, OP_STORE, slot);
        }
        break;
    }

    case AST_ASSIGNMENT: {
        ASTAssignment *assign = (ASTAssignment*)node;
        compileNode(assign->exp, chunk, compiler);

        //find if the variable being assigned to has already been declared
        Symbol *symbol = symbolTable_find(compiler->currentScope, assign->name);
        
        //variable already exists
        if(symbol != NULL) {
            if(symbol->type == SYMBOL_GLOBAL) emitInstruction(chunk, OP_STORE, symbol->slot);
            if(symbol->type == SYMBOL_LOCAL) emitInstruction(chunk, OP_STORE_LOCAL, symbol->slot);
        } else if (symbol == NULL){
            printf("Error: undefined variable '%s'", assign->name);
            exit(1);
        }
        break; 
    }

    case AST_IDENTIFIER: {
        ASTIdentifier *idn = (ASTIdentifier*)node;
        Symbol *symbol = symbolTable_find(compiler->currentScope, idn->name);
        if(symbol == NULL) {
            printf("Error: Undefined Variable '%s'\n", idn->name);
            exit(1);
        }
        if(symbol->type == SYMBOL_GLOBAL) emitInstruction(chunk, OP_LOAD, symbol->slot);
        if(symbol->type == SYMBOL_LOCAL) emitInstruction(chunk, OP_LOAD_LOCAL, symbol->slot);
        break;
    }

    case AST_BINARYEXP: {
        ASTBinaryExp *bexp = (ASTBinaryExp*)node;
        compileNode(bexp->left, chunk, compiler);
        compileNode(bexp->right, chunk, compiler);
        switch(bexp->op) {
            case '+': emitInstruction(chunk, OP_ADD, 0); break;
            case '-': emitInstruction(chunk, OP_SUB, 0); break;
            case '*': emitInstruction(chunk, OP_MUL, 0); break;
            case '/': emitInstruction(chunk, OP_DIV, 0); break;
            case '>': emitInstruction(chunk, OP_GT, 0); break;
            case '<': emitInstruction(chunk, OP_LT, 0); break;
            case 'g': emitInstruction(chunk, OP_GTE, 0); break;
            case 'l': emitInstruction(chunk, OP_LTE, 0); break;
            case '=': emitInstruction(chunk, OP_EQ, 0); break;
            case '!': emitInstruction(chunk, OP_NEQ, 0); break;
            default: printf("Compiling Error: Unknown binary operation '%c' found\n", bexp->op); exit(1); 
        }
        break;
    }
    
    case AST_IF: {
        ASTIf *ifNode = (ASTIf*)node;
        compileNode(ifNode->condition, chunk, compiler);
        int jumpIfFalse = emitJump(chunk, OP_JUMP_IF_FALSE);
        //get current local slot
        int ifSlot = compiler->nextLocalSlot;
        //build a new scope for this if block with the parent as the currentScope
        compiler->currentScope = symbolTable_create(compiler->currentScope, compiler->currentScope->depth + 1);
        for(int i = 0; i < ifNode->if_stmt_count; i++){
            compileNode(ifNode->if_statements[i], chunk, compiler);
        }
        //after compiling all statements, pop all local variables of the if block
        int ifLocalsToPop = compiler->nextLocalSlot - ifSlot;
        for(int i = 0; i < ifLocalsToPop; i++) {
            emitInstruction(chunk, OP_POP, 0);
        }
        compiler->nextLocalSlot = ifSlot;
        //free the current scope once compiled
        SymbolTable *ifScope = compiler->currentScope;
        compiler->currentScope = ifScope->parent;
        symbolTable_free(ifScope);

        int jumpToEnd = emitJump(chunk, OP_JUMP);
        jumpOffset(chunk, jumpIfFalse);

        if(ifNode->else_stmt_count > 0) {
            int elseSlot = compiler->nextLocalSlot;
            compiler->currentScope = symbolTable_create(compiler->currentScope, compiler->currentScope->depth + 1);
            for(int i = 0; i < ifNode->else_stmt_count; i++){
                compileNode(ifNode->else_statements[i], chunk, compiler);
            }
            int elseLocalsToPop = compiler->nextLocalSlot - elseSlot;
            for(int i = 0; i < elseLocalsToPop; i++) {
                emitInstruction(chunk, OP_POP, 0);
            }
            compiler->nextLocalSlot = elseSlot;
            //free the current scope once compiled
            SymbolTable *elseScope = compiler->currentScope;
            compiler->currentScope = elseScope->parent;
            symbolTable_free(elseScope);
        }
        jumpOffset(chunk, jumpToEnd);
        break;
    }

    case AST_WHILE: {
        ASTWhile *loop = (ASTWhile*)node;
        int loopStart = chunk->count;       

        LoopState loopState;
        loopState.loopStart = loopStart;
        loopState.localsAtLoopStart = compiler->nextLocalSlot;
        loopState.breakJumps = NULL;
        loopState.breakCount = 0;
        loopState.breakCapacity = 0;
        loopState.parent = compiler->currentLoop;
        compiler->currentLoop = &loopState;

        compileNode(loop->condition, chunk, compiler);
        int jumpIfFalse = emitJump(chunk, OP_JUMP_IF_FALSE);
        int whileSlot = compiler->nextLocalSlot;
        compiler->currentScope = symbolTable_create(compiler->currentScope, compiler->currentScope->depth + 1);
        for(int i = 0; i < loop->while_stmt_count; i++) {
            compileNode(loop->while_stmts[i], chunk, compiler);
        }
        int whileLocalsToPop = compiler->nextLocalSlot - whileSlot;
        for(int i = 0; i < whileLocalsToPop; i++) {
            emitInstruction(chunk, OP_POP, 0);
        }
        compiler->nextLocalSlot = whileSlot;
        //free the current scope once compiled
        SymbolTable *whileScope = compiler->currentScope;
        compiler->currentScope = whileScope->parent;
        symbolTable_free(whileScope);
        emitLoop(chunk, OP_JUMP, loopStart);
        jumpOffset(chunk, jumpIfFalse);
        for(int i = 0; i < loopState.breakCount; i++) {
            jumpOffset(chunk, loopState.breakJumps[i]);
        }
        free(loopState.breakJumps);
        compiler->currentLoop = loopState.parent;
        break;
    }

    case AST_BREAK: {
        if (compiler->currentLoop == NULL) {
            printf("Error: break outside loop\n");
            exit(1);
        }
        // Pop all local variables created since the loop started
        int localsToPop = compiler->nextLocalSlot - compiler->currentLoop->localsAtLoopStart;
        for (int i = 0; i < localsToPop; i++) {
            emitInstruction(chunk, OP_POP, 0);
        }
        // Emit the forward jump instruction
        int breakJump = emitJump(chunk, OP_JUMP);
        // Append breakJump index to the active loop's dynamic array
        LoopState *currentLoop = compiler->currentLoop;
        if (currentLoop->breakCount >= currentLoop->breakCapacity) {
            int newCapacity = currentLoop->breakCapacity == 0 ? 8 : currentLoop->breakCapacity * 2;
            currentLoop->breakJumps = realloc(currentLoop->breakJumps, sizeof(int) * newCapacity);
            currentLoop->breakCapacity = newCapacity;
        }
        currentLoop->breakJumps[currentLoop->breakCount++] = breakJump;
        break;
    }

    case AST_CONTINUE: {
        if (compiler->currentLoop == NULL) {
            printf("Error: break outside loop\n");
            exit(1);
        }
        int localsToPop = compiler->nextLocalSlot - compiler->currentLoop->localsAtLoopStart;
        for (int i = 0; i < localsToPop; i++) {
            emitInstruction(chunk, OP_POP, 0);
        }
        emitLoop(chunk, OP_JUMP, compiler->currentLoop->loopStart);
        break;
    }

    case AST_PRINT: {
        ASTPrint *print = (ASTPrint*)node;
        compileNode(print->exp, chunk, compiler);
        emitInstruction(chunk, OP_PRINT, 0);
        break;
    }

    case AST_BLOCK: {
        int blockSlot = compiler->nextLocalSlot;
        compiler->currentScope = symbolTable_create(compiler->currentScope, compiler->currentScope->depth + 1);
        ASTBlock *block = (ASTBlock*)node;
        for(int i = 0; i < block->smt_count; i++) {
            compileNode(block->statements[i], chunk, compiler);
        }
        int blockLocalsToPop = compiler->nextLocalSlot - blockSlot;
        for(int i = 0; i < blockLocalsToPop; i++) {
            emitInstruction(chunk, OP_POP, 0);
        }
        compiler->nextLocalSlot = blockSlot;
        SymbolTable *blockScope = compiler->currentScope;
        compiler->currentScope = blockScope->parent;
        symbolTable_free(blockScope);
        break;
    }
    
    case AST_FUNC_DECL: {
        ASTFuncDecl *func = (ASTFuncDecl*)node;
        
        ObjFunction *objFunc = malloc(sizeof(ObjFunction));
        objFunc->name = malloc(strlen(func->name) + 1);
        strcpy(objFunc->name, func->name);
        objFunc->argCount = func->paramCount;
        initChunk(&objFunc->chunk);

        int parentLocalSlot = compiler->nextLocalSlot;
        LoopState *parentLoop = compiler->currentLoop;
        
        compiler->nextLocalSlot = 1; 
        compiler->currentLoop = NULL; 

        SymbolTable *globalScope = compiler->currentScope;
        while (globalScope != NULL && globalScope->depth > 0) {
            globalScope = globalScope->parent;
        }

        int globalSlot = compiler->globalCount++;
        symbolTable_add(globalScope, func->name, SYMBOL_GLOBAL, globalSlot);

        SymbolTable *funcScope = symbolTable_create(globalScope, 1);
        SymbolTable *parentScope = compiler->currentScope;
        compiler->currentScope = funcScope;

        for (int i = 0; i < func->paramCount; i++) {
            int slot = compiler->nextLocalSlot++;
            symbolTable_add(funcScope, func->params[i], SYMBOL_LOCAL, slot);
        }

        for (int i = 0; i < func->smtCount; i++) {
            compileNode(func->statements[i], &objFunc->chunk, compiler);
        }

        int retIdx = addConstant(value_int(0), &objFunc->chunk);
        emitInstruction(&objFunc->chunk, OP_CONST, retIdx);
        emitInstruction(&objFunc->chunk, OP_RETURN, 0);

        compiler->currentScope = parentScope;
        compiler->nextLocalSlot = parentLocalSlot;
        compiler->currentLoop = parentLoop;
        symbolTable_free(funcScope);

        Value val;
        val.type = VALUE_FUNC;
        val.func = objFunc;
        int constIdx = addConstant(val, chunk);
        
        emitInstruction(chunk, OP_CONST, constIdx);
        emitInstruction(chunk, OP_STORE, globalSlot);
        break;
    }

    case AST_CALL: {
        ASTCall *call = (ASTCall*)node;
        
        Symbol *symbol = symbolTable_find(compiler->currentScope, call->callee);
        if (symbol == NULL) {
            printf("Error: Undefined function '%s'\n", call->callee);
            exit(1);
        }
        
        if (symbol->type == SYMBOL_GLOBAL) {
            emitInstruction(chunk, OP_LOAD, symbol->slot);
        } else if (symbol->type == SYMBOL_LOCAL) {
            emitInstruction(chunk, OP_LOAD_LOCAL, symbol->slot);
        }

        for (int i = 0; i < call->argCount; i++) {
            compileNode(call->args[i], chunk, compiler);
        }
        
        emitInstruction(chunk, OP_CALL, call->argCount);
        break;
    }

    case AST_RETURN: {
        ASTReturn *ret = (ASTReturn*)node;
        if (ret->exp != NULL) {
            compileNode(ret->exp, chunk, compiler);
        } else {
            int index = addConstant(value_int(0), chunk);
            emitInstruction(chunk, OP_CONST, index);
        }
        emitInstruction(chunk, OP_RETURN, 0);
        break;
    }

    case AST_PROGRAM: {
        ASTProgram *root = (ASTProgram*)node;
        for(int i = 0; i < root->smt_count; i++) {
            compileNode(root->statements[i], chunk, compiler);
        }
        break;
    }
    default: printf("Unknown node type encountered in codegen\n: %d", node->type); exit(1);
    }
}

void initCompileState(CompileState *compiler) {
    compiler->currentScope = symbolTable_create(NULL, 0);
    compiler->currentLoop = NULL;
    compiler->globalCount = 0;
    compiler->nextLocalSlot = 0;
}
//main compilation
void compile(ASTNode *root, Chunk *chunk) {
    CompileState compiler;
    initCompileState(&compiler);
    initChunk(chunk);
    compileNode(root, chunk, &compiler);
    emitInstruction(chunk, OP_HALT, 0);
}

//to debug
void debug_compile(ASTNode *root) {
    Chunk chunk;
    compile(root, &chunk);    
    printf("Constants:\n");
    for (int i = 0; i < chunk.constants.count; i++) {
        printf("  [%d] = %d\n", i, chunk.constants.values[i].val);
    }
    printf("Instructions:\n");
    for (int i = 0; i < chunk.count; i++) {
        printf("  %s operand = %d\n",
               opCodeName(chunk.code[i]),
               chunk.code[i].operand);
    }
}