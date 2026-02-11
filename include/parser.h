#ifndef PARSER_H
#define PARSER_H

// Simple tokenizer (expand to full C parser later)
typedef enum { TOK_INT, TOK_PLUS, TOK_MUL, TOK_EOF } TokenType;

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
