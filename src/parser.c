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
        /* Skip whitespace */
        if (isspace(source[i])) {
            i++;
            continue;
        }

        /* Keywords and identifiers (start with alpha or underscore) */
        if (isalpha(source[i]) || source[i] == '_') {
            int start = i;
            while (isalnum(source[i]) || source[i] == '_') i++;
            int len = i - start;

            /* Check keywords first */
            if (len == 3 && strncmp(&source[start], "for", 3) == 0) {
                tokens[token_idx++] = (Token){TOK_FOR, 0};
            } else if (len == 5 && strncmp(&source[start], "while", 5) == 0) {
                tokens[token_idx++] = (Token){TOK_WHILE, 0};
            } else if (len == 3 && strncmp(&source[start], "int", 3) == 0) {
                tokens[token_idx++] = (Token){TOK_INT_KW, 0};
            } else {
                tokens[token_idx++] = (Token){TOK_IDENT, 0};
            }
            continue;
        }

        /* Integer literal */
        if (isdigit(source[i])) {
            int value = 0;
            while (isdigit(source[i])) {
                value = value * 10 + (source[i] - '0');
                i++;
            }
            tokens[token_idx++] = (Token){TOK_INT, value};
            continue;
        }

        /* Operators and symbols */
        switch (source[i]) {
            case '+':
                if (source[i + 1] == '+') {
                    tokens[token_idx++] = (Token){TOK_PLUS_PLUS, 0};
                    i += 2;
                } else {
                    tokens[token_idx++] = (Token){TOK_PLUS, 0};
                    i++;
                }
                break;
            case '*':
                tokens[token_idx++] = (Token){TOK_MUL, 0};
                i++;
                break;
            case '=':
                tokens[token_idx++] = (Token){TOK_EQ, 0};
                i++;
                break;
            case '<':
                tokens[token_idx++] = (Token){TOK_LT, 0};
                i++;
                break;
            case '(':
                tokens[token_idx++] = (Token){TOK_LPAREN, 0};
                i++;
                break;
            case ')':
                tokens[token_idx++] = (Token){TOK_RPAREN, 0};
                i++;
                break;
            case '{':
                tokens[token_idx++] = (Token){TOK_LBRACE, 0};
                i++;
                break;
            case '}':
                tokens[token_idx++] = (Token){TOK_RBRACE, 0};
                i++;
                break;
            case ';':
                tokens[token_idx++] = (Token){TOK_SEMI, 0};
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
