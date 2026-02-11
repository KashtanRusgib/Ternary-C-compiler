#ifndef PARSER_H
#define PARSER_H

/*
 * Token types for the ternary compiler lexer.
 * Phase 1: Arithmetic expressions
 * Phase 2: Keywords (for, while), identifiers, delimiters
 */
typedef enum {
    /* Literals */
    TOK_INT,        /* Integer literal (value in .value) */

    /* Operators */
    TOK_PLUS,       /* + */
    TOK_MUL,        /* * */
    TOK_PLUS_PLUS,  /* ++ */
    TOK_EQ,         /* = */
    TOK_LT,         /* < */

    /* Delimiters */
    TOK_LPAREN,     /* ( */
    TOK_RPAREN,     /* ) */
    TOK_LBRACE,     /* { */
    TOK_RBRACE,     /* } */
    TOK_SEMI,       /* ; */

    /* Keywords */
    TOK_FOR,        /* for */
    TOK_WHILE,      /* while */
    TOK_INT_KW,     /* int (keyword, distinct from TOK_INT literal) */

    /* Identifiers */
    TOK_IDENT,      /* identifier */

    /* Special */
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    int value;
} Token;

#define MAX_TOKENS 100

extern Token tokens[MAX_TOKENS];
extern int token_idx;

void tokenize(const char *source);
void parse(void);

#endif
