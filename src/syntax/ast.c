#include<stdlib.h>
#include<Stdio.h>
#include<string.h>
#include "ast.h"

ASTNode *make_number(int x) {
    ASTNumber* node = malloc(sizeof(ASTNumber));

    node->type = AST_NUMBER;
    node->value = x;

    return (ASTNode*)node;
}

ASTNode *make_identifier(char *name) {
    ASTIdentifier* node = malloc(sizeof(ASTIdentifier));

    node->type = AST_IDENTIFIER;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);

    return (ASTNode*)node;
}

ASTNode *make_binaryexp(char op, ASTNode* left, ASTNode *right) {
    ASTBinaryExp* node = malloc(sizeof(ASTBinaryExp));

    node->type = AST_BINARYEXP;
    node->op = op;
    node->left = left;
    node->right = right;

    return (ASTNode*)node;
}

ASTNode *make_func(char *name) {
    ASTFuncDecl *node = malloc(sizeof(ASTFuncDecl));
    node->type = AST_FUNC_DECL;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);
    node->params = NULL;
    node->paramCount = 0;
    node->statements = NULL;
    node->smtCount = 0;
    return (ASTNode*)node;
}

ASTNode *make_call(char *callee) {
    ASTCall *node = malloc(sizeof(ASTCall));
    node->type = AST_CALL;
    node->callee = malloc(strlen(callee) + 1);
    strcpy(node->callee, callee);
    node->args = NULL;
    node->argCount = 0;
    return (ASTNode*)node;
}

ASTNode *make_return(ASTNode *exp) {
    ASTReturn *node = malloc(sizeof(ASTReturn));
    node->type = AST_RETURN;
    node->exp = exp;
    return (ASTNode*)node;
}

ASTNode *make_print_smt(ASTNode* exp) {
    ASTPrint* node = malloc(sizeof(ASTPrint));

    node->type = AST_PRINT;
    node->exp = exp;

    return (ASTNode*)node;
}

ASTNode *make_var(char *name, ASTNode *initializer) {
    ASTVar *node = malloc(sizeof(ASTVar));
    node->type = AST_VAR_DECL;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);
    node->initializer = initializer;
    return (ASTNode*)node;
}

ASTNode *make_assignment(char *name, ASTNode* exp) {
    ASTAssignment* node = malloc(sizeof(ASTAssignment));

    node->type = AST_ASSIGNMENT;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);   
    node->exp = exp;

    return (ASTNode*)node;
}

ASTNode *make_string(char *str) {
    ASTString *node = malloc(sizeof(ASTString));

    node->type = AST_STRING;
    node->str = malloc(strlen(str) + 1);
    strcpy(node->str, str);

    return (ASTNode*)node;
}

ASTNode *make_bool(int x) {
    ASTBool *node = malloc(sizeof(ASTBool));

    node->type = AST_BOOL;
    node->bool_value = x ? 1 : 0;

    return (ASTNode*)node;
}

ASTNode *make_if(ASTNode *condition) {
    ASTIf *node = malloc(sizeof(ASTIf));
    node->type = AST_IF;
    node->condition = condition;
    node->if_statements = NULL;
    node->if_stmt_count = 0;
    node->else_statements = NULL;
    node->else_stmt_count = 0;

    return (ASTNode*)node;
}

ASTNode *make_while(ASTNode *condition) {
    ASTWhile *node = malloc(sizeof(ASTWhile));
    node->type = AST_WHILE;
    node->condition = condition;
    node->while_stmts = NULL;
    node->while_stmt_count = 0;

    return (ASTNode*)node;
}

ASTNode *make_block(void) {
    ASTBlock *block = malloc(sizeof(ASTBlock));
    block->type = AST_BLOCK;
    block->statements = NULL;
    block->smt_count = 0;
    return (ASTNode*)block;
}