#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

#define MAX_LEXEME_LEN 65

typedef enum {
    IDENTIFIER, TYPE, NUMBER, SEMICOLON, ASSIGNMENT, PLUS, MINUS, MULTIPLICATION, DIVISION, EXPONENTIATION, LEFTPARENTHESES, RIGHTPARENTHESES, EQUALITY, INEQUALITY, GREATERTHAN, LESSTHAN, GREATERTHANEQUAL, LESSTHANEQUAL, NOT, AND, OR, XOR, COMMA, TRUEVALUE, FALSEVALUE, NULLVALUE, STRING, LEFTBRACE, RIGHTBRACE, PARAMETER, RETURN, QUESTIONMARK, COLON, WHILE, CONTINUE, BREAK, AT, FORMATTEDSTRING, LEFTBRACKET, RIGHTBRACKET, PLUSASSIGNMENT, SUBTRACTIONASSIGNMENT, MULTIPLICATIONASSIGNMENT, DIVISIONASSIGNMENT, EXPONENTIATIONASSIGNMENT, INCREMENT, DECREMENT, REFERENCE, PERIOD, NAMESPACE, PRINT, SCAN, RAISE, CASTBOOLEAN, CASTINTEGER, CASTFLOAT, CASTSTRING, CASTARRAY, CASTHASHTABLE, TYPEOF, MODULO, FILEREAD, FILEWRITE, TIME, EXECUTE, CATCONSTANT, NULLCOALESCE, LENGTH, PLACEHOLDER, TOKENEOF, FORMATTEDSTART, FORMATTEDEND, FORMATTEDPART, UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
    int line;
    int column;
} Token;

typedef struct AstNode AstNode;

typedef enum {
    AST_UNKNOWN,
    AST_PROGRAM,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_LOGICAL_OP,
    AST_ASSIGNMENT,
    AST_IDENTIFIER,
    AST_LITERAL,
    AST_PRINT_STATEMENT,
    AST_FORMATTED_STRING,
    AST_ARRAY_LITERAL,
    AST_SUBSCRIPT,
    AST_HASHTABLE_LITERAL,
    AST_FUNCTION_DECLARATION,
    AST_CALL_EXPRESSION,
    AST_RETURN_STATEMENT,
    AST_BLOCK,
    AST_EXPRESSION_STATEMENT,
    AST_IF_STATEMENT,
    AST_TERNARY_EXPRESSION
} AstNodeType;

typedef struct {
    AstNode** statements;
    size_t count;
    size_t capacity;
} AstNodeProgram;

typedef struct {
    Token operator;
    AstNode *left;
    AstNode *right;
} AstNodeBinaryOp;

typedef struct {
    Token operator;
    AstNode *right;
} AstNodeUnaryOp;

typedef struct {
    Token operator;
    AstNode *left;
    AstNode *right;
} AstNodeLogicalOp;

typedef struct {
    AstNode *left;
    AstNode *value;
} AstNodeAssignment;

typedef struct {
    Token name;
} AstNodeIdentifier;

typedef struct {
    Token value;
} AstNodeLiteral;

typedef struct {
    AstNode** expressions;
    size_t count;
    size_t capacity;
} AstNodePrintStmt;

typedef enum {
    FMT_PART_LITERAL,
    FMT_PART_EXPRESSION
} FmtStringPartType;

typedef struct {
    FmtStringPartType type;
    union {
        Token   literal;
        AstNode* expression;
    } as;
} FmtStringPart;

typedef struct {
    FmtStringPart* parts;
    size_t count;
    size_t capacity;
} AstNodeFormattedString;

typedef struct {
    AstNode** elements;
    size_t count;
    size_t capacity;
} AstNodeArrayLiteral;

typedef struct {
    AstNode* array;
    AstNode* index;
} AstNodeSubscript;

typedef struct {
    AstNode* key;
    AstNode* value;
} AstNodeKeyValuePair;

typedef struct {
    AstNodeKeyValuePair* pairs;
    size_t count;
    size_t capacity;
} AstNodeHashtableLiteral;

typedef struct {
    Token name;
    Token* params;
    size_t param_count;
    size_t param_capacity;
    AstNode* body;
} AstNodeFunctionDeclaration;

typedef struct {
    AstNode* callee;
    AstNode** arguments;
    size_t arg_count;
    size_t arg_capacity;
    Token paren;
} AstNodeCallExpression;

typedef struct {
    Token keyword;
    AstNode* value;
} AstNodeReturnStatement;

typedef struct {
    AstNode** statements;
    size_t count;
    size_t capacity;
} AstNodeBlock;

typedef struct {
    AstNode* expression;
} AstNodeExpressionStatement;

typedef struct {
    AstNode* condition;
    AstNode* body;
} AstNodeElseIfClause;

typedef struct {
    AstNode* condition;
    AstNode* then_branch;
    AstNodeElseIfClause* else_if_clauses;
    size_t else_if_count;
    size_t else_if_capacity;
    AstNode* else_branch;
} AstNodeIfStatement;

typedef struct {
    AstNode* condition;
    AstNode* then_expr;
    AstNode* else_expr;
} AstNodeTernaryExpression;

struct AstNode {
    AstNodeType type;
    int line;

    union {
        AstNodeProgram              program;
        AstNodeBinaryOp             binary_op;
        AstNodeUnaryOp              unary_op;
        AstNodeLogicalOp            logical_op;
        AstNodeAssignment           assignment;
        AstNodeIdentifier           identifier;
        AstNodeLiteral              literal;
        AstNodeFormattedString      formatted_string;
        AstNodeArrayLiteral         array_literal;
        AstNodePrintStmt            print_stmt;
        AstNodeSubscript            subscript;
        AstNodeHashtableLiteral     hashtable_literal;
        AstNodeFunctionDeclaration  function_declaration;
        AstNodeCallExpression       call_expression;
        AstNodeReturnStatement      return_statement;
        AstNodeBlock                block;
        AstNodeExpressionStatement  expression_statement;
        AstNodeIfStatement          if_statement;
        AstNodeTernaryExpression   ternary_expression;
    } as;
};

typedef struct GraveyardString GraveyardString;
typedef struct GraveyardValue GraveyardValue;
typedef struct GraveyardArray GraveyardArray;
typedef struct GraveyardHashtable GraveyardHashtable;
typedef struct GraveyardFunction GraveyardFunction;

typedef enum {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_ARRAY,
    VAL_HASHTABLE,
    VAL_FUNCTION
} ValueType;

struct GraveyardValue {
    ValueType type;
    union {
        bool                boolean;
        double              number;
        GraveyardString*    string;
        GraveyardArray*     array;
        GraveyardHashtable* hashtable;
        GraveyardFunction*  function;
    } as;
};

struct GraveyardString {
    int ref_count;
    char* chars;
    size_t length;
};

struct GraveyardArray {
    int ref_count;
    size_t count;
    size_t capacity;
    GraveyardValue* values;
};

typedef struct {
    bool is_in_use;
    GraveyardValue key;
    GraveyardValue value;
} HashtableEntry;

struct GraveyardHashtable {
    int ref_count;
    int count;
    int capacity;
    HashtableEntry* entries;
};

typedef struct {
    char* key;
    GraveyardValue value;
} MonolithEntry;

typedef struct {
    int count;
    int capacity;
    MonolithEntry* entries;
} Monolith;

typedef struct Environment {
    struct Environment* enclosing;
    Monolith values;
} Environment;

struct GraveyardFunction {
    int ref_count;
    int arity;
    Environment* closure;
    GraveyardString* name;
    AstNode* body;
    Token* params;
};

typedef struct {
    char** lines;
    int count;
} Lines;

typedef struct {
    const char *mode;
    const char *filename;
    char *source_code;
    Token *tokens;
    size_t token_count;
    AstNode *ast_root;
    Environment* environment;
    GraveyardValue last_executed_value;
    bool is_returning;
    GraveyardValue return_value;
} Graveyard;

typedef struct {
    Graveyard* gy;
    Token* tokens;
    size_t     token_count;
    size_t     current;
    bool       had_error;
} Parser;

//LOAD--------------------------------------------------------------------

char *load(FILE *file, long *out_length) {
    if (out_length) { *out_length = 0; }
    if (!file) { fprintf(stderr, "load: Received NULL file pointer\n"); return NULL; }
    if (fseek(file, 0, SEEK_END) != 0) { perror("load: fseek to SEEK_END failed"); return NULL; }
    long length = ftell(file);
    if (length == -1L) { perror("load: ftell failed after seeking to end"); return NULL; }
    if (fseek(file, 0, SEEK_SET) != 0) { perror("load: fseek to SEEK_SET failed"); return NULL; }
    if (length < 0) { fprintf(stderr, "load: ftell returned negative file size unexpectedly.\n"); return NULL; }
    if ((unsigned long)length >= SIZE_MAX) { fprintf(stderr, "load: File size is too large.\n"); return NULL; }
    char *buffer = malloc((size_t)length + 1);
    if (!buffer) { fprintf(stderr, "load: Memory allocation failed.\n"); return NULL; }
    size_t items_read = 0;
    if (length > 0) {
        items_read = fread(buffer, 1, (size_t)length, file);
        if (items_read < (size_t)length) {
            if (ferror(file)) { perror("load: fread encountered an error"); free(buffer); return NULL; }
        }
    }
    buffer[items_read] = '\0';
    if (out_length) { *out_length = (long)items_read; }
    return buffer;
}

//TOKENIZE-----------------------------------------------------------------------------------

TokenType identify_three_char_token(char c1, char c2, char c3) {
    if (c1 == '*' && c2 == '*' && c3 == '=') return EXPONENTIATIONASSIGNMENT;
    if (c1 == '!' && c2 == '|' && c3 == '|') return XOR;
    if (c1 == '!' && c2 == '>' && c3 == '>') return RAISE;
    if (c1 == ':' && c2 == '<' && c3 == '<') return FILEREAD;
    if (c1 == ':' && c2 == '>' && c3 == '>') return FILEWRITE;
    return UNKNOWN;
}

TokenType identify_two_char_token(char c1, char c2) {
    if (c1 == '*' && c2 == '*') return EXPONENTIATION;
    if (c1 == '=' && c2 == '=') return EQUALITY;
    if (c1 == '!' && c2 == '=') return INEQUALITY;
    if (c1 == '>' && c2 == '=') return GREATERTHANEQUAL;
    if (c1 == '<' && c2 == '=') return LESSTHANEQUAL;
    if (c1 == '&' && c2 == '&') return AND;
    if (c1 == '|' && c2 == '|') return OR;
    if (c1 == '-' && c2 == '>') return RETURN;
    if (c1 == '+' && c2 == '=') return PLUSASSIGNMENT;
    if (c1 == '-' && c2 == '=') return SUBTRACTIONASSIGNMENT;
    if (c1 == '*' && c2 == '=') return MULTIPLICATIONASSIGNMENT;
    if (c1 == '/' && c2 == '=') return DIVISIONASSIGNMENT;
    if (c1 == '+' && c2 == '+') return INCREMENT;
    if (c1 == '-' && c2 == '-') return DECREMENT;
    if (c1 == ':' && c2 == ':') return NAMESPACE;
    if (c1 == '>' && c2 == '>') return PRINT;
    if (c1 == '<' && c2 == '<') return SCAN;
    if (c1 == '>' && c2 == 'b') return CASTBOOLEAN;
    if (c1 == '>' && c2 == 'i') return CASTINTEGER;
    if (c1 == '>' && c2 == 'f') return CASTFLOAT;
    if (c1 == '>' && c2 == 's') return CASTSTRING;
    if (c1 == '>' && c2 == 'a') return CASTARRAY;
    if (c1 == '>' && c2 == 'h') return CASTHASHTABLE;
    if (c1 == '@' && c2 == '@') return TYPEOF;
    if (c1 == '/' && c2 == '%') return MODULO;
    if (c1 == ':' && c2 == '@') return TIME;
    if (c1 == ':' && c2 == '=') return EXECUTE;
    if (c1 == ':' && c2 == '3') return CATCONSTANT;
    if (c1 == '?' && c2 == '?') return NULLCOALESCE;
    if (c1 == '@' && c2 == '?') return LENGTH;
    return UNKNOWN;
}

TokenType identify_single_char_token(char c) {
    switch (c) {
        case '=': return ASSIGNMENT;
        case ';': return SEMICOLON;
        case '+': return PLUS;
        case '-': return MINUS;
        case '*': return MULTIPLICATION;
        case '/': return DIVISION;
        case '(': return LEFTPARENTHESES;
        case ')': return RIGHTPARENTHESES;
        case '>': return GREATERTHAN;
        case '<': return LESSTHAN;
        case '!': return NOT;
        case ',': return COMMA;
        case '$': return TRUEVALUE;
        case '%': return FALSEVALUE;
        case '|': return NULLVALUE;
        case '{': return LEFTBRACE;
        case '}': return RIGHTBRACE;
        case '&': return PARAMETER;
        case '?': return QUESTIONMARK;
        case ':': return COLON;
        case '~': return WHILE;
        case '^': return CONTINUE;
        case '`': return BREAK;
        case '@': return AT;
        case '[': return LEFTBRACKET;
        case ']': return RIGHTBRACKET;
        case '#': return REFERENCE;
        case '.': return PERIOD;
        default: return UNKNOWN;
    }
}

bool tokenize(Graveyard *gy) {
    const char* source = gy->source_code;
    const char* start_ptr = strstr(source, "::{");
    if (!start_ptr) {
        fprintf(stderr, "Fatal error: Could not find global scope start '::{'.\n");
        return false;
    }
    start_ptr += 3;

    const char* end_ptr = strrchr(start_ptr, '}');
    if (!end_ptr) {
        fprintf(stderr, "Fatal error: Could not find global scope end '}'.\n");
        return false;
    }
    
    size_t capacity = 16;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    if (!tokens) { perror("tokenize: malloc failed"); return false; }

    typedef enum { STATE_DEFAULT, STATE_IN_FMT_STRING } TokenizerState;
    TokenizerState state = STATE_DEFAULT;
    int fstring_brace_depth = 0;
    
    size_t line = 1;
    const char* line_start_ptr = source;
    const char* current_ptr = start_ptr;

    while (current_ptr < end_ptr) {
        if (count + 1 >= capacity) {
            size_t new_capacity = capacity * 2;
            Token *new_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!new_tokens) { perror("tokenize: realloc failed"); goto cleanup_failure; }
            tokens = new_tokens;
            capacity = new_capacity;
        }

        if (state == STATE_IN_FMT_STRING) {
            int column = (current_ptr - line_start_ptr) + 1;
            char c = *current_ptr;

            if (c == '\'') {
                state = STATE_DEFAULT;
                tokens[count].type = FORMATTEDEND;
                tokens[count].lexeme[0] = '\''; tokens[count].lexeme[1] = '\0';
                tokens[count].line = line; tokens[count].column = column;
                count++;
                current_ptr++;
                continue;
            }
            if (c == '{') {
                state = STATE_DEFAULT;
                fstring_brace_depth = 1;
                tokens[count].type = LEFTBRACE;
                tokens[count].lexeme[0] = '{'; tokens[count].lexeme[1] = '\0';
                tokens[count].line = line; tokens[count].column = column;
                count++;
                current_ptr++;
                continue;
            }
            if (c == '\n' || c == '\0') {
                fprintf(stderr, "Tokenizer error [line %zu, col %d]: Unterminated formatted string.\n", line, column);
                goto cleanup_failure;
            }
            
            const char* start = current_ptr;
            while (current_ptr < end_ptr && *current_ptr != '\'' && *current_ptr != '{' && *current_ptr != '\n') {
                current_ptr++;
            }
            size_t len = current_ptr - start;
            if (len > 0) {
                if (len >= MAX_LEXEME_LEN) {
                    fprintf(stderr, "Tokenizer error [line %d, col %d]: Literal part of formatted string is too long.\n", line, column);
                    goto cleanup_failure;
                }
                tokens[count].type = FORMATTEDPART;
                strncpy(tokens[count].lexeme, start, len);
                tokens[count].lexeme[len] = '\0';
                tokens[count].line = line;
                tokens[count].column = column;
                count++;
            }
            continue;
        }
        
        if (current_ptr + 1 < end_ptr && *current_ptr == '/' && *(current_ptr + 1) == '/') {
            current_ptr += 2;
            while (current_ptr < end_ptr && *current_ptr != '\n') { current_ptr++; }
            continue;
        }

        if (current_ptr + 1 < end_ptr && *current_ptr == '/' && *(current_ptr + 1) == '*') {
            const char* comment_start = current_ptr;
            int comment_start_line = line;
            int comment_start_col = (comment_start - line_start_ptr) + 1;
            current_ptr += 2;
            while (current_ptr + 1 < end_ptr && !(*current_ptr == '*' && *(current_ptr + 1) == '/')) {
                if (*current_ptr == '\n') { line++; line_start_ptr = current_ptr + 1; }
                current_ptr++;
            }

            if (current_ptr + 1 >= end_ptr) {
                fprintf(stderr, "Tokenizer error [line %d, col %d]: Unterminated block comment.\n", comment_start_line, comment_start_col);
                goto cleanup_failure;
            }

            current_ptr += 2;
            continue;
        }
        if (isspace((unsigned char)*current_ptr)) {
            if (*current_ptr == '\n') { line++; line_start_ptr = current_ptr + 1; }
            current_ptr++;
            continue;
        }

        int column = (current_ptr - line_start_ptr) + 1;
        char c = *current_ptr;

        if (c == '\'') {
            state = STATE_IN_FMT_STRING;
            tokens[count].type = FORMATTEDSTART;
            tokens[count].lexeme[0] = '\''; tokens[count].lexeme[1] = '\0';
            tokens[count].line = line; tokens[count].column = column;
            count++;
            current_ptr++;
            continue;
        }
        
        if (fstring_brace_depth > 0) {
            if (c == '}') {
                fstring_brace_depth--;
                if (fstring_brace_depth == 0) { state = STATE_IN_FMT_STRING; }
            } else if (c == '{') { fstring_brace_depth++; }
        }

        if (isalpha((unsigned char)c) || c == '_') {
            const char* start = current_ptr;
            while (current_ptr < end_ptr && (isalnum((unsigned char)*current_ptr) || *current_ptr == '_')) { current_ptr++; }
            size_t len = current_ptr - start;
            if (len >= MAX_LEXEME_LEN) {
                fprintf(stderr, "Tokenizer error [line %d, col %d]: Identifier is too long.\n", line, column); goto cleanup_failure;
            }
            tokens[count].type = IDENTIFIER;
            strncpy(tokens[count].lexeme, start, len); tokens[count].lexeme[len] = '\0';
            tokens[count].line = line; tokens[count].column = column;
            count++;

        } else if (isdigit((unsigned char)c) || (c == '.' && (current_ptr + 1 < end_ptr) && isdigit((unsigned char)*(current_ptr+1)))) {
            const char* start = current_ptr;
            while (current_ptr < end_ptr && isdigit((unsigned char)*current_ptr)) { current_ptr++; }
            if (current_ptr + 1 < end_ptr && *current_ptr == '.' && isdigit((unsigned char)*(current_ptr + 1))) {
                current_ptr++;
                while (current_ptr < end_ptr && isdigit((unsigned char)*current_ptr)) { current_ptr++; }
            }
            size_t len = current_ptr - start;
            if (len >= MAX_LEXEME_LEN) {
                 fprintf(stderr, "Tokenizer error [line %d, col %d]: Number literal is too long.\n", line, column); goto cleanup_failure;
            }
            tokens[count].type = NUMBER;
            strncpy(tokens[count].lexeme, start, len); tokens[count].lexeme[len] = '\0';
            tokens[count].line = line; tokens[count].column = column;
            count++;

        } else if (c == '"') {
            const char* string_start_ptr = current_ptr;
            int start_line = line;
            int start_col = column;
            current_ptr++;

            char lexeme_buffer[MAX_LEXEME_LEN];
            size_t len = 0;

            while (current_ptr < end_ptr && *current_ptr != '"') {
                char ch = *current_ptr;

                if (ch == '\n') {
                    fprintf(stderr, "Tokenizer error [line %d, col %d]: Unterminated string literal (newline encountered).\n", line, column);
                    goto cleanup_failure;
                }
                if (len >= MAX_LEXEME_LEN - 1) {
                    fprintf(stderr, "Tokenizer error [line %d, col %d]: String literal is too long.\n", start_line, start_col);
                    goto cleanup_failure;
                }

                if (ch == '\\' && current_ptr + 1 < end_ptr) {
                    current_ptr++;
                    char next_ch = *current_ptr;
                    switch (next_ch) {
                        case 'n':  lexeme_buffer[len++] = '\n'; break;
                        case 't':  lexeme_buffer[len++] = '\t'; break;
                        case '"':  lexeme_buffer[len++] = '\"'; break;
                        case '\\': lexeme_buffer[len++] = '\\'; break;
                        default:   lexeme_buffer[len++] = next_ch; break;
                    }
                } else {
                    lexeme_buffer[len++] = ch;
                }
                current_ptr++;
            }

            if (current_ptr >= end_ptr || *current_ptr != '"') {
                fprintf(stderr, "Tokenizer error [line %d, col %d]: Unterminated string literal.\n", start_line, start_col);
                goto cleanup_failure;
            }
            current_ptr++;

            tokens[count].type = STRING;
            strncpy(tokens[count].lexeme, lexeme_buffer, len);
            tokens[count].lexeme[len] = '\0';
            tokens[count].line = start_line;
            tokens[count].column = start_col;
            count++;

        } else {
            const char* start = current_ptr;
            if (c == '<') {
                const char* lookahead_ptr = current_ptr + 1;
                if (lookahead_ptr < end_ptr && (isalpha((unsigned char)*lookahead_ptr) || *lookahead_ptr == '_')) {
                    lookahead_ptr++;
                    while (lookahead_ptr < end_ptr && (isalnum((unsigned char)*lookahead_ptr) || *lookahead_ptr == '_')) { lookahead_ptr++; }
                    if (lookahead_ptr < end_ptr && *lookahead_ptr == '>') {
                        size_t len = (lookahead_ptr + 1) - start;
                        if (len >= MAX_LEXEME_LEN) {
                            fprintf(stderr, "Tokenizer error [line %d, col %d]: TYPE_IDENTIFIER is too long.\n", line, column); goto cleanup_failure;
                        }
                        tokens[count].type = TYPE;
                        strncpy(tokens[count].lexeme, start, len); tokens[count].lexeme[len] = '\0';
                        tokens[count].line = line; tokens[count].column = column;
                        count++;
                        current_ptr = lookahead_ptr + 1;
                        continue;
                    }
                }
            }

            TokenType ttype = UNKNOWN;

            if (current_ptr + 2 < end_ptr) {
                ttype = identify_three_char_token(start[0], start[1], start[2]);
                if (ttype != UNKNOWN) {
                    snprintf(tokens[count].lexeme, 4, "%c%c%c", start[0], start[1], start[2]);
                    tokens[count].type = ttype; tokens[count].line = line; tokens[count].column = column;
                    count++; current_ptr += 3; continue;
                }
            }

            if (current_ptr + 1 < end_ptr) {
                ttype = identify_two_char_token(start[0], start[1]);
                if (ttype != UNKNOWN) {
                    snprintf(tokens[count].lexeme, 3, "%c%c", start[0], start[1]);
                    tokens[count].type = ttype; tokens[count].line = line; tokens[count].column = column;
                    count++; current_ptr += 2; continue;
                }
            }

            ttype = identify_single_char_token(c);
            if (ttype == UNKNOWN) {
                fprintf(stderr, "Tokenizer error [line %d, col %d]: Unknown character encountered: '%c'\n", line, column, c);
                goto cleanup_failure;
            }
            tokens[count].type = ttype;
            tokens[count].lexeme[0] = c; tokens[count].lexeme[1] = '\0';
            tokens[count].line = line; tokens[count].column = column;
            count++;
            current_ptr++;
        }
    }

    if (state == STATE_IN_FMT_STRING) {
        fprintf(stderr, "Tokenizer error [line %zu]: Unterminated formatted string at end of file.\n", line);
        goto cleanup_failure;
    }

    tokens[count].type = TOKENEOF;
    tokens[count].lexeme[0] = '\0';
    tokens[count].line = line;
    tokens[count].column = (current_ptr - line_start_ptr) + 1;
    count++;
    
    if (count < capacity && count > 0) {
        Token *shrunk_tokens = realloc(tokens, count * sizeof(Token));
        if (shrunk_tokens) { tokens = shrunk_tokens; }
    }
    
    gy->tokens = tokens;
    gy->token_count = count;
    return true;

cleanup_failure:
    free(tokens);
    gy->tokens = NULL;
    gy->token_count = 0;
    return false;
}

//PARSE---------------------------------------------------------------

void free_ast(AstNode* node) {
    if (node == NULL) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (size_t i = 0; i < node->as.program.count; i++) {
                free_ast(node->as.program.statements[i]);
            }
            free(node->as.program.statements);
            break;
        case AST_PRINT_STATEMENT:
            for (size_t i = 0; i < node->as.print_stmt.count; i++) {
                free_ast(node->as.print_stmt.expressions[i]);
            }
            free(node->as.print_stmt.expressions);
            break;
        case AST_LOGICAL_OP:
            free_ast(node->as.logical_op.left);
            free_ast(node->as.logical_op.right);
            break;
        case AST_BINARY_OP:
            free_ast(node->as.binary_op.left);
            free_ast(node->as.binary_op.right);
            break;
        case AST_UNARY_OP:
            free_ast(node->as.unary_op.right);
            break;
        case AST_ASSIGNMENT:
            free_ast(node->as.assignment.value);
            break;
        case AST_FORMATTED_STRING:
            for (size_t i = 0; i < node->as.formatted_string.count; i++) {
                FmtStringPart part = node->as.formatted_string.parts[i];
                if (part.type == FMT_PART_EXPRESSION) {
                    free_ast(part.as.expression);
                }
            }
            free(node->as.formatted_string.parts);
            break;
        case AST_ARRAY_LITERAL:
            for (size_t i = 0; i < node->as.array_literal.count; i++) {
                free_ast(node->as.array_literal.elements[i]);
            }
            free(node->as.array_literal.elements);
            break;
        case AST_SUBSCRIPT:
            free_ast(node->as.subscript.array);
            free_ast(node->as.subscript.index);
            break;
        case AST_HASHTABLE_LITERAL:
            for (size_t i = 0; i < node->as.hashtable_literal.count; i++) {
                free_ast(node->as.hashtable_literal.pairs[i].key);
                free_ast(node->as.hashtable_literal.pairs[i].value);
            }
            free(node->as.hashtable_literal.pairs);
            break;
        case AST_FUNCTION_DECLARATION:
            free(node->as.function_declaration.params);
            free_ast(node->as.function_declaration.body);
            break;
        case AST_CALL_EXPRESSION:
            free_ast(node->as.call_expression.callee);
            for (size_t i = 0; i < node->as.call_expression.arg_count; i++) {
                free_ast(node->as.call_expression.arguments[i]);
            }
            free(node->as.call_expression.arguments);
            break;
        case AST_RETURN_STATEMENT:
            free_ast(node->as.return_statement.value);
            break;

        case AST_BLOCK:
            for (size_t i = 0; i < node->as.block.count; i++) {
                free_ast(node->as.block.statements[i]);
            }
            free(node->as.block.statements);
            break;
        case AST_EXPRESSION_STATEMENT:
            free_ast(node->as.expression_statement.expression);
            break;
        case AST_IF_STATEMENT:
            free_ast(node->as.if_statement.condition);
            free_ast(node->as.if_statement.then_branch);
            for (size_t i = 0; i < node->as.if_statement.else_if_count; i++) {
                free_ast(node->as.if_statement.else_if_clauses[i].condition);
                free_ast(node->as.if_statement.else_if_clauses[i].body);
            }
            free(node->as.if_statement.else_if_clauses);
            free_ast(node->as.if_statement.else_branch);
            break;
        case AST_TERNARY_EXPRESSION:
            free_ast(node->as.ternary_expression.condition);
            free_ast(node->as.ternary_expression.then_expr);
            free_ast(node->as.ternary_expression.else_expr);
            break;
        case AST_IDENTIFIER:
        case AST_LITERAL:
            break;
    }
    free(node);
}

static Token* peek(Parser* parser) {
    return &parser->tokens[parser->current];
}

static bool is_at_end(Parser* parser) {
    return peek(parser)->type == TOKENEOF;
}

static Token* consume(Parser* parser) {
    if (!is_at_end(parser)) {
        parser->current++;
    }
    return &parser->tokens[parser->current - 1];
}

static void error_at_token(Parser* parser, Token* token, const char* message) {
    if (parser->had_error) return;
    parser->had_error = true;

    fprintf(stderr, "Parser Error [line %d, col %d]: ", token->line, token->column);
    if (token->type == TOKENEOF) {
        fprintf(stderr, "at end of file. ");
    } else {
        fprintf(stderr, "at '%s'. ", token->lexeme);
    }
    fprintf(stderr, "%s\n", message);
}

static Token* expect(Parser* parser, TokenType type, const char* message) {
    if (peek(parser)->type == type) {
        return consume(parser);
    }
    error_at_token(parser, peek(parser), message);
    return NULL;
}

static bool match(Parser* parser, TokenType type) {
    if (is_at_end(parser)) return false;
    if (peek(parser)->type == type) {
        consume(parser);
        return true;
    }
    return false;
}

static AstNode* create_node(Parser* parser, AstNodeType type) {
    AstNode* node = malloc(sizeof(AstNode));
    if (!node) {
        parser->had_error = true;
        perror("AST node malloc failed");
        return NULL;
    }
    node->type = type;
    return node;
}

static int get_operator_precedence(TokenType type) {
    switch (type) {
        case QUESTIONMARK:
            return 2;
        case OR:
            return 3;
        case XOR:
            return 4;
        case AND:
            return 5;
        case EQUALITY:
        case INEQUALITY:
            return 6;
        case LESSTHAN:
        case GREATERTHAN:
        case LESSTHANEQUAL:
        case GREATERTHANEQUAL:
            return 7;
        case PLUS:
        case MINUS:
            return 8;
        case MULTIPLICATION:
        case DIVISION:
        case MODULO:
            return 9;
        case EXPONENTIATION:
            return 10;
        default:
            return 0;
    }
}

static AstNode* parse_statement(Parser* parser);
static AstNode* parse_expression(Parser* parser, int min_precedence);

static AstNode* parse_formatted_string(Parser* parser) {
    Token start_token = parser->tokens[parser->current - 1];

    AstNode* node = create_node(parser, AST_FORMATTED_STRING);
    if (!node) return NULL;
    node->line = start_token.line;

    node->as.formatted_string.capacity = 4;
    node->as.formatted_string.count = 0;
    node->as.formatted_string.parts = malloc(node->as.formatted_string.capacity * sizeof(FmtStringPart));
    if (!node->as.formatted_string.parts) {
        perror("AST formatted string parts malloc failed");
        free(node);
        parser->had_error = true;
        return NULL;
    }

    while (peek(parser)->type != FORMATTEDEND && !is_at_end(parser)) {

        if (match(parser, FORMATTEDPART)) {
            FmtStringPart part;
            part.type = FMT_PART_LITERAL;
            part.as.literal = parser->tokens[parser->current - 1];
            node->as.formatted_string.parts[node->as.formatted_string.count++] = part;
        } else if (match(parser, LEFTBRACE)) {
            FmtStringPart part;
            part.type = FMT_PART_EXPRESSION;
            part.as.expression = parse_expression(parser, 1);
            expect(parser, RIGHTBRACE, "Expected '}' after expression in formatted string.");
            node->as.formatted_string.parts[node->as.formatted_string.count++] = part;
        } else {
            error_at_token(parser, peek(parser), "Unexpected token inside formatted string.");
            break;
        }
    }

    expect(parser, FORMATTEDEND, "Expected closing ' to terminate formatted string.");
    return node;
}

static AstNode* parse_array_literal(Parser* parser) {
    Token start_token = parser->tokens[parser->current - 1];

    AstNode* node = create_node(parser, AST_ARRAY_LITERAL);
    if (!node) return NULL;
    node->line = start_token.line;
    node->as.array_literal.capacity = 4;
    node->as.array_literal.count = 0;
    node->as.array_literal.elements = malloc(node->as.array_literal.capacity * sizeof(AstNode*));
    if (!node->as.array_literal.elements) {
        perror("AST array elements malloc failed");
        free(node);
        parser->had_error = true;
        return NULL;
    }

    if (peek(parser)->type != RIGHTBRACKET) {
        do {
            if (node->as.array_literal.count == node->as.array_literal.capacity) {
                size_t new_capacity = node->as.array_literal.capacity * 2;
                AstNode** new_elements = realloc(node->as.array_literal.elements, new_capacity * sizeof(AstNode*));
                if (!new_elements) {
                    perror("AST array elements realloc failed");
                    free_ast(node);
                    parser->had_error = true;
                    return NULL;
                }
                node->as.array_literal.elements = new_elements;
                node->as.array_literal.capacity = new_capacity;
            }
            
            node->as.array_literal.elements[node->as.array_literal.count++] = parse_expression(parser, 1);
        } while (match(parser, COMMA));
    }

    expect(parser, RIGHTBRACKET, "Expected ']' after elements in array literal.");

    return node;
}

static AstNode* parse_hashtable_literal(Parser* parser) {
    AstNode* node = create_node(parser, AST_HASHTABLE_LITERAL);
    if (!node) return NULL;
    node->line = parser->tokens[parser->current - 1].line;

    node->as.hashtable_literal.capacity = 4;
    node->as.hashtable_literal.count = 0;
    node->as.hashtable_literal.pairs = malloc(node->as.hashtable_literal.capacity * sizeof(AstNodeKeyValuePair));
    if (!node->as.hashtable_literal.pairs) {
        perror("AST hashtable pairs malloc failed");
        free(node);
        parser->had_error = true;
        return NULL;
    }

    if (peek(parser)->type != RIGHTBRACE) {
        do {
            if (node->as.hashtable_literal.count >= node->as.hashtable_literal.capacity) {
                size_t new_capacity = node->as.hashtable_literal.capacity * 2;
                AstNodeKeyValuePair* new_pairs = realloc(node->as.hashtable_literal.pairs, new_capacity * sizeof(AstNodeKeyValuePair));
                if (!new_pairs) {
                    perror("AST hashtable pairs realloc failed");
                    free_ast(node);
                    parser->had_error = true;
                    return NULL;
                }
                node->as.hashtable_literal.pairs = new_pairs;
                node->as.hashtable_literal.capacity = new_capacity;
            }

            AstNodeKeyValuePair pair;
            pair.key = parse_expression(parser, 1);
            expect(parser, COLON, "Expected ':' between key and value in hashtable literal.");
            pair.value = parse_expression(parser, 1);

            node->as.hashtable_literal.pairs[node->as.hashtable_literal.count++] = pair;

        } while (match(parser, COMMA));
    }

    expect(parser, RIGHTBRACE, "Expected '}' to close hashtable literal.");
    return node;
}

static AstNode* parse_block(Parser* parser) {
    AstNode* node = create_node(parser, AST_BLOCK);
    node->line = peek(parser)->line;
    node->as.block.capacity = 8;
    node->as.block.count = 0;
    node->as.block.statements = malloc(node->as.block.capacity * sizeof(AstNode*));

    while (peek(parser)->type != RIGHTBRACE && !is_at_end(parser)) {
        if (node->as.block.count >= node->as.block.capacity) {
            size_t new_capacity = node->as.block.capacity * 2;
            AstNode** new_statements = realloc(node->as.block.statements, new_capacity * sizeof(AstNode*));
            if (!new_statements) {
                perror("AST block statements realloc failed");
                free_ast(node);
                parser->had_error = true;
                return NULL;
            }
            node->as.block.statements = new_statements;
            node->as.block.capacity = new_capacity;
        }

        AstNode* statement = parse_statement(parser);
        node->as.block.statements[node->as.block.count++] = statement;

        if (statement->type != AST_FUNCTION_DECLARATION) {
            expect(parser, SEMICOLON, "Expected ';' after statement inside block.");
            if (parser->had_error) break;
        }
    }

    expect(parser, RIGHTBRACE, "Expected '}' after block.");
    return node;
}

static AstNode* parse_return_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_RETURN_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.return_statement.keyword = parser->tokens[parser->current - 1];

    if (peek(parser)->type != SEMICOLON) {
        node->as.return_statement.value = parse_expression(parser, 1);
    } else {
        node->as.return_statement.value = NULL;
    }

    return node;
}

static AstNode* parse_call_expression(Parser* parser, AstNode* callee) {
    AstNode* node = create_node(parser, AST_CALL_EXPRESSION);
    node->line = callee->line;
    node->as.call_expression.callee = callee;
    node->as.call_expression.arg_capacity = 4;
    node->as.call_expression.arg_count = 0;
    node->as.call_expression.arguments = malloc(node->as.call_expression.arg_capacity * sizeof(AstNode*));

    if (peek(parser)->type != RIGHTPARENTHESES) {
        do {
            if (node->as.call_expression.arg_count >= node->as.call_expression.arg_capacity) {
                size_t new_capacity = node->as.call_expression.arg_capacity * 2;
                AstNode** new_args = realloc(node->as.call_expression.arguments, new_capacity * sizeof(AstNode*));
                if (!new_args) {
                    perror("AST call arguments realloc failed");
                    free_ast(node);
                    parser->had_error = true;
                    return NULL;
                }
                node->as.call_expression.arguments = new_args;
                node->as.call_expression.arg_capacity = new_capacity;
            }
            node->as.call_expression.arguments[node->as.call_expression.arg_count++] = parse_expression(parser, 1);
        } while (match(parser, COMMA));
    }

    node->as.call_expression.paren = *expect(parser, RIGHTPARENTHESES, "Expected ')' after arguments.");
    return node;
}

static AstNode* parse_function_declaration(Parser* parser, Token name) {
    AstNode* node = create_node(parser, AST_FUNCTION_DECLARATION);
    node->line = name.line;
    node->as.function_declaration.name = name;
    node->as.function_declaration.param_capacity = 4;
    node->as.function_declaration.param_count = 0;
    node->as.function_declaration.params = malloc(node->as.function_declaration.param_capacity * sizeof(Token));

    while (match(parser, PARAMETER)) {
        if (node->as.function_declaration.param_count >= node->as.function_declaration.param_capacity) {
            size_t new_capacity = node->as.function_declaration.param_capacity * 2;
            Token* new_params = realloc(node->as.function_declaration.params, new_capacity * sizeof(Token));
            if (!new_params) {
                perror("AST function parameters realloc failed");
                free_ast(node);
                parser->had_error = true;
                return NULL;
            }
            node->as.function_declaration.params = new_params;
            node->as.function_declaration.param_capacity = new_capacity;
        }
        Token param_name = *expect(parser, IDENTIFIER, "Expected parameter name after '&'.");
        node->as.function_declaration.params[node->as.function_declaration.param_count++] = param_name;
    }

    expect(parser, LEFTBRACE, "Expected '{' to begin function body.");
    node->as.function_declaration.body = parse_block(parser);

    return node;
}

static AstNode* parse_primary(Parser* parser) {
    if (match(parser, LEFTBRACE)) {
        return parse_hashtable_literal(parser);
    }

    if (match(parser, LEFTBRACKET)) {
        return parse_array_literal(parser);
    }

    if (match(parser, FORMATTEDSTART)) {
        return parse_formatted_string(parser);
    }

    if (match(parser, LEFTPARENTHESES)) {
        AstNode* expr = parse_expression(parser, 1);
        expect(parser, RIGHTPARENTHESES, "Expected ')' after expression.");
        return expr;
    }

    if (match(parser, MINUS) || match(parser, NOT)) {
        Token operator_token = parser->tokens[parser->current - 1];
        AstNode* right = parse_expression(parser, 6);
        if (!right) return NULL;

        AstNode* node = create_node(parser, AST_UNARY_OP);
        if (!node) { free_ast(right); return NULL; }
        node->line = operator_token.line;
        node->as.unary_op.operator = operator_token;
        node->as.unary_op.right = right;
        return node;
    }

    if (match(parser, STRING) || match(parser, NUMBER) || 
        match(parser, TRUEVALUE) || match(parser, FALSEVALUE) || match(parser, NULLVALUE)) 
    {
        Token literal_token = parser->tokens[parser->current - 1];
        AstNode* node = create_node(parser, AST_LITERAL);
        if (!node) return NULL;
        node->line = literal_token.line;
        node->as.literal.value = literal_token;
        return node;
    }
    
    if (match(parser, IDENTIFIER)) {
        Token identifier_token = parser->tokens[parser->current - 1];
        AstNode* node = create_node(parser, AST_IDENTIFIER);
        if (!node) return NULL;
        node->line = identifier_token.line;
        node->as.identifier.name = identifier_token;
        return node;
    }

    error_at_token(parser, peek(parser), "Expected expression.");
    return NULL;
}

static AstNode* parse_postfix(Parser* parser) {
    AstNode* expr = parse_primary(parser);
    if (!expr) return NULL;

    while (true) {
        if (match(parser, LEFTBRACKET)) {
            AstNode* index = parse_expression(parser, 1);
            expect(parser, RIGHTBRACKET, "Expected ']' after subscript index.");

            AstNode* subscript_node = create_node(parser, AST_SUBSCRIPT);
            if (!subscript_node) { free_ast(expr); free_ast(index); return NULL; }
            subscript_node->line = expr->line;
            subscript_node->as.subscript.array = expr;
            subscript_node->as.subscript.index = index;

            expr = subscript_node;
        } else if (match(parser, REFERENCE)) {
            Token op = parser->tokens[parser->current - 1];
            AstNode* key = parse_primary(parser);

            AstNode* lookup_node = create_node(parser, AST_BINARY_OP);
            if (!lookup_node) { free_ast(expr); free_ast(key); return NULL; }
            
            lookup_node->line = op.line;
            lookup_node->as.binary_op.operator = op;
            lookup_node->as.binary_op.left = expr;
            lookup_node->as.binary_op.right = key;

            expr = lookup_node;
        } else if (match(parser, LEFTPARENTHESES)) {
            expr = parse_call_expression(parser, expr);
        } else {
            break;
        }
    }

    return expr;
}

static AstNode* parse_expression(Parser* parser, int min_precedence) {
    AstNode* left = parse_postfix(parser);
    if (!left) return NULL;

    while (true) {
        TokenType op_type = peek(parser)->type;
        int current_precedence = get_operator_precedence(op_type);

        if (current_precedence == 0 || current_precedence < min_precedence) {
            break;
        }

        if (op_type == QUESTIONMARK) {
            consume(parser);
            AstNode* then_expr = parse_expression(parser, 1);
            expect(parser, COLON, "Expected ':' for ternary operator.");
            
            AstNode* else_expr = parse_expression(parser, current_precedence - 1);

            AstNode* ternary_node = create_node(parser, AST_TERNARY_EXPRESSION);
            ternary_node->line = left->line;
            ternary_node->as.ternary_expression.condition = left;
            ternary_node->as.ternary_expression.then_expr = then_expr;
            ternary_node->as.ternary_expression.else_expr = else_expr;
            left = ternary_node;
            continue;
        }
        
        Token operator_token = *consume(parser);
        AstNode* right = parse_expression(parser, current_precedence + 1);
        if (!right) { free_ast(left); return NULL; }

        AstNode* node;
        if (operator_token.type == AND || operator_token.type == OR) {
            node = create_node(parser, AST_LOGICAL_OP);
            node->as.logical_op.operator = operator_token;
            node->as.logical_op.left = left;
            node->as.logical_op.right = right;
        } else {
            node = create_node(parser, AST_BINARY_OP);
            node->as.binary_op.operator = operator_token;
            node->as.binary_op.left = left;
            node->as.binary_op.right = right;
        }
        node->line = operator_token.line;
        left = node;
    }

    return left;
}

static AstNode* parse_print_statement(Parser* parser) {
    int line = parser->tokens[parser->current - 1].line;

    AstNode* node = create_node(parser, AST_PRINT_STATEMENT);
    if (!node) return NULL;
    node->line = line;

    node->as.print_stmt.capacity = 4;
    node->as.print_stmt.count = 0;
    node->as.print_stmt.expressions = malloc(node->as.print_stmt.capacity * sizeof(AstNode*));
    if (!node->as.print_stmt.expressions) {
        perror("AST print expressions malloc failed");
        free(node);
        parser->had_error = true;
        return NULL;
    }

    do {
        if (node->as.print_stmt.count == node->as.print_stmt.capacity) {
            size_t new_capacity = node->as.print_stmt.capacity * 2;
            AstNode** new_expressions = realloc(node->as.print_stmt.expressions, new_capacity * sizeof(AstNode*));
            if (!new_expressions) {
                perror("AST print expressions realloc failed");
                parser->had_error = true;
                return NULL;
            }
            node->as.print_stmt.expressions = new_expressions;
            node->as.print_stmt.capacity = new_capacity;
        }
        
        node->as.print_stmt.expressions[node->as.print_stmt.count++] = parse_expression(parser, 1);

    } while (match(parser, COMMA));

    return node;
}

static bool is_compound_assignment(TokenType type) {
    switch (type) {
        case PLUSASSIGNMENT:
        case SUBTRACTIONASSIGNMENT:
        case MULTIPLICATIONASSIGNMENT:
        case DIVISIONASSIGNMENT:
        case EXPONENTIATIONASSIGNMENT:
            return true;
        default:
            return false;
    }
}

static TokenType get_base_operator(TokenType compound_type) {
    switch (compound_type) {
        case PLUSASSIGNMENT:         return PLUS;
        case SUBTRACTIONASSIGNMENT:      return MINUS;
        case MULTIPLICATIONASSIGNMENT:   return MULTIPLICATION;
        case DIVISIONASSIGNMENT:         return DIVISION;
        case EXPONENTIATIONASSIGNMENT:   return EXPONENTIATION;
        default:                         return UNKNOWN;
    }
}

static AstNode* parse_compound_assignment(Parser* parser) {
    Token identifier = *consume(parser);
    Token compound_op = *consume(parser);
    AstNode* right_side = parse_expression(parser, 1);
    if (!right_side) return NULL;

    AstNode* left_side = create_node(parser, AST_IDENTIFIER);
    left_side->line = identifier.line;
    left_side->as.identifier.name = identifier;
    
    AstNode* binary_op_node = create_node(parser, AST_BINARY_OP);
    binary_op_node->line = compound_op.line;

    TokenType base_op_type = get_base_operator(compound_op.type);
    binary_op_node->as.binary_op.operator.type = base_op_type;
    switch (base_op_type) {
        case PLUS:       strcpy(binary_op_node->as.binary_op.operator.lexeme, "+"); break;
        case MINUS:          strcpy(binary_op_node->as.binary_op.operator.lexeme, "-"); break;
        case MULTIPLICATION: strcpy(binary_op_node->as.binary_op.operator.lexeme, "*"); break;
        case DIVISION:       strcpy(binary_op_node->as.binary_op.operator.lexeme, "/"); break;
        case EXPONENTIATION: strcpy(binary_op_node->as.binary_op.operator.lexeme, "**"); break;
        default:             binary_op_node->as.binary_op.operator.lexeme[0] = '\0'; break;
    }

    binary_op_node->as.binary_op.left = left_side;
    binary_op_node->as.binary_op.right = right_side;

    AstNode* assignment_node = create_node(parser, AST_ASSIGNMENT);
    assignment_node->line = identifier.line;

    AstNode* assignment_target = create_node(parser, AST_IDENTIFIER);
    assignment_target->line = identifier.line;
    assignment_target->as.identifier.name = identifier;
    assignment_node->as.assignment.left = assignment_target;    
    assignment_node->as.assignment.value = binary_op_node;

    return assignment_node;
}

static AstNode* parse_inc_dec_statement(Parser* parser) {
    Token identifier = *consume(parser);
    Token op = *consume(parser);

    AstNode* left_side = create_node(parser, AST_IDENTIFIER);
    if (!left_side) return NULL;
    left_side->line = identifier.line;
    left_side->as.identifier.name = identifier;

    AstNode* right_side = create_node(parser, AST_LITERAL);
    if (!right_side) { free_ast(left_side); return NULL; }
    right_side->line = op.line;
    right_side->as.literal.value.type = NUMBER;
    strcpy(right_side->as.literal.value.lexeme, "1");

    AstNode* binary_op_node = create_node(parser, AST_BINARY_OP);
    if (!binary_op_node) { free_ast(left_side); free_ast(right_side); return NULL; }
    binary_op_node->line = op.line;
    binary_op_node->as.binary_op.operator.type = (op.type == INCREMENT) ? PLUS : MINUS;
    strcpy(binary_op_node->as.binary_op.operator.lexeme, (op.type == INCREMENT) ? "+" : "-");
    binary_op_node->as.binary_op.left = left_side;
    binary_op_node->as.binary_op.right = right_side;

    AstNode* assignment_node = create_node(parser, AST_ASSIGNMENT);
    if (!assignment_node) { free_ast(binary_op_node); return NULL; }
    assignment_node->line = identifier.line;

    AstNode* assignment_target = create_node(parser, AST_IDENTIFIER);
    assignment_target->line = identifier.line;
    assignment_target->as.identifier.name = identifier;

    assignment_node->as.assignment.left = assignment_target;

    assignment_node->as.assignment.value = binary_op_node;

    return assignment_node;
}

static AstNode* parse_if_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_IF_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;

    node->as.if_statement.condition = parse_expression(parser, 1);
    expect(parser, LEFTBRACE, "Expected '{' after if condition.");
    node->as.if_statement.then_branch = parse_block(parser);

    node->as.if_statement.else_if_capacity = 4;
    node->as.if_statement.else_if_count = 0;
    node->as.if_statement.else_if_clauses = malloc(node->as.if_statement.else_if_capacity * sizeof(AstNodeElseIfClause));
    
    while (match(parser, COMMA)) {
        if (node->as.if_statement.else_if_count >= node->as.if_statement.else_if_capacity) {
            size_t new_capacity = node->as.if_statement.else_if_capacity * 2;
            AstNodeElseIfClause* new_clauses = realloc(node->as.if_statement.else_if_clauses, new_capacity * sizeof(AstNodeElseIfClause));
            if (!new_clauses) {
                perror("AST if-else clauses realloc failed");
                free_ast(node);
                parser->had_error = true;
                return NULL;
            }
            node->as.if_statement.else_if_clauses = new_clauses;
            node->as.if_statement.else_if_capacity = new_capacity;
        }
        AstNodeElseIfClause* clause = &node->as.if_statement.else_if_clauses[node->as.if_statement.else_if_count++];
        clause->condition = parse_expression(parser, 1);
        expect(parser, LEFTBRACE, "Expected '{' after else-if condition.");
        clause->body = parse_block(parser);
    }
    
    if (match(parser, COLON)) {
        expect(parser, LEFTBRACE, "Expected '{' after else colon.");
        node->as.if_statement.else_branch = parse_block(parser);
    } else {
        node->as.if_statement.else_branch = NULL;
    }

    return node;
}

static AstNode* parse_statement(Parser* parser) {
    if (match(parser, QUESTIONMARK)) {
        return parse_if_statement(parser);
    }

    if (match(parser, RETURN)) {
        return parse_return_statement(parser);
    }
    
    if (match(parser, PRINT)) {
        return parse_print_statement(parser);
    }
    
    if (peek(parser)->type == IDENTIFIER &&
       (parser->tokens[parser->current + 1].type == PARAMETER ||
        parser->tokens[parser->current + 1].type == LEFTBRACE)) {
        Token name = *consume(parser);
        return parse_function_declaration(parser, name);
    }

    AstNode* expr = parse_expression(parser, 1);
    if (!expr) {
        return NULL;
    }

    if (match(parser, ASSIGNMENT)) {
        if (expr->type != AST_IDENTIFIER &&
            expr->type != AST_SUBSCRIPT &&
            !(expr->type == AST_BINARY_OP && expr->as.binary_op.operator.type == REFERENCE)) {
            error_at_token(parser, &parser->tokens[parser->current - 1], "Invalid assignment target.");
            free_ast(expr);
            return NULL;
        }

        AstNode* value = parse_expression(parser, 1);
        if (!value) {
            free_ast(expr);
            return NULL;
        }
        
        AstNode* assignment_node = create_node(parser, AST_ASSIGNMENT);
        assignment_node->line = expr->line;
        assignment_node->as.assignment.left = expr;
        assignment_node->as.assignment.value = value;
        
        AstNode* stmt_node = create_node(parser, AST_EXPRESSION_STATEMENT);
        stmt_node->line = assignment_node->line;
        stmt_node->as.expression_statement.expression = assignment_node;
        return stmt_node;
    }
    
    if (expr->type == AST_CALL_EXPRESSION) {
        AstNode* stmt_node = create_node(parser, AST_EXPRESSION_STATEMENT);
        stmt_node->line = expr->line;
        stmt_node->as.expression_statement.expression = expr;
        return stmt_node;
    }

    error_at_token(parser, peek(parser), "Invalid use of expression as a statement.");
    free_ast(expr);
    return NULL;
}

bool parse(Graveyard *gy) {
    Parser parser;
    parser.gy = gy;
    parser.tokens = gy->tokens;
    parser.token_count = gy->token_count;
    parser.current = 0;
    parser.had_error = false;

    AstNode* root = create_node(&parser, AST_PROGRAM);
    if (!root) return false;
    root->line = 0;

    root->as.program.capacity = 8;
    root->as.program.count = 0;
    root->as.program.statements = malloc(root->as.program.capacity * sizeof(AstNode*));
    if (!root->as.program.statements) {
        perror("AST statements malloc failed");
        free(root);
        parser.had_error = true;
        return false;
    }
    
    gy->ast_root = root;

    while (!is_at_end(&parser)) {
        AstNode* statement = parse_statement(&parser);
        if (parser.had_error) {
            goto cleanup;
        }

        if (statement->type != AST_FUNCTION_DECLARATION && statement->type != AST_IF_STATEMENT) {
            expect(&parser, SEMICOLON, "Expected ';' at the end of the statement.");
            if (parser.had_error) goto cleanup;
        }

        if (root->as.program.count == root->as.program.capacity) {
            size_t new_capacity = root->as.program.capacity * 2;
            AstNode** new_statements = realloc(root->as.program.statements, new_capacity * sizeof(AstNode*));
            if (!new_statements) {
                perror("AST statements realloc failed");
                parser.had_error = true;
                goto cleanup;
            }
            root->as.program.statements = new_statements;
            root->as.program.capacity = new_capacity;
        }
        root->as.program.statements[root->as.program.count++] = statement;
    }

cleanup:
    if (parser.had_error) {
        free_ast(gy->ast_root); 
        gy->ast_root = NULL;
    }
    
    return !parser.had_error;
}

//COMPILE (AST SERIALIZATION TO FILE)--------------------------------------------------

static void write_ast_node(FILE* file, AstNode* node, int indent) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < indent; ++i) {
        fprintf(file, "  ");
    }

    switch (node->type) {
        case AST_PROGRAM:
            fprintf(file, "(PROGRAM line=1\n");
            for (size_t i = 0; i < node->as.program.count; i++) {
                write_ast_node(file, node->as.program.statements[i], indent + 1);
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;

        case AST_PRINT_STATEMENT:
            fprintf(file, "(PRINT_STATEMENT line=%d\n", node->line);
            for (size_t i = 0; i < node->as.print_stmt.count; i++) {
                write_ast_node(file, node->as.print_stmt.expressions[i], indent + 1);
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;

        case AST_LOGICAL_OP:
            fprintf(file, "(LOGICAL_OP op=\"%s\" line=%d\n", node->as.logical_op.operator.lexeme, node->line);
            write_ast_node(file, node->as.logical_op.left, indent + 1);
            write_ast_node(file, node->as.logical_op.right, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;

        case AST_BINARY_OP:
            fprintf(file, "(BINARY_OP op=\"%s\" line=%d\n", node->as.binary_op.operator.lexeme, node->line);
            write_ast_node(file, node->as.binary_op.left, indent + 1);
            write_ast_node(file, node->as.binary_op.right, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;

        case AST_UNARY_OP:
            fprintf(file, "(UNARY_OP op=\"%s\" line=%d\n", node->as.unary_op.operator.lexeme, node->line);
            write_ast_node(file, node->as.unary_op.right, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;

        case AST_ASSIGNMENT:
            fprintf(file, "(ASSIGN line=%d\n", node->line);
            write_ast_node(file, node->as.assignment.left, indent + 1);
            write_ast_node(file, node->as.assignment.value, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;

        case AST_IDENTIFIER:
            fprintf(file, "(IDENTIFIER name=\"%s\" line=%d)\n", node->as.identifier.name.lexeme, node->line);
            break;
            
        case AST_FORMATTED_STRING: {
            fprintf(file, "(FORMATTED_STRING line=%d\n", node->line);
            for (size_t i = 0; i < node->as.formatted_string.count; i++) {
                FmtStringPart part = node->as.formatted_string.parts[i];
                if (part.type == FMT_PART_LITERAL) {
                    for (int j = 0; j < indent + 1; ++j) { fprintf(file, "  "); }
                    fprintf(file, "(FORMATTEDPART value=\"%s\")\n", part.as.literal.lexeme);
                } else {
                    write_ast_node(file, part.as.expression, indent + 1);
                }
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_ARRAY_LITERAL: {
            fprintf(file, "(ARRAY_LITERAL line=%d\n", node->line);
            for (size_t i = 0; i < node->as.array_literal.count; i++) {
                write_ast_node(file, node->as.array_literal.elements[i], indent + 1);
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_HASHTABLE_LITERAL: {
            fprintf(file, "(HASHTABLE_LITERAL line=%d\n", node->line);
            for (size_t i = 0; i < node->as.hashtable_literal.count; i++) {
                for (int j = 0; j < indent + 1; ++j) { fprintf(file, "  "); }
                fprintf(file, "(KEY_VALUE_PAIR\n");
                
                write_ast_node(file, node->as.hashtable_literal.pairs[i].key, indent + 2);
                write_ast_node(file, node->as.hashtable_literal.pairs[i].value, indent + 2);

                for (int j = 0; j < indent + 1; ++j) { fprintf(file, "  "); }
                fprintf(file, ")\n");
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_SUBSCRIPT: {
            fprintf(file, "(SUBSCRIPT line=%d\n", node->line);
            write_ast_node(file, node->as.subscript.array, indent + 1);
            write_ast_node(file, node->as.subscript.index, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_LITERAL: {
            Token literal_token = node->as.literal.value;
            switch (literal_token.type) {
                case STRING:
                    fprintf(file, "(LITERAL_STR value=\"%s\" line=%d)\n", literal_token.lexeme, node->line);
                    break;
                case NUMBER:
                    fprintf(file, "(LITERAL_NUM value=\"%s\" line=%d)\n", literal_token.lexeme, node->line);
                    break;
                case TRUEVALUE:
                    fprintf(file, "(LITERAL_BOOL value=\"$\" line=%d)\n", node->line);
                    break;
                case FALSEVALUE:
                    fprintf(file, "(LITERAL_BOOL value=\"%%\" line=%d)\n", node->line);
                    break;
                case NULLVALUE:
                    fprintf(file, "(LITERAL_NULL line=%d)\n", node->line);
                    break;
                default:
                    fprintf(file, "(LITERAL_UNKNOWN)\n");
                    break;
            }
            break;
        }

        case AST_EXPRESSION_STATEMENT: {
            fprintf(file, "(EXPRESSION_STATEMENT line=%d\n", node->line);
            write_ast_node(file, node->as.expression_statement.expression, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_BLOCK: {
            fprintf(file, "(BLOCK line=%d\n", node->line);
            for (size_t i = 0; i < node->as.block.count; i++) {
                write_ast_node(file, node->as.block.statements[i], indent + 1);
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_RETURN_STATEMENT: {
            fprintf(file, "(RETURN_STATEMENT line=%d\n", node->line);
            if (node->as.return_statement.value != NULL) {
                write_ast_node(file, node->as.return_statement.value, indent + 1);
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_FUNCTION_DECLARATION: {
            char params_str[256] = "";
            for (size_t i = 0; i < node->as.function_declaration.param_count; i++) {
                strcat(params_str, node->as.function_declaration.params[i].lexeme);
                if (i < node->as.function_declaration.param_count - 1) {
                    strcat(params_str, " ");
                }
            }

            fprintf(file, "(FUNCTION_DECLARATION name=\"%s\" params=\"%s\" line=%d\n",
                node->as.function_declaration.name.lexeme,
                params_str,
                node->line);
            
            write_ast_node(file, node->as.function_declaration.body, indent + 1);
            
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_CALL_EXPRESSION: {
            fprintf(file, "(CALL_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.call_expression.callee, indent + 1);
            for (size_t i = 0; i < node->as.call_expression.arg_count; i++) {
                write_ast_node(file, node->as.call_expression.arguments[i], indent + 1);
            }
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_IF_STATEMENT: {
            fprintf(file, "(IF_STATEMENT line=%d\n", node->line);

            for (int i = 0; i < indent + 1; ++i) { fprintf(file, "  "); }
            fprintf(file, "(IF_CONDITION\n");
            write_ast_node(file, node->as.if_statement.condition, indent + 2);
            for (int i = 0; i < indent + 1; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");

            for (int i = 0; i < indent + 1; ++i) { fprintf(file, "  "); }
            fprintf(file, "(THEN_BRANCH\n");
            write_ast_node(file, node->as.if_statement.then_branch, indent + 2);
            for (int i = 0; i < indent + 1; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");

            for (size_t i = 0; i < node->as.if_statement.else_if_count; i++) {
                for (int j = 0; j < indent + 1; ++j) { fprintf(file, "  "); }
                fprintf(file, "(ELSE_IF_CLAUSE\n");
                write_ast_node(file, node->as.if_statement.else_if_clauses[i].condition, indent + 2);
                write_ast_node(file, node->as.if_statement.else_if_clauses[i].body, indent + 2);
                for (int j = 0; j < indent + 1; ++j) { fprintf(file, "  "); }
                fprintf(file, ")\n");
            }

            if (node->as.if_statement.else_branch) {
                for (int i = 0; i < indent + 1; ++i) { fprintf(file, "  "); }
                fprintf(file, "(ELSE_BRANCH\n");
                write_ast_node(file, node->as.if_statement.else_branch, indent + 2);
                for (int i = 0; i < indent + 1; ++i) { fprintf(file, "  "); }
                fprintf(file, ")\n");
            }

            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_TERNARY_EXPRESSION: {
            fprintf(file, "(TERNARY_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.ternary_expression.condition, indent + 1);
            write_ast_node(file, node->as.ternary_expression.then_expr, indent + 1);
            write_ast_node(file, node->as.ternary_expression.else_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        default:
             fprintf(file, "(UNKNOWN_NODE type=%d line=%d)\n", node->type, node->line);
             break;
    }
}

bool save_ast_to_file(Graveyard* gy, const char* out_filename) {
    FILE* file = fopen(out_filename, "w");
    if (!file) {
        perror("save_ast_to_file: Could not open file for writing");
        return false;
    }

    printf("Writing AST to %s...\n", out_filename);
    
    write_ast_node(file, gy->ast_root, 0);

    fclose(file);
    return true;
}

void print_ast(AstNode* root) {
    printf("--- In-Memory AST ---\n");
    if (root == NULL) {
        printf("(NULL)\n");
        return;
    }
    write_ast_node(stdout, root, 0);
    printf("--- End AST ---\n");
}

//DESERIALIZER-----------------------------------------------------

static int get_indent_level(const char* line) {
    int indent = 0;
    while (line[indent] == ' ') {
        indent++;
    }
    return indent / 2;
}

static bool get_node_type_from_line(const char* line, char* out_type_str, size_t buffer_size) {
    const char* start = strchr(line, '(');
    if (!start) return false;
    start++;

    const char* end = start;
    while (*end && *end != ' ' && *end != '\n' && *end != ')') {
        end++;
    }

    size_t len = end - start;
    if (len == 0 || len >= buffer_size) return false;

    strncpy(out_type_str, start, len);
    out_type_str[len] = '\0';
    return true;
}

static bool get_attribute_string(const char* line, const char* key, char* out_buffer, size_t buffer_size) {
    const char* key_ptr = strstr(line, key);
    if (!key_ptr) return false;

    const char* value_start = key_ptr + strlen(key);
    if (*value_start != '"') return false;
    value_start++;

    const char* value_end = strchr(value_start, '"');
    if (!value_end) return false;

    size_t len = value_end - value_start;
    if (len >= buffer_size) return false;

    strncpy(out_buffer, value_start, len);
    out_buffer[len] = '\0';
    return true;
}

static int get_attribute_int(const char* line, const char* key) {
    const char* key_ptr = strstr(line, key);
    if (!key_ptr) return -1;

    const char* value_start = key_ptr + strlen(key);
    return atoi(value_start);
}

static AstNodeType get_node_type_from_string(const char* type_str) {
    if (strcmp(type_str, "PROGRAM") == 0) return AST_PROGRAM;
    if (strcmp(type_str, "ASSIGN") == 0) return AST_ASSIGNMENT;
    if (strcmp(type_str, "IDENTIFIER") == 0) return AST_IDENTIFIER;
    if (strcmp(type_str, "FORMATTED_STRING") == 0) return AST_FORMATTED_STRING;
    if (strcmp(type_str, "LITERAL_STR") == 0) return AST_LITERAL;
    if (strcmp(type_str, "LITERAL_NUM") == 0) return AST_LITERAL;
    if (strcmp(type_str, "LITERAL_BOOL") == 0) return AST_LITERAL;
    if (strcmp(type_str, "LITERAL_NULL") == 0) return AST_LITERAL;
    if (strcmp(type_str, "LOGICAL_OP") == 0) return AST_LOGICAL_OP;
    if (strcmp(type_str, "BINARY_OP") == 0) return AST_BINARY_OP;
    if (strcmp(type_str, "UNARY_OP") == 0) return AST_UNARY_OP;
    if (strcmp(type_str, "PRINT_STATEMENT") == 0) return AST_PRINT_STATEMENT;
    if (strcmp(type_str, "ARRAY_LITERAL") == 0) return AST_ARRAY_LITERAL;
    if (strcmp(type_str, "SUBSCRIPT") == 0) return AST_SUBSCRIPT;
    if (strcmp(type_str, "HASHTABLE_LITERAL") == 0) return AST_HASHTABLE_LITERAL;
    if (strcmp(type_str, "EXPRESSION_STATEMENT") == 0) return AST_EXPRESSION_STATEMENT;
    if (strcmp(type_str, "BLOCK") == 0) return AST_BLOCK;
    if (strcmp(type_str, "RETURN_STATEMENT") == 0) return AST_RETURN_STATEMENT;
    if (strcmp(type_str, "FUNCTION_DECLARATION") == 0) return AST_FUNCTION_DECLARATION;
    if (strcmp(type_str, "CALL_EXPRESSION") == 0) return AST_CALL_EXPRESSION;
    if (strcmp(type_str, "KEY_VALUE_PAIR") == 0) return AST_UNKNOWN;
    if (strcmp(type_str, "IF_STATEMENT") == 0) return AST_IF_STATEMENT;
    if (strcmp(type_str, "TERNARY_EXPRESSION") == 0) return AST_TERNARY_EXPRESSION;
    return AST_UNKNOWN;
}

static AstNode* parse_node_recursive(Lines* lines, int* current_line_idx, int expected_indent, Parser* dummy_parser_for_node_creation);

bool load_ast_from_file(Graveyard* gy, const char* filename) {
    long file_size = 0;
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("load_ast_from_file: Could not open file");
        return false;
    }
    char* buffer = load(file, &file_size);
    fclose(file);
    if (!buffer) return false;

    Lines lines;
    lines.count = 0;
    lines.lines = malloc(sizeof(char*));
    int capacity = 1;

    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        if (lines.count == capacity) {
            capacity *= 2;
            lines.lines = realloc(lines.lines, capacity * sizeof(char*));
        }
        lines.lines[lines.count++] = line;
        line = strtok(NULL, "\n");
    }

    int current_line = 0;
    Parser dummy_parser = {0};
    gy->ast_root = parse_node_recursive(&lines, &current_line, 0, &dummy_parser);
    
    free(lines.lines);
    free(buffer);

    if (dummy_parser.had_error) {
        free_ast(gy->ast_root);
        gy->ast_root = NULL;
        return false;
    }

    return gy->ast_root != NULL;
}

static AstNode* parse_node_recursive(Lines* lines, int* current_line_idx, int expected_indent, Parser* parser) {
    if (*current_line_idx >= lines->count) return NULL;

    const char* line = lines->lines[*current_line_idx];
    int current_indent = get_indent_level(line);

    if (current_indent < expected_indent) return NULL;

    if (*(line + current_indent * 2) == ')') return NULL;
    
    if (current_indent > expected_indent) {
        fprintf(stderr, "AST Deserializer Error [line %d]: Unexpected indentation.\n", *current_line_idx + 1);
        parser->had_error = true;
        return NULL;
    }

    int node_start_line_for_errors = *current_line_idx + 1;
    (*current_line_idx)++;

    char type_str[64];
    if (!get_node_type_from_line(line, type_str, sizeof(type_str))) {
        fprintf(stderr, "AST Deserializer Error [line %d]: Malformed node, expected '('.\n", node_start_line_for_errors);
        parser->had_error = true;
        return NULL;
    }

    AstNodeType type = get_node_type_from_string(type_str);
    if (type == AST_UNKNOWN) {
        fprintf(stderr, "AST Deserializer Error [line %d]: Unknown node type '%s'.\n", node_start_line_for_errors, type_str);
        parser->had_error = true;
        return NULL;
    }

    AstNode* node = create_node(parser, type);
    if (!node) return NULL;
    node->line = get_attribute_int(line, "line=");

    switch (type) {
        case AST_PROGRAM:

        case AST_PRINT_STATEMENT:

        case AST_ARRAY_LITERAL: {
            AstNode** list_ptr = NULL; size_t* count_ptr = NULL; size_t* capacity_ptr = NULL;
            if (type == AST_PROGRAM) {
                node->as.program.capacity = 8; node->as.program.count = 0;
                node->as.program.statements = malloc(node->as.program.capacity * sizeof(AstNode*));
                list_ptr = node->as.program.statements; count_ptr = &node->as.program.count; capacity_ptr = &node->as.program.capacity;
            } else if (type == AST_PRINT_STATEMENT) {
                node->as.print_stmt.capacity = 4; node->as.print_stmt.count = 0;
                node->as.print_stmt.expressions = malloc(node->as.print_stmt.capacity * sizeof(AstNode*));
                list_ptr = node->as.print_stmt.expressions; count_ptr = &node->as.print_stmt.count; capacity_ptr = &node->as.print_stmt.capacity;
            } else {
                node->as.array_literal.capacity = 4; node->as.array_literal.count = 0;
                node->as.array_literal.elements = malloc(node->as.array_literal.capacity * sizeof(AstNode*));
                list_ptr = node->as.array_literal.elements; count_ptr = &node->as.array_literal.count; capacity_ptr = &node->as.array_literal.capacity;
            }
            if (!list_ptr) { parser->had_error = true; free(node); return NULL; }

            AstNode* child;
            while ((child = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser))) {
                if (*count_ptr == *capacity_ptr) {
                    *capacity_ptr *= 2;
                    list_ptr = realloc(list_ptr, *capacity_ptr * sizeof(AstNode*));
                    if (!list_ptr) { parser->had_error = true; free_ast(child); free_ast(node); return NULL; }
                    if (type == AST_PROGRAM) node->as.program.statements = list_ptr;
                    else node->as.print_stmt.expressions = list_ptr;
                }
                list_ptr[(*count_ptr)++] = child;
            }
            break;
        }

        case AST_HASHTABLE_LITERAL: {
            node->as.hashtable_literal.capacity = 4;
            node->as.hashtable_literal.count = 0;
            node->as.hashtable_literal.pairs = malloc(node->as.hashtable_literal.capacity * sizeof(AstNodeKeyValuePair));
            if (!node->as.hashtable_literal.pairs) { parser->had_error = true; free(node); return NULL; }

            while (*current_line_idx < lines->count && get_indent_level(lines->lines[*current_line_idx]) > expected_indent) {
                const char* pair_line = lines->lines[*current_line_idx];
                char part_type_str[64];
                get_node_type_from_line(pair_line, part_type_str, sizeof(part_type_str));

                if (strcmp(part_type_str, "KEY_VALUE_PAIR") == 0) {
                    (*current_line_idx)++;

                    if (node->as.hashtable_literal.count >= node->as.hashtable_literal.capacity) {
                        size_t new_capacity = node->as.hashtable_literal.capacity * 2;
                        node->as.hashtable_literal.pairs = realloc(node->as.hashtable_literal.pairs, new_capacity * sizeof(AstNodeKeyValuePair));
                        node->as.hashtable_literal.capacity = new_capacity;
                    }
                    
                    AstNodeKeyValuePair pair;
                    pair.key = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                    pair.value = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                    node->as.hashtable_literal.pairs[node->as.hashtable_literal.count++] = pair;

                    (*current_line_idx)++; 
                } else {
                    break;
                }
            }
            break;
        }

        case AST_FORMATTED_STRING: {
            node->as.formatted_string.capacity = 4;
            node->as.formatted_string.count = 0;
            node->as.formatted_string.parts = malloc(node->as.formatted_string.capacity * sizeof(FmtStringPart));
            if (!node->as.formatted_string.parts) { parser->had_error = true; free(node); return NULL; }

            while (*current_line_idx < lines->count && get_indent_level(lines->lines[*current_line_idx]) > expected_indent) {
                const char* part_line = lines->lines[*current_line_idx];
                char part_type_str[64];
                get_node_type_from_line(part_line, part_type_str, sizeof(part_type_str));

                FmtStringPart part;
                if (strcmp(part_type_str, "FORMATTEDPART") == 0) {
                    part.type = FMT_PART_LITERAL;
                    get_attribute_string(part_line, "value=", part.as.literal.lexeme, MAX_LEXEME_LEN);
                    part.as.literal.type = FORMATTEDSTRING;
                    node->as.formatted_string.parts[node->as.formatted_string.count++] = part;
                    (*current_line_idx)++;
                } else {
                    AstNode* expr_node = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
                    if (expr_node) {
                        part.type = FMT_PART_EXPRESSION;
                        part.as.expression = expr_node;
                        node->as.formatted_string.parts[node->as.formatted_string.count++] = part;
                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case AST_ASSIGNMENT: {
            node->as.assignment.left = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.assignment.value = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_BINARY_OP: {
            get_attribute_string(line, "op=", node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN);
            char* op_str = node->as.binary_op.operator.lexeme;
            size_t op_len = strlen(op_str);
            if (op_len == 3) node->as.binary_op.operator.type = identify_three_char_token(op_str[0], op_str[1], op_str[2]);
            else if (op_len == 2) node->as.binary_op.operator.type = identify_two_char_token(op_str[0], op_str[1]);
            else if (op_len == 1) node->as.binary_op.operator.type = identify_single_char_token(op_str[0]);
            node->as.binary_op.left = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.binary_op.right = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_LOGICAL_OP: {
            get_attribute_string(line, "op=", node->as.logical_op.operator.lexeme, MAX_LEXEME_LEN);
            
            char* op_str = node->as.logical_op.operator.lexeme;
            size_t op_len = strlen(op_str);
            if (op_len == 2) {
                node->as.logical_op.operator.type = identify_two_char_token(op_str[0], op_str[1]);
            } else {
                node->as.logical_op.operator.type = UNKNOWN;
            }

            node->as.logical_op.left = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.logical_op.right = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_UNARY_OP: {
            get_attribute_string(line, "op=", node->as.unary_op.operator.lexeme, MAX_LEXEME_LEN);
            if (strlen(node->as.unary_op.operator.lexeme) == 1) {
                node->as.unary_op.operator.type = identify_single_char_token(node->as.unary_op.operator.lexeme[0]);
            }
            node->as.unary_op.right = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_LITERAL: {
            get_attribute_string(line, "value=", node->as.literal.value.lexeme, MAX_LEXEME_LEN);
            if (strcmp(type_str, "LITERAL_STR") == 0) {
                node->as.literal.value.type = STRING;
            } else if (strcmp(type_str, "LITERAL_NUM") == 0) {
                node->as.literal.value.type = NUMBER;
            } else if (strcmp(type_str, "LITERAL_BOOL") == 0) {
                if (strcmp(node->as.literal.value.lexeme, "$") == 0) node->as.literal.value.type = TRUEVALUE;
                else node->as.literal.value.type = FALSEVALUE;
            } else if (strcmp(type_str, "LITERAL_NULL") == 0) {
                node->as.literal.value.type = NULLVALUE;
            }
            break;
        }

        case AST_IDENTIFIER: {
            get_attribute_string(line, "name=", node->as.identifier.name.lexeme, MAX_LEXEME_LEN);
            break;
        }

        case AST_SUBSCRIPT: {
            node->as.subscript.array = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.subscript.index = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_EXPRESSION_STATEMENT: {
            node->as.expression_statement.expression = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_BLOCK: {
            node->as.block.capacity = 8;
            node->as.block.count = 0;
            node->as.block.statements = malloc(node->as.block.capacity * sizeof(AstNode*));
            if (!node->as.block.statements) { parser->had_error = true; free(node); return NULL; }

            AstNode* child;
            while ((child = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser))) {
                if (node->as.block.count >= node->as.block.capacity) {
                    size_t new_capacity = node->as.block.capacity * 2;
                    AstNode** new_statements = realloc(node->as.block.statements, new_capacity * sizeof(AstNode*));
                    if (!new_statements) {
                        perror("AST block statements realloc failed");
                        free_ast(node);
                        parser->had_error = true;
                        return NULL;
                    }
                    node->as.block.statements = new_statements;
                    node->as.block.capacity = new_capacity;
                }
                node->as.block.statements[node->as.block.count++] = child;
            }
            break;
        }

        case AST_RETURN_STATEMENT: {
            if (*current_line_idx < lines->count && get_indent_level(lines->lines[*current_line_idx]) > expected_indent) {
                node->as.return_statement.value = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            } else {
                node->as.return_statement.value = NULL;
            }
            break;
        }

        case AST_FUNCTION_DECLARATION: {
            get_attribute_string(line, "name=", node->as.function_declaration.name.lexeme, MAX_LEXEME_LEN);
            node->as.function_declaration.name.type = IDENTIFIER;

            char params_buffer[256];
            get_attribute_string(line, "params=", params_buffer, sizeof(params_buffer));
            
            node->as.function_declaration.param_capacity = 4;
            node->as.function_declaration.param_count = 0;
            node->as.function_declaration.params = malloc(node->as.function_declaration.param_capacity * sizeof(Token));

            char* param_name = strtok(params_buffer, " ");
            while (param_name != NULL) {
                if (node->as.function_declaration.param_count >= node->as.function_declaration.param_capacity) {
                    size_t new_capacity = node->as.function_declaration.param_capacity * 2;
                    Token* new_params = realloc(node->as.function_declaration.params, new_capacity * sizeof(Token));
                    if (!new_params) {
                        perror("AST function params realloc failed");
                        free_ast(node);
                        parser->had_error = true;
                        return NULL;
                    }
                    node->as.function_declaration.params = new_params;
                    node->as.function_declaration.param_capacity = new_capacity;
                }
                Token param_token;
                param_token.type = IDENTIFIER;
                strncpy(param_token.lexeme, param_name, MAX_LEXEME_LEN);
                node->as.function_declaration.params[node->as.function_declaration.param_count++] = param_token;
                param_name = strtok(NULL, " ");
            }

            node->as.function_declaration.body = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_CALL_EXPRESSION: {
            node->as.call_expression.callee = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            
            node->as.call_expression.arg_capacity = 4;
            node->as.call_expression.arg_count = 0;
            node->as.call_expression.arguments = malloc(node->as.call_expression.arg_capacity * sizeof(AstNode*));
            if (!node->as.call_expression.arguments) { parser->had_error = true; free_ast(node); return NULL; }

            AstNode* child;
            while ((child = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser))) {
                if (node->as.call_expression.arg_count >= node->as.call_expression.arg_capacity) {
                    size_t new_capacity = node->as.call_expression.arg_capacity * 2;
                    AstNode** new_args = realloc(node->as.call_expression.arguments, new_capacity * sizeof(AstNode*));
                    if (!new_args) {
                        perror("AST call arguments realloc failed");
                        free_ast(node);
                        parser->had_error = true;
                        return NULL;
                    }
                    node->as.call_expression.arguments = new_args;
                    node->as.call_expression.arg_capacity = new_capacity;
                }
                node->as.call_expression.arguments[node->as.call_expression.arg_count++] = child;
            }
            break;
        }

        case AST_IF_STATEMENT: {
            node->as.if_statement.else_branch = NULL;
            node->as.if_statement.else_if_capacity = 4;
            node->as.if_statement.else_if_count = 0;
            node->as.if_statement.else_if_clauses = malloc(node->as.if_statement.else_if_capacity * sizeof(AstNodeElseIfClause));
            if (!node->as.if_statement.else_if_clauses) { parser->had_error = true; free(node); return NULL; }

            while (*current_line_idx < lines->count && get_indent_level(lines->lines[*current_line_idx]) > expected_indent) {
                const char* part_line = lines->lines[*current_line_idx];
                char part_type_str[64];
                get_node_type_from_line(part_line, part_type_str, sizeof(part_type_str));
                (*current_line_idx)++;

                if (strcmp(part_type_str, "IF_CONDITION") == 0) {
                    node->as.if_statement.condition = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                } else if (strcmp(part_type_str, "THEN_BRANCH") == 0) {
                    node->as.if_statement.then_branch = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                } else if (strcmp(part_type_str, "ELSE_BRANCH") == 0) {
                    node->as.if_statement.else_branch = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                } else if (strcmp(part_type_str, "ELSE_IF_CLAUSE") == 0) {
                    if (node->as.if_statement.else_if_count >= node->as.if_statement.else_if_capacity) {
                        size_t new_capacity = node->as.if_statement.else_if_capacity * 2;
                        AstNodeElseIfClause* new_clauses = realloc(node->as.if_statement.else_if_clauses, new_capacity * sizeof(AstNodeElseIfClause));
                        if (!new_clauses) {
                            perror("AST if-else clauses realloc failed");
                            free_ast(node);
                            parser->had_error = true;
                            return NULL;
                        }
                        node->as.if_statement.else_if_clauses = new_clauses;
                        node->as.if_statement.else_if_capacity = new_capacity;
                    }
                    AstNodeElseIfClause* clause = &node->as.if_statement.else_if_clauses[node->as.if_statement.else_if_count++];
                    clause->condition = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                    clause->body = parse_node_recursive(lines, current_line_idx, expected_indent + 2, parser);
                }
                (*current_line_idx)++;
            }
            break;
        }

        case AST_TERNARY_EXPRESSION: {
            node->as.ternary_expression.condition = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.ternary_expression.then_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.ternary_expression.else_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        default: break;
    }
    
    if (parser->had_error) { free_ast(node); return NULL; }
    
    bool is_block_node = false;
    switch(type) {
        case AST_PROGRAM: case AST_ASSIGNMENT: case AST_BINARY_OP:
        case AST_UNARY_OP: case AST_PRINT_STATEMENT:
        case AST_LOGICAL_OP: case AST_FORMATTED_STRING:
        case AST_ARRAY_LITERAL: case AST_SUBSCRIPT:
        case AST_HASHTABLE_LITERAL:
        case AST_EXPRESSION_STATEMENT:
        case AST_BLOCK:
        case AST_RETURN_STATEMENT:
        case AST_FUNCTION_DECLARATION:
        case AST_CALL_EXPRESSION:
        case AST_IF_STATEMENT:
        case AST_TERNARY_EXPRESSION:
            is_block_node = true;
            break;
        default: break;
    }

    if (is_block_node) {
        if (*current_line_idx < lines->count) {
             const char* closing_line = lines->lines[*current_line_idx];
             if (get_indent_level(closing_line) == expected_indent && *(closing_line + expected_indent * 2) == ')') {
                 (*current_line_idx)++;
             } else {
                 fprintf(stderr, "AST Deserializer Error [line %d]: Malformed AST. Expected closing ')' for node started on line %d.\n", *current_line_idx + 1, node->line);
                 parser->had_error = true;
                 free_ast(node);
                 return NULL;
             }
        } else {
            fprintf(stderr, "AST Deserializer Error: Unterminated AST node (reached end of file) for node started on line %d.\n", node->line);
            parser->had_error = true;
            free_ast(node);
            return NULL;
        }
    }
    
    return node;
}

//EXECUTE----------------------------------------------------------------------

static bool are_values_equal(GraveyardValue a, GraveyardValue b) {
    if (a.type != b.type) {
        return false;
    }

    switch (a.type) {
        case VAL_NULL:   return true;
        case VAL_BOOL:   return a.as.boolean == b.as.boolean;
        case VAL_NUMBER: return a.as.number == b.as.number;
        case VAL_STRING:
            return a.as.string->length == b.as.string->length && strcmp(a.as.string->chars, b.as.string->chars) == 0;
        case VAL_ARRAY: return a.as.array == b.as.array;
        default:
            return false;
    }
}

static uint32_t hash_string(const char* key, int length) {
    uint32_t hash = 5381;
    for (int i = 0; i < length; i++) {
        hash = ((hash << 5) + hash) + key[i];
    }
    return hash;
}
static uint32_t hash_graveyard_value(GraveyardValue value);

static GraveyardValue create_hashtable_value() {
    GraveyardValue val;
    val.type = VAL_HASHTABLE;
    GraveyardHashtable* ht = malloc(sizeof(GraveyardHashtable));
    ht->count = 0;
    ht->capacity = 8;
    ht->ref_count = 0;
    ht->entries = malloc(ht->capacity * sizeof(HashtableEntry));
    for (int i = 0; i < ht->capacity; i++) {
        ht->entries[i].is_in_use = false;
    }
    val.as.hashtable = ht;
    return val;
}

static uint32_t hash_graveyard_value(GraveyardValue value) {
    switch (value.type) {
        case VAL_STRING:
            return hash_string(value.as.string->chars, value.as.string->length);
        case VAL_NUMBER:
            return (uint32_t)value.as.number;
        case VAL_BOOL:
            return value.as.boolean ? 1 : 0;
        case VAL_NULL:
            return 2;
        default:
            return 0;
    }
}

static HashtableEntry* hashtable_find_entry(HashtableEntry* entries, int capacity, GraveyardValue key) {
    uint32_t index = hash_graveyard_value(key) % capacity;
    for (;;) {
        HashtableEntry* entry = &entries[index];
        if (!entry->is_in_use || are_values_equal(entry->key, key)) {
            return entry;
        }
        index = (index + 1) % capacity;
    }
}

static void hashtable_resize(GraveyardHashtable* ht, int new_capacity) {
    HashtableEntry* new_entries = malloc(new_capacity * sizeof(HashtableEntry));
    if (!new_entries) {
        perror("hashtable_resize: malloc failed");
        exit(1);
    }
    for (int i = 0; i < new_capacity; i++) {
        new_entries[i].key.type = VAL_NULL;
    }

    for (int i = 0; i < ht->capacity; i++) {
        HashtableEntry* entry = &ht->entries[i];
        if (!entry->is_in_use) continue;

        HashtableEntry* dest = hashtable_find_entry(new_entries, new_capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    free(ht->entries);
    ht->entries = new_entries;
    ht->capacity = new_capacity;
}

static void hashtable_set(GraveyardHashtable* ht, GraveyardValue key, GraveyardValue value) {
    if (ht->count + 1 > ht->capacity * 0.75) {
        int new_capacity = ht->capacity < 8 ? 8 : ht->capacity * 2;
        hashtable_resize(ht, new_capacity);
    }

    HashtableEntry* entry = hashtable_find_entry(ht->entries, ht->capacity, key);
    
    bool is_new_key = !entry->is_in_use;
    if (is_new_key) {
        entry->is_in_use = true;
        ht->count++;
    }
    
    entry->key = key;
    entry->value = value;
}

static MonolithEntry* find_entry(MonolithEntry* entries, int capacity, const char* key) {
    uint32_t index = hash_string(key, strlen(key)) % capacity;
    for (;;) {
        MonolithEntry* entry = &entries[index];
        if (entry->key == NULL || strcmp(entry->key, key) == 0) {
            return entry;
        }
        index = (index + 1) % capacity;
    }
}

static GraveyardValue create_null_value() {
    GraveyardValue val;
    val.type = VAL_NULL;
    val.as.number = 0;
    return val;
}

static GraveyardValue create_function_value(Graveyard* gy, AstNode* node) {
    GraveyardValue val;
    val.type = VAL_FUNCTION;
    GraveyardFunction* func = malloc(sizeof(GraveyardFunction));

    func->ref_count = 0;
    func->arity = node->as.function_declaration.param_count;
    func->body = node->as.function_declaration.body;
    func->params = node->as.function_declaration.params;
    
    func->closure = gy->environment;

    size_t len = strlen(node->as.function_declaration.name.lexeme);
    GraveyardString* name_str = malloc(sizeof(GraveyardString));
    name_str->chars = malloc(len + 1);
    memcpy(name_str->chars, node->as.function_declaration.name.lexeme, len + 1);
    name_str->length = len;
    func->name = name_str;

    val.as.function = func;
    return val;
}

static GraveyardValue execute_node(Graveyard* gy, AstNode* node);

static GraveyardValue execute_block(Graveyard* gy, AstNode* block_node, Environment* environment) {
    Environment* previous = gy->environment;
    gy->environment = environment;
    GraveyardValue last_val = create_null_value();

    for (size_t i = 0; i < block_node->as.block.count; i++) {
        last_val = execute_node(gy, block_node->as.block.statements[i]);
        if (gy->is_returning) {
            break;
        }
    }

    gy->environment = previous;
    return last_val;
}

static void monolith_resize(Monolith* monolith, int new_capacity) {
    MonolithEntry* new_entries = malloc(new_capacity * sizeof(MonolithEntry));
    if (!new_entries) {
        perror("monolith_resize: malloc failed");
        exit(1); 
    }
    for (int i = 0; i < new_capacity; i++) {
        new_entries[i].key = NULL;
    }

    for (int i = 0; i < monolith->capacity; i++) {
        MonolithEntry* entry = &monolith->entries[i];
        if (entry->key == NULL) continue;

        MonolithEntry* dest = find_entry(new_entries, new_capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    free(monolith->entries);
    monolith->entries = new_entries;
    monolith->capacity = new_capacity;
}

void monolith_init(Monolith* monolith) {
    monolith->count = 0;
    monolith->capacity = 8;
    monolith->entries = malloc(monolith->capacity * sizeof(MonolithEntry));
    if (!monolith->entries) {
        perror("monolith_init: malloc failed");
        exit(1);
    }
    for (int i = 0; i < monolith->capacity; i++) {
        monolith->entries[i].key = NULL;
    }
}

void monolith_free(Monolith* monolith) {
    for (int i = 0; i < monolith->capacity; i++) {
        free(monolith->entries[i].key);
    }
    free(monolith->entries);
    monolith_init(monolith);
}

bool monolith_set(Monolith* monolith, const char* key, GraveyardValue value) {
    if (monolith->count + 1 > monolith->capacity * 0.75) {
        int new_capacity = monolith->capacity < 8 ? 8 : monolith->capacity * 2;
        monolith_resize(monolith, new_capacity);
    }

    MonolithEntry* entry = find_entry(monolith->entries, monolith->capacity, key);
    bool is_new_key = entry->key == NULL;
    if (is_new_key) {
        monolith->count++;
        entry->key = strdup(key);
        if (entry->key == NULL) {
             perror("monolith_set: strdup failed");
             return false;
        }
    }
    entry->value = value;
    return true;
}

bool monolith_get(Monolith* monolith, const char* key, GraveyardValue* out_value) {
    if (monolith->count == 0) return false;

    MonolithEntry* entry = find_entry(monolith->entries, monolith->capacity, key);
    if (entry->key == NULL) {
        return false;
    }

    *out_value = entry->value;
    return true;
}

Environment* environment_new(Environment* enclosing) {
    Environment* env = malloc(sizeof(Environment));
    env->enclosing = enclosing;
    monolith_init(&env->values);
    return env;
}

void environment_define(Environment* env, const char* name, GraveyardValue value) {
    monolith_set(&env->values, name, value);
}

bool environment_get(Environment* env, const char* name, GraveyardValue* out_value) {
    if (monolith_get(&env->values, name, out_value)) {
        return true;
    }

    if (env->enclosing != NULL) {
        return environment_get(env->enclosing, name, out_value);
    }

    return false;
}

bool environment_assign(Environment* env, const char* name, GraveyardValue value) {
    MonolithEntry* entry = find_entry(env->values.entries, env->values.capacity, name);
    if (entry->key != NULL) {
        entry->value = value;
        return true;
    }

    if (env->enclosing != NULL) {
        return environment_assign(env->enclosing, name, value);
    }
    
    return false;
}

static GraveyardValue create_bool_value(bool value) {
    GraveyardValue val;
    val.type = VAL_BOOL;
    val.as.boolean = value;
    return val;
}

static GraveyardValue create_number_value(double value) {
    GraveyardValue val;
    val.type = VAL_NUMBER;
    val.as.number = value;
    return val;
}

static GraveyardValue create_string_value(const char* chars) {
    GraveyardValue val;
    val.type = VAL_STRING;

    size_t length = strlen(chars);
    GraveyardString* string_obj = malloc(sizeof(GraveyardString));
    string_obj->chars = malloc(length + 1);
    memcpy(string_obj->chars, chars, length);
    string_obj->chars[length] = '\0';
    string_obj->length = length;
    string_obj->ref_count = 0;

    val.as.string = string_obj;
    return val;
}

static GraveyardValue create_array_value() {
    GraveyardValue val;
    val.type = VAL_ARRAY;

    GraveyardArray* array_obj = malloc(sizeof(GraveyardArray));
    array_obj->capacity = 8;
    array_obj->count = 0;
    array_obj->values = malloc(array_obj->capacity * sizeof(GraveyardValue));
    array_obj->ref_count = 0;

    val.as.array = array_obj;
    return val;
}

static void value_to_string(GraveyardValue value, char* buffer, size_t buffer_size) {
    switch (value.type) {
        case VAL_NULL:
            strncpy(buffer, "null", buffer_size);
            break;
        case VAL_BOOL:
            strncpy(buffer, value.as.boolean ? "true" : "false", buffer_size);
            break;
        case VAL_NUMBER:
            snprintf(buffer, buffer_size, "%g", value.as.number);
            break;
        case VAL_STRING:
            strncpy(buffer, value.as.string->chars, buffer_size);
            break;
        default:
            strncpy(buffer, "", buffer_size);
            break;
    }
}

static bool is_value_falsy(GraveyardValue value) {
    switch (value.type) {
        case VAL_NULL:   return true;
        case VAL_BOOL:   return !value.as.boolean;
        case VAL_NUMBER: return value.as.number == 0;
        case VAL_ARRAY:  return value.as.array->count == 0;
        default:         return false;
    }
}

static void array_append(GraveyardArray* array, GraveyardValue value) {
    if (array->count == array->capacity) {
        array->capacity *= 2;
        array->values = realloc(array->values, array->capacity * sizeof(GraveyardValue));
    }
    array->values[array->count++] = value;
}

void print_value(GraveyardValue value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(value.as.boolean ? "$" : "%%");
            break;
        case VAL_NULL:
            printf("|");
            break;
        case VAL_NUMBER:
            printf("%g", value.as.number);
            break;
        case VAL_STRING:
            printf("%s", value.as.string->chars);
            break;
        case VAL_ARRAY:
            printf("[");
            for (size_t i = 0; i < value.as.array->count; i++) {
                print_value(value.as.array->values[i]);
                if (i < value.as.array->count - 1) {
                    printf(", ");
                }
            }
            printf("]");
            break;
        case VAL_HASHTABLE:
            printf("{");
            int printed = 0;
            for (int i = 0; i < value.as.hashtable->capacity; i++) {
                HashtableEntry* entry = &value.as.hashtable->entries[i];
                if (entry->is_in_use) {
                    if (printed > 0) printf(", ");
                    print_value(entry->key);
                    printf(": ");
                    print_value(entry->value);
                    printed++;
                }
            }
            printf("}");
            break;
    }
}

void monolith_print(Monolith* monolith) {
    printf("--- Monolith Contents ---\n");
    if (monolith->count == 0) {
        printf("  (empty)\n");
        printf("-------------------------\n");
        return;
    }

    for (int i = 0; i < monolith->capacity; i++) {
        MonolithEntry* entry = &monolith->entries[i];
        if (entry->key != NULL) {
            printf("  %s = ", entry->key);
            print_value(entry->value);
            printf("\n");
        }
    }
    printf("-------------------------\n");
}

static GraveyardValue execute_node(Graveyard* gy, AstNode* node) {
    switch (node->type) {
        case AST_PROGRAM: {
            GraveyardValue last_value = create_null_value();
            for (size_t i = 0; i < node->as.program.count; i++) {
                last_value = execute_node(gy, node->as.program.statements[i]);
            }
            return last_value;
        }

        case AST_PRINT_STATEMENT: {
            for (size_t i = 0; i < node->as.print_stmt.count; i++) {
                GraveyardValue value = execute_node(gy, node->as.print_stmt.expressions[i]);

                print_value(value);

                if (i < node->as.print_stmt.count - 1) {
                    printf(" ");
                }
            }
            printf("\n");

            return create_null_value();
        }

        case AST_LITERAL: {
            Token literal_token = node->as.literal.value;
            switch (literal_token.type) {
                case STRING:
                    return create_string_value(literal_token.lexeme);
                case NUMBER:
                    return create_number_value(strtod(literal_token.lexeme, NULL));
                case TRUEVALUE:
                    return create_bool_value(true);
                case FALSEVALUE:
                    return create_bool_value(false);
                case NULLVALUE:
                    return create_null_value();
                default:
                    return create_null_value();
            }
        }

        case AST_ARRAY_LITERAL: {
            GraveyardValue array_val = create_array_value();

            for (size_t i = 0; i < node->as.array_literal.count; i++) {
                GraveyardValue element_value = execute_node(gy, node->as.array_literal.elements[i]);
                array_append(array_val.as.array, element_value);
            }
            
            return array_val;
        }

        case AST_SUBSCRIPT: {
            GraveyardValue array_val = execute_node(gy, node->as.subscript.array);
            if (array_val.type != VAL_ARRAY) {
                fprintf(stderr, "Runtime Error [line %d]: Subscript '[]' can only be applied to arrays.\n", node->line);
                return create_null_value();
            }

            GraveyardValue index_val = execute_node(gy, node->as.subscript.index);
            if (index_val.type != VAL_NUMBER) {
                fprintf(stderr, "Runtime Error [line %d]: Array index must be a number.\n", node->line);
                return create_null_value();
            }
            
            double raw_index = index_val.as.number;
            if (raw_index < 0 || fmod(raw_index, 1.0) != 0) {
                fprintf(stderr, "Runtime Error [line %d]: Array index must be a non-negative integer.\n", node->line);
                return create_null_value();
            }
            
            int index = (int)raw_index;
            GraveyardArray* array = array_val.as.array;

            if (index >= array->count) {
                fprintf(stderr, "Runtime Error [line %d]: Array index out of bounds. Index %d is too large for array of size %zu.\n", node->line, index, array->count);
                return create_null_value();
            }

            return array->values[index];
        }

        case AST_HASHTABLE_LITERAL: {
            GraveyardValue ht_val = create_hashtable_value();
            GraveyardHashtable* ht = ht_val.as.hashtable;

            for (size_t i = 0; i < node->as.hashtable_literal.count; i++) {
                AstNodeKeyValuePair pair = node->as.hashtable_literal.pairs[i];
                GraveyardValue key = execute_node(gy, pair.key);

                if (key.type != VAL_STRING && key.type != VAL_NUMBER &&
                    key.type != VAL_BOOL && key.type != VAL_NULL) {
                    fprintf(stderr, "Runtime Error [line %d]: Invalid type used as a hashtable key.\n", pair.key->line);
                    free(ht->entries);
                    free(ht);
                    return create_null_value();
                }
                
                if (key.type == VAL_NUMBER && fmod(key.as.number, 1.0) != 0) {
                     fprintf(stderr, "Runtime Error [line %d]: A non-integer number cannot be used as a hashtable key.\n", pair.key->line);
                     free(ht->entries);
                     free(ht);
                     return create_null_value();
                }

                GraveyardValue value = execute_node(gy, pair.value);
                hashtable_set(ht, key, value);
            }
            return ht_val;
        }

        case AST_IDENTIFIER: {
            GraveyardValue value;
            if (environment_get(gy->environment, node->as.identifier.name.lexeme, &value)) {
                return value;
            }

            fprintf(stderr, "Runtime Error [line %d]: Undefined variable '%s'.\n",
                    node->line, node->as.identifier.name.lexeme);
            return create_null_value();
        }

        case AST_EXPRESSION_STATEMENT: {
            execute_node(gy, node->as.expression_statement.expression);
            return create_null_value();
        }
        
        case AST_ASSIGNMENT: {
            AstNode* target_node = node->as.assignment.left;
            GraveyardValue value_to_assign = execute_node(gy, node->as.assignment.value);

            if (target_node->type == AST_IDENTIFIER) {
                const char* name = target_node->as.identifier.name.lexeme;
                if (!environment_assign(gy->environment, name, value_to_assign)) {
                    environment_define(gy->environment, name, value_to_assign);
                }
                return value_to_assign;

            } else if (target_node->type == AST_SUBSCRIPT) {
                AstNode* array_node = target_node->as.subscript.array;
                AstNode* index_node = target_node->as.subscript.index;

                GraveyardValue array_val = execute_node(gy, array_node);
                if (array_val.type != VAL_ARRAY) {
                    fprintf(stderr, "Runtime Error [line %d]: Cannot assign to subscript of a non-array type.\n", target_node->line);
                    return create_null_value();
                }

                GraveyardValue index_val = execute_node(gy, index_node);
                if (index_val.type != VAL_NUMBER) {
                    fprintf(stderr, "Runtime Error [line %d]: Array index must be a number.\n", index_node->line);
                    return create_null_value();
                }

                double raw_index = index_val.as.number;
                if (raw_index < 0 || fmod(raw_index, 1.0) != 0) {
                    fprintf(stderr, "Runtime Error [line %d]: Array index must be a non-negative integer.\n", index_node->line);
                    return create_null_value();
                }
                
                int index = (int)raw_index;
                GraveyardArray* array = array_val.as.array;

                if (index >= array->count) {
                    fprintf(stderr, "Runtime Error [line %d]: Array index out of bounds. Cannot assign to index %d in an array of size %zu.\n", target_node->line, index, array->count);
                    return create_null_value();
                }

                array->values[index] = value_to_assign;
                return value_to_assign;
            } else if (target_node->type == AST_BINARY_OP && target_node->as.binary_op.operator.type == REFERENCE) {
                AstNode* ht_node = target_node->as.binary_op.left;
                AstNode* key_node = target_node->as.binary_op.right;

                GraveyardValue ht_val = execute_node(gy, ht_node);
                if (ht_val.type != VAL_HASHTABLE) {
                    fprintf(stderr, "Runtime Error [line %d]: Cannot assign to a key of a non-hashtable type.\n", ht_node->line);
                    return create_null_value();
                }

                GraveyardValue key_val = execute_node(gy, key_node);

                if (key_val.type != VAL_STRING && key_val.type != VAL_NUMBER &&
                    key_val.type != VAL_BOOL && key_val.type != VAL_NULL) {
                    fprintf(stderr, "Runtime Error [line %d]: Invalid type used as a hashtable key.\n", key_node->line);
                    return create_null_value();
                }
                if (key_val.type == VAL_NUMBER && fmod(key_val.as.number, 1.0) != 0) {
                     fprintf(stderr, "Runtime Error [line %d]: A non-integer number cannot be used as a hashtable key.\n", key_node->line);
                     return create_null_value();
                }

                hashtable_set(ht_val.as.hashtable, key_val, value_to_assign);
                return value_to_assign;
            }
            
            return create_null_value();
        }
        
        case AST_LOGICAL_OP: {
            GraveyardValue left = execute_node(gy, node->as.logical_op.left);
            TokenType op_type = node->as.logical_op.operator.type;

            if (op_type == AND) {
                if (is_value_falsy(left)) {
                    return left;
                }
            } else if (op_type == OR) {
                if (!is_value_falsy(left)) {
                    return left;
                }
            }

            return execute_node(gy, node->as.logical_op.right);
        }

        case AST_UNARY_OP: {
            GraveyardValue right = execute_node(gy, node->as.unary_op.right);

            switch (node->as.unary_op.operator.type) {
                case MINUS:
                    if (right.type != VAL_NUMBER) {
                        fprintf(stderr, "Runtime Error [line %d]: Operand for negation must be a number.\n", node->line);
                        return create_null_value();
                    }
                    return create_number_value(-right.as.number);

                case NOT:
                    return create_bool_value(is_value_falsy(right));

                default:
                    fprintf(stderr, "Runtime Error [line %d]: Unknown unary operator.\n", node->line);
                    return create_null_value();
            }
            break;
        }

        case AST_BINARY_OP: {
            if (node->as.binary_op.operator.type == REFERENCE) {
                GraveyardValue ht_val = execute_node(gy, node->as.binary_op.left);
                if (ht_val.type != VAL_HASHTABLE) {
                    fprintf(stderr, "Runtime Error [line %d]: Cannot apply '#' lookup to non-hashtable type.\n", node->line);
                    return create_null_value();
                }

                GraveyardValue key_val = execute_node(gy, node->as.binary_op.right);
                GraveyardHashtable* ht = ht_val.as.hashtable;
                HashtableEntry* entry = hashtable_find_entry(ht->entries, ht->capacity, key_val);

                if (entry->is_in_use) {
                    return entry->value;
                } else {
                    return create_null_value();
                }
            }

            GraveyardValue left = execute_node(gy, node->as.binary_op.left);
            GraveyardValue right = execute_node(gy, node->as.binary_op.right);
            TokenType op_type = node->as.binary_op.operator.type;

            if (op_type == EQUALITY)   return create_bool_value(are_values_equal(left, right));
            if (op_type == INEQUALITY) return create_bool_value(!are_values_equal(left, right));
            if (op_type == XOR) {
                bool left_is_truthy = !is_value_falsy(left);
                bool right_is_truthy = !is_value_falsy(right);
                return create_bool_value(left_is_truthy != right_is_truthy);
            }

            if (op_type == PLUS) {
                if (left.type == VAL_ARRAY) {
                    GraveyardValue new_array_val = create_array_value();
                    
                    GraveyardArray* old_array = left.as.array;
                    for (size_t i = 0; i < old_array->count; i++) {
                        array_append(new_array_val.as.array, old_array->values[i]);
                    }

                    array_append(new_array_val.as.array, right);

                    return new_array_val;
                }

                if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                    return create_number_value(left.as.number + right.as.number);
                }

                if (left.type == VAL_STRING || right.type == VAL_STRING) {
                    char left_str[256];
                    char right_str[256];
                    value_to_string(left, left_str, sizeof(left_str));
                    value_to_string(right, right_str, sizeof(right_str));

                    size_t total_len = strlen(left_str) + strlen(right_str);
                    if (total_len >= MAX_LEXEME_LEN) {
                        fprintf(stderr, "Runtime Error [line %d]: Resulting string from concatenation is too long.\n", node->line);
                        return create_null_value();
                    }
                    char result_buffer[MAX_LEXEME_LEN];
                    strcpy(result_buffer, left_str);
                    strcat(result_buffer, right_str);

                    return create_string_value(result_buffer);
                }

                goto type_error;
            }

            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) goto type_error;

            switch (op_type) {
                case MINUS:          return create_number_value(left.as.number - right.as.number);
                case MULTIPLICATION: return create_number_value(left.as.number * right.as.number);
                case EXPONENTIATION: return create_number_value(pow(left.as.number, right.as.number));
                case MODULO:
                    if (right.as.number == 0) {
                        fprintf(stderr, "Runtime Error [line %d]: Division by zero in modulo operation.\n", node->line);
                        return create_null_value();
                    }
                    return create_number_value(fmod(left.as.number, right.as.number));
                case DIVISION:
                    if (right.as.number == 0) {
                        fprintf(stderr, "Runtime Error [line %d]: Division by zero.\n", node->line);
                        return create_null_value();
                    }
                    return create_number_value(left.as.number / right.as.number);
                
                default:
                    return create_bool_value(
                        op_type == GREATERTHAN    ? left.as.number > right.as.number :
                        op_type == LESSTHAN       ? left.as.number < right.as.number :
                        op_type == GREATERTHANEQUAL ? left.as.number >= right.as.number :
                                                     left.as.number <= right.as.number
                    );
            }
        type_error:
            fprintf(stderr, "Runtime Error [line %d]: Operands have incompatible types for this operation.\n", node->line);
            return create_null_value();
        }

        case AST_FORMATTED_STRING: {
            size_t capacity = 128;
            size_t length = 0;
            char* result_string = malloc(capacity);
            if (!result_string) {
                perror("Formatted string buffer malloc failed");
                return create_null_value();
            }
            result_string[0] = '\0';

            for (size_t i = 0; i < node->as.formatted_string.count; i++) {
                FmtStringPart part = node->as.formatted_string.parts[i];
                char part_buffer[256]; 

                if (part.type == FMT_PART_LITERAL) {
                    strncpy(part_buffer, part.as.literal.lexeme, sizeof(part_buffer) - 1);
                    part_buffer[sizeof(part_buffer) - 1] = '\0';
                } else {
                    GraveyardValue value = execute_node(gy, part.as.expression);
                    value_to_string(value, part_buffer, sizeof(part_buffer));
                }
                
                size_t part_len = strlen(part_buffer);
                if (length + part_len + 1 > capacity) {
                    capacity = (length + part_len) * 2;
                    char* new_result = realloc(result_string, capacity);
                    if (!new_result) {
                        perror("Formatted string buffer realloc failed");
                        free(result_string);
                        return create_null_value();
                    }
                    result_string = new_result;
                }
                strcat(result_string, part_buffer);
                length += part_len;
            }

            GraveyardValue final_value = create_string_value(result_string);
            free(result_string);
            return final_value;
        }

        case AST_FUNCTION_DECLARATION: {
            GraveyardValue function = create_function_value(gy, node);
            environment_define(gy->environment, node->as.function_declaration.name.lexeme, function);
            return create_null_value();
        }

        case AST_BLOCK: {
            Environment* new_env = environment_new(gy->environment);
            return execute_block(gy, node, new_env);
        }

        case AST_RETURN_STATEMENT: {
            GraveyardValue value = create_null_value();
            if (node->as.return_statement.value) {
                value = execute_node(gy, node->as.return_statement.value);
            }
            gy->is_returning = true;
            gy->return_value = value;
            return value;
        }

        case AST_CALL_EXPRESSION: {
            GraveyardValue callee = execute_node(gy, node->as.call_expression.callee);

            if (callee.type != VAL_FUNCTION) {
                fprintf(stderr, "Runtime Error [line %d]: Can only call functions.\n", node->line);
                return create_null_value();
            }

            GraveyardFunction* function = callee.as.function;
            int arg_count = node->as.call_expression.arg_count;

            if (arg_count != function->arity) {
                fprintf(stderr, "Runtime Error [line %d]: Expected %d arguments but got %d.\n",
                    node->line, function->arity, arg_count);
                return create_null_value();
            }
            
            Environment* new_env = environment_new(function->closure);
            
            for (int i = 0; i < function->arity; i++) {
                GraveyardValue arg_value = execute_node(gy, node->as.call_expression.arguments[i]);
                environment_define(new_env, function->params[i].lexeme, arg_value);
            }
            
            execute_block(gy, function->body, new_env);

            GraveyardValue result = create_null_value();
            if (gy->is_returning) {
                result = gy->return_value;
                gy->is_returning = false; 
            }

            return result;
        }

        case AST_IF_STATEMENT: {
            GraveyardValue condition_val = execute_node(gy, node->as.if_statement.condition);
            if (!is_value_falsy(condition_val)) {
                execute_block(gy, node->as.if_statement.then_branch, environment_new(gy->environment));
                return create_null_value();
            }

            for (size_t i = 0; i < node->as.if_statement.else_if_count; i++) {
                AstNodeElseIfClause* clause = &node->as.if_statement.else_if_clauses[i];
                GraveyardValue else_if_condition = execute_node(gy, clause->condition);
                if (!is_value_falsy(else_if_condition)) {
                    execute_block(gy, clause->body, environment_new(gy->environment));
                    return create_null_value();
                }
            }

            if (node->as.if_statement.else_branch != NULL) {
                execute_block(gy, node->as.if_statement.else_branch, environment_new(gy->environment));
            }

            return create_null_value();
        }

        case AST_TERNARY_EXPRESSION: {
            GraveyardValue condition = execute_node(gy, node->as.ternary_expression.condition);
            if (!is_value_falsy(condition)) {
                return execute_node(gy, node->as.ternary_expression.then_expr);
            } else {
                return execute_node(gy, node->as.ternary_expression.else_expr);
            }
        }
    }

    return create_null_value();
}

bool execute(Graveyard *gy) {
    if (!gy || !gy->ast_root) {
        fprintf(stderr, "Execution error: Nothing to execute (no AST).\n");
        return false;
    }
    
    gy->last_executed_value = execute_node(gy, gy->ast_root);
    
    return true; 
}

//MAIN----------------------------------------------------------------------------

void graveyard_free(Graveyard *gy) {
    if (!gy) return;
    free(gy->source_code);
    free(gy->tokens);
    free_ast(gy->ast_root);
    
    Environment* env = gy->environment;
    while (env != NULL) {
        Environment* next = env->enclosing;
        monolith_free(&env->values);
        free(env);
        env = next;
    }

    free(gy);
}

Graveyard *graveyard_init(const char *mode, const char *filename) {
    Graveyard *gy = malloc(sizeof(Graveyard));
    if (!gy) {
        perror("graveyard_init: malloc failed");
        return NULL;
    }
    gy->mode = mode;
    gy->filename = filename;
    gy->source_code = NULL;
    gy->tokens = NULL;
    gy->token_count = 0;
    gy->ast_root = NULL;
    gy->last_executed_value = create_null_value();
    gy->environment = malloc(sizeof(Environment));
    gy->environment->enclosing = NULL;
    monolith_init(&gy->environment->values);
    gy->is_returning = false;
    gy->return_value = create_null_value();
    return gy;
}

static bool compile_source(Graveyard* gy) {
    if (!tokenize(gy)) {
        fprintf(stderr, "Compilation failed during tokenization.\n");
        return false;
    }
    if (!parse(gy)) {
        fprintf(stderr, "Compilation failed during parsing.\n");
        return false;
    }

    printf("Parsing successful. AST created.\n");
    
    char out_filename[512];
    strncpy(out_filename, gy->filename, sizeof(out_filename) - 5);
    out_filename[sizeof(out_filename) - 5] = '\0';
    
    char* dot = strrchr(out_filename, '.');
    if (dot != NULL) {
        strcpy(dot, ".gyc");
    } else {
        strncat(out_filename, ".gyc", sizeof(out_filename) - strlen(out_filename) - 1);
    }

    if (save_ast_to_file(gy, out_filename)) {
        printf("Successfully wrote AST to %s\n", out_filename);
    } else {
        fprintf(stderr, "Failed to write AST file.\n");
        return false;
    }
    
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: graveyard <mode> <source file>\n");
        fprintf(stderr, "Modes:\n");
        fprintf(stderr, "  --tokenize, -t          Tokenize source and print tokens\n");
        fprintf(stderr, "  --parse, -p             Parse source and save the AST to a .gyc file\n");
        fprintf(stderr, "  --execute, -e           Parse, save AST, and execute the source code\n");
        fprintf(stderr, "  --debug, -d             Parse, save AST, execute, and print monolith contents\n");
        fprintf(stderr, "  --executecompiled, -ec  Execute a pre-parsed .gyc file\n");
        return 1;
    }

    Graveyard *gy = graveyard_init(argv[1], argv[2]);
    if (!gy) { return 1; }

    bool success = true;

    if (strcmp(gy->mode, "--executecompiled") == 0 || strcmp(gy->mode, "-ec") == 0) {
        const char *ext = strrchr(gy->filename, '.');
        if (!ext || strcmp(ext, ".gyc") != 0) {
            fprintf(stderr, "Error: Execute compiled mode requires a .gyc file.\n");
            success = false;
        } else {
            printf("--- Loading and Executing Compiled AST from %s ---\n", gy->filename);
            if (load_ast_from_file(gy, gy->filename)) {
                print_ast(gy->ast_root);
                if (!execute(gy)) {
                    fprintf(stderr, "Execution failed.\n");
                    success = false;
                }
            } else {
                fprintf(stderr, "Failed to load AST from file.\n");
                success = false;
            }
        }
    } else {
        const char *ext = strrchr(gy->filename, '.');
        if (!ext || strcmp(ext, ".gy") != 0) {
            fprintf(stderr, "Error: Mode '%s' requires a .gy source file.\n", gy->mode);
            success = false;
        } else {
            FILE *file = fopen(gy->filename, "r");
            if (!file) {
                perror("Error opening source file");
                graveyard_free(gy);
                return 1;
            }
            gy->source_code = load(file, NULL);
            fclose(file);
            if (!gy->source_code) {
                fprintf(stderr, "Failed to load source file.\n");
                success = false;
            } else {
                if (strcmp(gy->mode, "--tokenize") == 0 || strcmp(gy->mode, "-t") == 0) {
                    printf("--- Tokenizer Debug Mode ---\n");
                    if (!tokenize(gy)) {
                        fprintf(stderr, "Tokenization failed.\n");
                        success = false;
                    } else {
                        printf("Tokenization successful. Found %zu tokens.\n", gy->token_count);
                        for (size_t i = 0; i < gy->token_count; i++) {
                            printf("Token %zu [L%d, C%d]: Type=%d, Lexeme='%s'\n",
                                   i,
                                   gy->tokens[i].line,
                                   gy->tokens[i].column,
                                   gy->tokens[i].type,
                                   gy->tokens[i].lexeme);
                        }
                    }
                } else if (strcmp(gy->mode, "--parse") == 0 || strcmp(gy->mode, "-p") == 0) {
                    if (!compile_source(gy)) { success = false; }
                } else if (strcmp(gy->mode, "--execute") == 0 || strcmp(gy->mode, "-e") == 0) {
                    if (!compile_source(gy) || !execute(gy)) { success = false; }
                } else if (strcmp(gy->mode, "--debug") == 0 || strcmp(gy->mode, "-d") == 0) {
                    if (compile_source(gy) && execute(gy)) {
                        monolith_print(&gy->environment->values);
                    } else { success = false; }
                } else {
                    fprintf(stderr, "Unknown mode for .gy file: %s\n", gy->mode);
                    success = false;
                }
            }
        }
    }
    
    graveyard_free(gy);
    
    return success ? 0 : 1;
}
