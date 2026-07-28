#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include "lexer.h"

static const char *source;
static int position;

void init_lexer(const char *src) {
    source = src;
    position = 0;
}
static char peek() {
    return source[position];
}

static char advance() {
    return source[position++];
}

static void skip_whitespace() {
    while(isspace(peek())) {
        advance();
    }
}

static int end_reached() {
    return source[position] == '\0';
}

static Token get_identifier() {
    int start = position;
    while(isalnum(peek())) {
        advance();
    }

    int length = position - start;
    char *text = malloc(length + 1);
    for(int i = 0; i < length; i++) {
        text[i] = source[start + i];
    }

    text[length] = '\0';
    Token token;
    token.lexeme = text;
    if(strcmp(text, "print") == 0) {
        token.type = TOKEN_PRINT;
    } else if(strcmp(text, "true") == 0) {
        token.type = TOKEN_TRUE;
    } else if(strcmp(text, "false") == 0) {
        token.type = TOKEN_FALSE;
    } else if(strcmp(text, "if") == 0) {
        token.type = TOKEN_IF;
    } else if(strcmp(text, "else") == 0) {
        token.type = TOKEN_ELSE;
    } else if(strcmp(text, "while") == 0) {
        token.type = TOKEN_WHILE;
    } else if(strcmp(text, "break") == 0) {
        token.type = TOKEN_BREAK;
    } else if(strcmp(text, "continue") == 0) {
        token.type = TOKEN_CONTINUE; 
    } else if(strcmp(text, "var") == 0) {
        token.type = TOKEN_VAR; 
    } else if(strcmp(text, "fn") == 0) {
        token.type = TOKEN_FN; 
    } else if(strcmp(text, "return") == 0) {
        token.type = TOKEN_RETURN; 
    } else {
        token.type = TOKEN_IDENTIFIER;
    }
    return token;
}

static Token get_number() {
    int start = position;
    while(isdigit(peek())) {
        advance();
    }

    int length = position - start;
    char *text = malloc(length + 1);

    for(int i = 0; i < length; i++) {
        text[i] = source[start + i];
    }

    text[length] = '\0';
    Token token;
    token.type = TOKEN_NUMBER;
    token.lexeme = text;

    return token;
}

static char *get_string() {
    int start = position;
    while(peek() != '"' && !end_reached() && peek() != '\n') {
        advance();
    }

    if(peek() == '\n') {
        printf("Syntax Error: unterminated string literal due to newline");
        exit(1);
    }
    
    if(end_reached()) {
        printf("Syntax Error: end reached due to unterminated string literal");
        exit(1);
    }
    int length = position - start;
    char *str = malloc(length + 1);
    
    for(int i = 0; i < length; i++) {
        str[i] = source[start + i];
    }

    str[length] = '\0';
    if(peek() == '"') advance();
    return str;
}
int convert_lexeme(const char *str) {
    int res = 0;
    int sign = 1;
    while(*str == ' ' || *str == '\n' || *str == '\t') {
        str++;
    }
    if(*str == '-') {
        sign = -1;
        str++;
    }
    while(*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res * sign;
}

Token get_token() {
    skip_whitespace();
    Token token;
    if(end_reached()) {
        token.type = TOKEN_EOF;
        token.lexeme = NULL;
        return token;
    }
    if(isalpha(peek())) {
        return get_identifier();
    }
    if(isdigit(peek())) {
        return get_number();
    }
    char c = advance();
    token.lexeme = NULL;
    switch(c) {
        case '+': token.type = TOKEN_PLUS; break;
        case '-': token.type = TOKEN_MINUS; break;
        case '*': token.type = TOKEN_STAR; break;
        case '/': token.type = TOKEN_SLASH; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case ',': token.type = TOKEN_COMMA; break;
        case '=': 
            if(peek() == '=') {
                advance();
                token.type = TOKEN_EQ; 
            } else {
                token.type = TOKEN_ASSIGN; 
            } 
            break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case '!': 
            if(peek() == '=') {
                advance();
                token.type = TOKEN_NEQ;
            } else {
                printf("Lexer error: Unexpected character %c\n", c);
                exit(1);
            }
            break;
        case '"': { 
            token.type = TOKEN_STRING; 
            token.lexeme = get_string();
            break; 
        }
        case '&': {
            if(peek() == '&') {
                advance();
                token.type = TOKEN_AND;
            } else {
                printf("Lexer error: Unexpected character %c\n", c);
                exit(1);
            }
            break;
        }
        case '|': {
            if(peek() == '|') {
                advance();
                token.type = TOKEN_OR;
            } else {
                printf("Lexer error: Unexpected character %c\n", c);
                exit(1);
            }
            break;
        }
        case '<': 
            if(peek() == '=') {
                advance();
                token.type = TOKEN_LTE;
            } else {
                token.type = TOKEN_LT;
            }
            break;
        case '>': 
            if(peek() == '=') {
                advance();
                token.type = TOKEN_GTE;
            } else {
                token.type = TOKEN_GT;
            }
            break;
        default: 
            printf("Lexer error: Unexpected character %c\n", c);
            exit(1);
    }

    return token;
}
