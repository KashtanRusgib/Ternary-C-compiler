#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/parser.h"
#include "../include/ir.h"
#include "../include/logger.h"

Token tokens[MAX_TOKENS];
int token_idx = 0;
char token_names[MAX_TOKENS][MAX_TOKEN_NAME];

void tokenize(const char *source) {
    token_idx = 0;
    memset(token_names, 0, sizeof(token_names));
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
            } else if (len == 6 && strncmp(&source[start], "return", 6) == 0) {
                tokens[token_idx++] = (Token){TOK_RETURN, 0};
            } else {
                int namelen = len < MAX_TOKEN_NAME - 1 ? len : MAX_TOKEN_NAME - 1;
                strncpy(token_names[token_idx], &source[start], namelen);
                token_names[token_idx][namelen] = '\0';
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
            case '-':
                tokens[token_idx++] = (Token){TOK_MINUS, 0};
                i++;
                break;
            case '*':
                tokens[token_idx++] = (Token){TOK_MUL, 0};
                i++;
                break;
            case '&':
                tokens[token_idx++] = (Token){TOK_AMP, 0};
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
            case ',':
                tokens[token_idx++] = (Token){TOK_COMMA, 0};
                i++;
                break;
            default:
                fprintf(stderr, "Unexpected character: '%c'\n", source[i]);
                exit(1);
        }
    }

    tokens[token_idx++] = (Token){TOK_EOF, 0};

    LOG_DEBUG_MSG("Lexer", "TASK-006", "tokenize complete");
}

// Parse to AST (postfix for now)
void parse(void) {
    // TODO: Shunting-yard algorithm for proper precedence
    // For now, codegen handles the hardcoded RPN: 1 2 3 * +
}

/* ==== Recursive descent parser for functions (TASK-004) ==== */

static int pidx;          /* Parser index into tokens[] */
static int perror_flag;   /* Parser error flag */

static void parser_error(const char *msg) {
    fprintf(stderr, "parser error: %s (at token %d)\n", msg, pidx);
    log_entry(LOG_ERROR, "Parser", "TASK-004", msg, NULL);
    perror_flag = 1;
}

static int expect(TokenType t) {
    if (tokens[pidx].type != t) {
        parser_error("unexpected token");
        return 0;
    }
    pidx++;
    return 1;
}

/* Forward declarations */
static Expr *parse_expr_r(void);

static Expr *parse_primary(void) {
    if (perror_flag) return NULL;

    /* Dereference: *expr */
    if (tokens[pidx].type == TOK_MUL) {
        pidx++;
        Expr *inner = parse_primary();
        if (perror_flag) return NULL;
        return create_deref(inner);
    }

    /* Address-of: &ident */
    if (tokens[pidx].type == TOK_AMP) {
        pidx++;
        if (tokens[pidx].type != TOK_IDENT) {
            parser_error("expected identifier after &");
            return NULL;
        }
        Expr *var = create_var(token_names[pidx]);
        pidx++;
        return create_addr_of(var);
    }

    if (tokens[pidx].type == TOK_INT) {
        Expr *e = create_const(tokens[pidx].value);
        pidx++;
        return e;
    }

    if (tokens[pidx].type == TOK_IDENT) {
        char *name = strdup(token_names[pidx]);
        pidx++;

        /* Function call: ident '(' args ')' */
        if (tokens[pidx].type == TOK_LPAREN) {
            pidx++; /* skip ( */

            Expr **args = NULL;
            int argc = 0;

            if (tokens[pidx].type != TOK_RPAREN) {
                /* Parse first argument */
                argc = 1;
                args = (Expr **)malloc(sizeof(Expr *));
                args[0] = parse_expr_r();
                if (perror_flag) { free(name); free(args); return NULL; }

                /* Parse remaining arguments */
                while (tokens[pidx].type == TOK_COMMA && !perror_flag) {
                    pidx++; /* skip , */
                    argc++;
                    args = (Expr **)realloc(args, argc * sizeof(Expr *));
                    args[argc - 1] = parse_expr_r();
                }
            }

            if (!expect(TOK_RPAREN)) {
                free(name);
                for (int k = 0; k < argc; k++) expr_free(args[k]);
                free(args);
                return NULL;
            }

            Expr *call = create_func_call(name, args, argc);
            free(name);
            return call;
        }

        /* Variable reference */
        Expr *var = create_var(name);
        free(name);
        return var;
    }

    parser_error("expected expression");
    return NULL;
}

static Expr *parse_term(void) {
    Expr *left = parse_primary();
    if (perror_flag) return NULL;

    while (tokens[pidx].type == TOK_MUL && !perror_flag) {
        pidx++;
        Expr *right = parse_primary();
        if (perror_flag) { expr_free(left); return NULL; }
        left = create_binop(OP_IR_MUL, left, right);
    }
    return left;
}

static Expr *parse_expr_r(void) {
    Expr *left = parse_term();
    if (perror_flag) return NULL;

    while ((tokens[pidx].type == TOK_PLUS || tokens[pidx].type == TOK_MINUS) && !perror_flag) {
        OpType op = (tokens[pidx].type == TOK_PLUS) ? OP_IR_ADD : OP_IR_SUB;
        pidx++;
        Expr *right = parse_term();
        if (perror_flag) { expr_free(left); return NULL; }
        left = create_binop(op, left, right);
    }
    return left;
}

static Expr *parse_stmt(void) {
    if (perror_flag) return NULL;

    if (tokens[pidx].type == TOK_RETURN) {
        pidx++; /* skip 'return' */
        Expr *expr = parse_expr_r();
        if (perror_flag) return NULL;
        if (!expect(TOK_SEMI)) { expr_free(expr); return NULL; }
        return create_return(expr);
    }

    /* Variable declaration: int x = expr; or int *x = expr; */
    if (tokens[pidx].type == TOK_INT_KW) {
        pidx++; /* skip 'int' */
        int is_ptr = 0;
        if (tokens[pidx].type == TOK_MUL) {
            is_ptr = 1;
            pidx++; /* skip '*' */
        }
        (void)is_ptr; /* type tracking deferred to Phase 3 */
        if (tokens[pidx].type != TOK_IDENT) {
            parser_error("expected variable name in declaration");
            return NULL;
        }
        char *vname = strdup(token_names[pidx]);
        pidx++;
        if (!expect(TOK_EQ)) { free(vname); return NULL; }
        Expr *init = parse_expr_r();
        if (perror_flag) { free(vname); return NULL; }
        if (!expect(TOK_SEMI)) { free(vname); expr_free(init); return NULL; }
        Expr *decl = create_var_decl(vname, init);
        free(vname);
        return decl;
    }

    /* Assignment or expression statement: ident = expr; or *expr = expr; */
    if (tokens[pidx].type == TOK_IDENT) {
        int saved = pidx;
        char *vname = strdup(token_names[pidx]);
        pidx++;
        if (tokens[pidx].type == TOK_EQ) {
            pidx++; /* skip '=' */
            Expr *rhs = parse_expr_r();
            if (perror_flag) { free(vname); return NULL; }
            if (!expect(TOK_SEMI)) { free(vname); expr_free(rhs); return NULL; }
            Expr *lhs = create_var(vname);
            free(vname);
            return create_assign(lhs, rhs);
        }
        /* Not an assignment — backtrack */
        free(vname);
        pidx = saved;
    }

    parser_error("expected statement (return, decl, or assign)");
    return NULL;
}

static Expr *parse_func_def_r(void) {
    if (perror_flag) return NULL;

    /* 'int' ident '(' params? ')' '{' body '}' */
    if (!expect(TOK_INT_KW)) return NULL;

    if (tokens[pidx].type != TOK_IDENT) {
        parser_error("expected function name");
        return NULL;
    }
    char *fname = strdup(token_names[pidx]);
    pidx++;

    if (!expect(TOK_LPAREN)) { free(fname); return NULL; }

    /* Parse parameter list: (int x, int y, ...) */
    Expr **params = NULL;
    int pcount = 0;

    while (tokens[pidx].type == TOK_INT_KW && !perror_flag) {
        pidx++; /* skip 'int' */
        if (tokens[pidx].type != TOK_IDENT) {
            parser_error("expected parameter name");
            free(fname);
            for (int k = 0; k < pcount; k++) expr_free(params[k]);
            free(params);
            return NULL;
        }
        pcount++;
        params = (Expr **)realloc(params, pcount * sizeof(Expr *));
        params[pcount - 1] = create_var(token_names[pidx]);
        pidx++;

        if (tokens[pidx].type == TOK_COMMA) pidx++; /* skip , */
    }

    if (!expect(TOK_RPAREN)) {
        free(fname);
        for (int k = 0; k < pcount; k++) expr_free(params[k]);
        free(params);
        return NULL;
    }

    if (!expect(TOK_LBRACE)) {
        free(fname);
        for (int k = 0; k < pcount; k++) expr_free(params[k]);
        free(params);
        return NULL;
    }

    /* Parse body: multiple statements, last one used as body.
     * For Phase 2, we store only the last return stmt as the body
     * and any preceding stmts as additional params (stmts list). */
    Expr **stmts = NULL;
    int stmt_count = 0;
    Expr *body = NULL;

    while (tokens[pidx].type != TOK_RBRACE && !perror_flag) {
        Expr *s = parse_stmt();
        if (perror_flag) {
            free(fname);
            for (int k = 0; k < pcount; k++) expr_free(params[k]);
            free(params);
            for (int k = 0; k < stmt_count; k++) expr_free(stmts[k]);
            free(stmts);
            return NULL;
        }
        if (body != NULL) {
            /* previous body becomes a stmt */
            stmt_count++;
            stmts = (Expr **)realloc(stmts, stmt_count * sizeof(Expr *));
            stmts[stmt_count - 1] = body;
        }
        body = s;
    }

    if (!expect(TOK_RBRACE)) {
        free(fname);
        expr_free(body);
        for (int k = 0; k < pcount; k++) expr_free(params[k]);
        free(params);
        for (int k = 0; k < stmt_count; k++) expr_free(stmts[k]);
        free(stmts);
        return NULL;
    }

    /* Merge params and preceding stmts into one param list */
    int total = pcount + stmt_count;
    Expr **all_params = NULL;
    if (total > 0) {
        all_params = (Expr **)malloc(total * sizeof(Expr *));
        for (int k = 0; k < pcount; k++) all_params[k] = params[k];
        for (int k = 0; k < stmt_count; k++) all_params[pcount + k] = stmts[k];
    }
    free(params);
    free(stmts);

    Expr *fn = create_func_def(fname, all_params, total, body);
    free(fname);
    return fn;
}

Expr *parse_program(const char *source) {
    LOG_DEBUG_MSG("Parser", "TASK-004", "parse_program entered");
    tokenize(source);
    pidx = 0;
    perror_flag = 0;

    Expr *prog = create_program();

    while (tokens[pidx].type != TOK_EOF && !perror_flag) {
        Expr *fn = parse_func_def_r();
        if (fn == NULL || perror_flag) {
            expr_free(prog);
            return NULL;
        }
        program_add_func(prog, fn);
    }

    return prog;
}
