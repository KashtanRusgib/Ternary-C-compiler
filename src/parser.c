#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/parser.h"

Token tokens[MAX_TOKENS];
int token_idx = 0;

void tokenize(const char *source) {
    token_idx = 0;
    int i = 0;

    while (source[i] != '\0') {
        // Skip whitespace
        if (isspace(source[i])) {
            i++;
            continue;
        }

        // Integer literal
        if (isdigit(source[i])) {
            int value = 0;
            while (isdigit(source[i])) {
                value = value * 10 + (source[i] - '0');
                i++;
            }
            tokens[token_idx++] = (Token){TOK_INT, value};
            continue;
        }

        // Operators
        switch (source[i]) {
            case '+':
                tokens[token_idx++] = (Token){TOK_PLUS, 0};
                i++;
                break;
            case '*':
                tokens[token_idx++] = (Token){TOK_MUL, 0};
                i++;
                break;
            default:
                fprintf(stderr, "Unexpected character: '%c'\n", source[i]);
                exit(1);
        }
    }

    tokens[token_idx++] = (Token){TOK_EOF, 0};
}

// Parse to AST (postfix for now)
void parse(void) {
    // TODO: Shunting-yard algorithm for proper precedence
    // For now, codegen handles the hardcoded RPN: 1 2 3 * +
}
