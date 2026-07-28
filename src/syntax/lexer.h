#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,

    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,

    TOKEN_VAR,
    TOKEN_ASSIGN,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTE,
    TOKEN_GTE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_FN,
    TOKEN_RETURN,
    TOKEN_COMMA,

    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_ELSE,
    
    TOKEN_WHILE,
    TOKEN_BREAK,
    TOKEN_CONTINUE,

    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_STRING,

    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
} Token;

void init_lexer(const char *source);
Token get_token(void);
int convert_lexeme(const char *str);
#endif