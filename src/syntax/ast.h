#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_VAR_DECL,
    AST_ASSIGNMENT,
    AST_FUNC_DECL,
    AST_CALL,
    AST_RETURN,
    AST_PRINT,
    AST_IF,
    AST_WHILE,
    AST_BREAK,
    AST_CONTINUE,
    AST_STRING,
    AST_BINARYEXP,
    AST_IDENTIFIER,
    AST_NUMBER,  
    AST_BOOL, 
    AST_BLOCK 
} ASTNodeType;

typedef struct {
    ASTNodeType type;
} ASTNode;

typedef struct {
    ASTNodeType type;
    ASTNode **statements;
    int smt_count;
} ASTProgram;

typedef struct {
    ASTNodeType type;
    char *name;
    ASTNode *initializer;
} ASTVar;

typedef struct {
    ASTNodeType type;
    char *name;
    ASTNode* exp;
} ASTAssignment;

typedef struct {
    ASTNodeType type;
    char *name;
    char **params;
    int paramCount;
    ASTNode **statements;
    int smtCount;
} ASTFuncDecl;

typedef struct {
    ASTNodeType type;
    char *callee;
    ASTNode **args;
    int argCount;
} ASTCall;

typedef struct {
    ASTNodeType type;
    ASTNode *exp;
} ASTReturn;

typedef struct {
    ASTNodeType type;
    ASTNode *exp;
} ASTPrint; 

typedef struct {
    ASTNodeType type;
    char *str;
} ASTString; 

typedef struct {
    ASTNodeType type;
    ASTNode *condition;

    ASTNode **if_statements;
    int if_stmt_count;

    ASTNode **else_statements;
    int else_stmt_count;
} ASTIf;

typedef struct {
    ASTNodeType type;
    ASTNode *condition;

    ASTNode **while_stmts;
    int while_stmt_count;
} ASTWhile;

typedef struct {
    ASTNodeType type;
    char op;
    ASTNode *left;
    ASTNode *right;
} ASTBinaryExp;

typedef struct {
    ASTNodeType type;
    char *name;
} ASTIdentifier;

typedef struct {
    ASTNodeType type;
    int value;
} ASTNumber;

typedef struct {
    ASTNodeType type;
    int bool_value;
} ASTBool;

typedef struct {
    ASTNodeType type;
    ASTNode **statements;
    int smt_count;
} ASTBlock;

ASTNode *make_number(int x);
ASTNode *make_identifier(char *name);
ASTNode *make_binaryexp(char op, ASTNode* left, ASTNode* right);
ASTNode *make_func(char *name);
ASTNode *make_call(char *callee);
ASTNode *make_return(ASTNode *exp);
ASTNode *make_print_smt(ASTNode* exp);
ASTNode *make_var(char *name, ASTNode *intializer);
ASTNode *make_assignment(char *name, ASTNode* exp);
ASTNode *make_string(char *str);
ASTNode *make_bool(int x);
ASTNode *make_if(ASTNode *condition);
ASTNode *make_while(ASTNode *condition);
ASTNode *make_block(void);

//for testing purpose
void print_ast(ASTNode* node, int level);
#endif