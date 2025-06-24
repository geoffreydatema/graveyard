#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

#define MAX_LEXEME_LEN 65

// --- Type Definitions ---

typedef enum {
    IDENTIFIER, TYPE, NUMBER, SEMICOLON, ASSIGNMENT, ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, EXPONENTIATION, LEFTPARENTHESES, RIGHTPARENTHESES, EQUALITY, INEQUALITY, GREATERTHAN, LESSTHAN, GREATERTHANEQUAL, LESSTHANEQUAL, NOT, AND, OR, XOR, COMMA, TRUEVALUE, FALSEVALUE, NULLVALUE, STRING, LEFTBRACE, RIGHTBRACE, PARAMETER, RETURN, QUESTIONMARK, COLON, WHILE, CONTINUE, BREAK, AT, FORMATTEDSTRING, LEFTBRACKET, RIGHTBRACKET, ADDITIONASSIGNMENT, SUBTRACTIONASSIGNMENT, MULTIPLICATIONASSIGNMENT, DIVISIONASSIGNMENT, EXPONENTIATIONASSIGNMENT, INCREMENT, DECREMENT, REFERENCE, PERIOD, NAMESPACE, PRINT, SCAN, RAISE, CASTBOOLEAN, CASTINTEGER, CASTFLOAT, CASTSTRING, CASTARRAY, CASTHASHTABLE, TYPEOF, MODULO, FILEREAD, FILEWRITE, TIME, EXECUTE, CATCONSTANT, NULLCOALESCE, LENGTH, PLACEHOLDER, TOKEN_EOF, UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
    int line;
    int column;
} Token;

// Forward-declare the main struct so its pointers can be used inside our definitions
typedef struct AstNode AstNode;

typedef enum {
    AST_PROGRAM,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_IDENTIFIER,
    AST_LITERAL_NUMBER
} AstNodeType;

typedef struct {
    AstNode** statements; // Dynamic array of pointers to statement nodes
    size_t count;
    size_t capacity;
} AstNodeProgram;

typedef struct {
    Token operator;
    AstNode *left;
    AstNode *right;
} AstNodeBinaryOp;

// Node for: my_variable = <expression>
typedef struct {
    Token identifier; // The token for the variable name being assigned to
    AstNode *value;   // The expression node for the value on the right-hand side
} AstNodeAssignment;

// Node for referencing an identifier (e.g., a variable or function name)
typedef struct {
    Token name; // The IDENTIFIER token itself
} AstNodeIdentifier;

// Node for a literal value, like a number or string
typedef struct {
    Token value;
} AstNodeLiteral;

// The main generic AST node struct, using a tagged union.
// This allows a single struct type to represent many different kinds of nodes
// in a memory-efficient way.
struct AstNode {
    AstNodeType type; // The tag that tells us which part of the union is active
    int line;         // The line number for excellent error reporting!

    union {
        AstNodeProgram      program;
        AstNodeBinaryOp     binary_op;
        AstNodeAssignment   assignment;
        AstNodeIdentifier   identifier;
        AstNodeLiteral      literal;
    } as;
};

// Defines the type of data a GraveyardValue can hold.
typedef enum {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER
    // Future types will be added here: VAL_STRING, VAL_TYPE, etc.
} ValueType;

// A tagged union struct that can represent any value in the Graveyard language.
typedef struct {
    ValueType type;
    union {
        bool   boolean;
        double number;
    } as;
} GraveyardValue;

// An entry in the hash table's bucket array.
typedef struct {
    char* key;
    GraveyardValue value;
} MonolithEntry;

// The hash table structure itself.
typedef struct {
    int count;
    int capacity;
    MonolithEntry* entries;
} Monolith;

typedef struct {
    const char *mode;
    const char *filename;
    char *source_code;
    Token *tokens;
    size_t token_count;
    AstNode *ast_root;
    Monolith globals;
    GraveyardValue last_executed_value;
} Graveyard;

// Creates a GraveyardValue of type bool.
static GraveyardValue create_bool_value(bool value) {
    GraveyardValue val;
    val.type = VAL_BOOL;
    val.as.boolean = value;
    return val;
}

// Creates a GraveyardValue of type null.
static GraveyardValue create_null_value() {
    GraveyardValue val;
    val.type = VAL_NULL;
    val.as.number = 0; // The union must be initialized, but the value doesn't matter for null.
    return val;
}

// Creates a GraveyardValue of type number.
static GraveyardValue create_number_value(double value) {
    GraveyardValue val;
    val.type = VAL_NUMBER;
    val.as.number = value;
    return val;
}

// Prints a GraveyardValue to the console (for debugging).
void print_value(GraveyardValue value) {
    switch (value.type) {
        case VAL_BOOL:
            // Use the language's native symbols for true/false
            printf(value.as.boolean ? "$" : "%%");
            break;
        case VAL_NULL:
            printf("|");
            break;
        case VAL_NUMBER:
            printf("%g", value.as.number);
            break;
    }
}

// A proven, simple string hashing function (djb2).
static uint32_t hash_string(const char* key, int length) {
    uint32_t hash = 5381;
    for (int i = 0; i < length; i++) {
        hash = ((hash << 5) + hash) + key[i]; // hash * 33 + c
    }
    return hash;
}

// Internal helper to find a bucket for a key. This is the core of the hash table.
// It may return a bucket with the key, an empty bucket, or a tombstone.
static MonolithEntry* find_entry(MonolithEntry* entries, int capacity, const char* key) {
    uint32_t index = hash_string(key, strlen(key)) % capacity;
    for (;;) {
        MonolithEntry* entry = &entries[index];
        // If the slot is empty or the key matches, we've found our spot.
        if (entry->key == NULL || strcmp(entry->key, key) == 0) {
            return entry;
        }
        // Collision, try the next slot.
        index = (index + 1) % capacity;
    }
}

// Resizes the hash table when it gets too full.
static void monolith_resize(Monolith* monolith, int new_capacity) {
    MonolithEntry* new_entries = malloc(new_capacity * sizeof(MonolithEntry));
    if (!new_entries) {
        perror("monolith_resize: malloc failed");
        // In a real application, you might want to handle this more gracefully.
        exit(1); 
    }
    for (int i = 0; i < new_capacity; i++) {
        new_entries[i].key = NULL;
    }

    // Re-insert all old entries into the new, larger bucket array.
    for (int i = 0; i < monolith->capacity; i++) {
        MonolithEntry* entry = &monolith->entries[i];
        if (entry->key == NULL) continue;

        MonolithEntry* dest = find_entry(new_entries, new_capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    free(monolith->entries); // Free the old array.
    monolith->entries = new_entries;
    monolith->capacity = new_capacity;
}

// --- Public Monolith API ---

void monolith_init(Monolith* monolith) {
    monolith->count = 0;
    monolith->capacity = 8; // Initial size
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
        free(monolith->entries[i].key); // Free the copied keys
    }
    free(monolith->entries);
    monolith_init(monolith); // Reset to a clean state
}

bool monolith_set(Monolith* monolith, const char* key, GraveyardValue value) {
    // Resize if the table is getting too full (>75% load factor)
    if (monolith->count + 1 > monolith->capacity * 0.75) {
        int new_capacity = monolith->capacity < 8 ? 8 : monolith->capacity * 2;
        monolith_resize(monolith, new_capacity);
    }

    MonolithEntry* entry = find_entry(monolith->entries, monolith->capacity, key);
    bool is_new_key = entry->key == NULL;
    if (is_new_key) {
        monolith->count++;
        entry->key = strdup(key); // Take ownership of the key by copying it
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
        return false; // Key not found
    }

    *out_value = entry->value;
    return true;
}

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
    if (c1 == '+' && c2 == '=') return ADDITIONASSIGNMENT;
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
        case '+': return ADDITION;
        case '-': return SUBTRACTION;
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

// --- Initialization and Cleanup ---

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
    monolith_init(&gy->globals);
    gy->last_executed_value = create_null_value();
    return gy;
}

void free_ast(AstNode* node) {
    if (node == NULL) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (size_t i = 0; i < node->as.program.count; i++) {
                free_ast(node->as.program.statements[i]);
            }
            free(node->as.program.statements);
            break;
        case AST_BINARY_OP:
            free_ast(node->as.binary_op.left);
            free_ast(node->as.binary_op.right);
            break;
        case AST_ASSIGNMENT:
            free_ast(node->as.assignment.value);
            break;
        case AST_IDENTIFIER:
        case AST_LITERAL_NUMBER:
            break;
    }
    free(node);
}

void graveyard_free(Graveyard *gy) {
    if (!gy) return;
    free(gy->source_code);
    free(gy->tokens);
    free_ast(gy->ast_root);
    monolith_free(&gy->globals);
    free(gy);
}

// --- Parser Implementation ---

typedef struct {
    Graveyard* gy;         // A link back to the main state if needed
    Token* tokens;     // The array of tokens we are parsing
    size_t     token_count;
    size_t     current;    // Index of the next token to be processed
    bool       had_error;
} Parser;

// Returns the current token without consuming it.
static Token* peek(Parser* parser) {
    return &parser->tokens[parser->current];
}

// Checks if we have run out of tokens to parse.
static bool is_at_end(Parser* parser) {
    return peek(parser)->type == TOKEN_EOF;
}

// Consumes the current token and returns it, advancing the cursor.
static Token* consume(Parser* parser) {
    if (!is_at_end(parser)) {
        parser->current++;
    }
    // Return the token that was just consumed.
    return &parser->tokens[parser->current - 1];
}

// Reports an error at a given token's location.
static void error_at_token(Parser* parser, Token* token, const char* message) {
    if (parser->had_error) return; // Only report the first error encountered.
    parser->had_error = true;

    fprintf(stderr, "Parser Error [line %d, col %d]: ", token->line, token->column);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, "at end of file. ");
    } else {
        fprintf(stderr, "at '%s'. ", token->lexeme);
    }
    fprintf(stderr, "%s\n", message);
}

// Checks if the current token is of the expected type. If not, reports an error.
// This is for mandatory tokens, like a semicolon at the end of a statement.
static Token* expect(Parser* parser, TokenType type, const char* message) {
    if (peek(parser)->type == type) {
        return consume(parser);
    }
    error_at_token(parser, peek(parser), message);
    return NULL; // Return NULL on error
}

// Checks if the current token is of a given type. If so, consumes it and returns true.
// This is for optional tokens, like an 'else' clause after an 'if'.
static bool match(Parser* parser, TokenType type) {
    if (is_at_end(parser)) return false;
    if (peek(parser)->type == type) {
        consume(parser);
        return true;
    }
    return false;
}

// Helper to create a new AST node and allocate memory for it.
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

// Add this helper function with the other parser helpers
static int get_operator_precedence(TokenType type) {
    switch (type) {
        case EQUALITY:
        case INEQUALITY:
            return 1;
        case LESSTHAN:
        case GREATERTHAN:
        case LESSTHANEQUAL:
        case GREATERTHANEQUAL:
            return 2;
        case ADDITION:
        case SUBTRACTION:
            return 3;
        case MULTIPLICATION:
        case DIVISION:
            return 4;
        case EXPONENTIATION:
            return 5;
        default:
            return 0; // Not a binary operator
    }
}

// Forward-declare parsing functions that call each other.
static AstNode* parse_statement(Parser* parser);
static AstNode* parse_expression(Parser* parser, int min_precedence);

// Parses the most basic units of an expression.
// For now, it only handles numbers and identifiers.
static AstNode* parse_primary(Parser* parser) {
    if (match(parser, LEFTPARENTHESES)) {
        // A parenthesized expression starts a new precedence context.
        AstNode* expr = parse_expression(parser, 1);
        expect(parser, RIGHTPARENTHESES, "Expected ')' after expression.");
        return expr;
    }

    if (match(parser, NUMBER)) {
        Token literal_token = parser->tokens[parser->current - 1];
        AstNode* node = create_node(parser, AST_LITERAL_NUMBER);
        node->line = literal_token.line;
        node->as.literal.value = literal_token;
        return node;
    }
    
    if (match(parser, IDENTIFIER)) {
        Token identifier_token = parser->tokens[parser->current - 1];
        AstNode* node = create_node(parser, AST_IDENTIFIER);
        node->line = identifier_token.line;
        node->as.identifier.name = identifier_token;
        return node;
    }

    // If we get here, it's not a valid start to an expression.
    error_at_token(parser, peek(parser), "Expected expression.");
    return NULL;
}

// This is the new, precedence-aware expression parser.
static AstNode* parse_expression(Parser* parser, int min_precedence) {
    // First, parse a "primary" expression, which is the start of our sequence.
    // This could be a number, an identifier, or a parenthesized expression.
    AstNode* left = parse_primary(parser);
    if (!left) return NULL;

    // The Precedence Climbing loop
    while (true) {
        Token operator_token = *peek(parser);
        int current_precedence = get_operator_precedence(operator_token.type);

        // If the next token is not an operator, or its precedence is too low,
        // we're done with this sub-expression.
        if (current_precedence == 0 || current_precedence < min_precedence) {
            break;
        }

        // We found an operator with good precedence, so consume it.
        consume(parser);

        // Recursively call parse_expression for the right-hand side.
        // This is the magic: we pass a higher precedence level to ensure that
        // operators to the right are handled correctly.
        AstNode* right = parse_expression(parser, current_precedence + 1);
        if (!right) { free_ast(left); return NULL; }

        // Combine the left, operator, and right into a new binary operation node.
        AstNode* node = create_node(parser, AST_BINARY_OP);
        node->line = operator_token.line;
        node->as.binary_op.operator = operator_token;
        node->as.binary_op.left = left;
        node->as.binary_op.right = right;
        
        // This new node now becomes the 'left' side for the next loop iteration.
        // This is how we handle chains like 1 + 2 + 3.
        left = node;
    }

    return left;
}

// Parses an assignment statement like 'x = 1;'
static AstNode* parse_assignment(AstNode* identifier_node, Parser* parser) {
    int line = identifier_node->line;
    // The '=' token has already been matched by the caller.

    // Call parse_expression with the lowest precedence to parse the whole right-hand side.
    AstNode* value = parse_expression(parser, 1);
    if (!value) return NULL; // Propagate error

    AstNode* node = create_node(parser, AST_ASSIGNMENT);
    node->line = line;
    // In an assignment, the left side isn't a value, it's a target.
    // For now, we reuse the identifier node, but later this might change.
    node->as.assignment.identifier = identifier_node->as.identifier.name;
    node->as.assignment.value = value;
    
    // We consumed the identifier node to create this new assignment node.
    free(identifier_node);
    
    return node;
}

// This function acts as a dispatcher to the correct statement parser.
static AstNode* parse_statement(Parser* parser) {
    // Peek ahead to see if this is an assignment statement.
    if (peek(parser)->type == IDENTIFIER &&
        parser->tokens[parser->current + 1].type == ASSIGNMENT) {
        
        AstNode* identifier_node = parse_primary(parser);
        match(parser, ASSIGNMENT); // Consume the '='
        return parse_assignment(identifier_node, parser);
    }

    // For now, we don't have other statement types. Add them here later.
    // e.g., if (match(parser, WHILE)) { return parse_while_statement(parser); }
    
    error_at_token(parser, peek(parser), "Expected a statement.");
    return NULL;
}

// This is the top-level function that orchestrates the parsing.
bool parse(Graveyard *gy) {
    Parser parser;
    parser.gy = gy;
    parser.tokens = gy->tokens;
    parser.token_count = gy->token_count;
    parser.current = 0;
    parser.had_error = false;

    AstNode* root = create_node(&parser, AST_PROGRAM);
    if (!root) return false; // Malloc failed
    root->line = 0; // The program node doesn't correspond to a single line

    // Initialize the dynamic array of statements
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

    // --- NEW: Loop until we run out of tokens ---
    while (!is_at_end(&parser)) {
        AstNode* statement = parse_statement(&parser);
        if (parser.had_error) {
            // If an error occurred, stop and clean up.
            // Note: parse_statement is responsible for freeing its own partial nodes.
            // We just need to free the program list.
            goto cleanup;
        }

        // Expect a semicolon after every statement
        expect(&parser, SEMICOLON, "Expected ';' at the end of the statement.");
        if (parser.had_error) goto cleanup;

        // Add the successfully parsed statement to our program's list
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
        // free_ast is now responsible for freeing the program node and its children
        free_ast(gy->ast_root); 
        gy->ast_root = NULL;
    }
    
    return !parser.had_error;
}

// --- AST Serialization ---

// The recursive helper function that traverses the tree and prints it.
static void write_ast_node(FILE* file, AstNode* node, int indent) {
    if (node == NULL) {
        return;
    }

    // Print indentation to show the tree structure
    for (int i = 0; i < indent; ++i) {
        fprintf(file, "  ");
    }

    // Print the node's information based on its type
    switch (node->type) {
        case AST_PROGRAM:
            // NEW: Print the program node and then iterate through each statement.
            fprintf(file, "(PROGRAM\n");
            for (size_t i = 0; i < node->as.program.count; i++) {
                write_ast_node(file, node->as.program.statements[i], indent + 1);
            }
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
        case AST_ASSIGNMENT:
            fprintf(file, "(ASSIGN identifier=\"%s\" line=%d\n", node->as.assignment.identifier.lexeme, node->line);
            write_ast_node(file, node->as.assignment.value, indent + 1);
            
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); } // Indent the closing parenthesis
            fprintf(file, ")\n");
            break;
        case AST_IDENTIFIER:
            fprintf(file, "(IDENTIFIER name=\"%s\" line=%d)\n", node->as.identifier.name.lexeme, node->line);
            break;
        case AST_LITERAL_NUMBER:
            fprintf(file, "(LITERAL_NUM value=\"%s\" line=%d)\n", node->as.literal.value.lexeme, node->line);
            break;
        default:
             fprintf(file, "(UNKNOWN_NODE type=%d line=%d)\n", node->type, node->line);
             break;
    }
}

// The main public function to save the entire AST to a file.
bool save_ast_to_file(Graveyard* gy, const char* out_filename) {
    FILE* file = fopen(out_filename, "w");
    if (!file) {
        perror("save_ast_to_file: Could not open file for writing");
        return false;
    }

    printf("Writing AST to %s...\n", out_filename);
    
    // Start the recursive writing process from the root node
    write_ast_node(file, gy->ast_root, 0);

    fclose(file);
    return true;
}

// --- Core Logic Functions ---

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
        if (count == capacity) {
            size_t new_capacity = capacity * 2;
            Token *new_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!new_tokens) { perror("tokenize: realloc failed"); goto cleanup_failure; }
            tokens = new_tokens;
            capacity = new_capacity;
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

        if (state == STATE_IN_FMT_STRING) {
            if (c == '\'') { state = STATE_DEFAULT; current_ptr++; continue; }
            if (c == '{') {
                state = STATE_DEFAULT; fstring_brace_depth = 1;
                tokens[count].type = LEFTBRACE; tokens[count].lexeme[0] = '{'; tokens[count].lexeme[1] = '\0';
                tokens[count].line = line; tokens[count].column = column;
                count++; current_ptr++; continue;
            }
            if (c == '\n' || c == '\0') {
                fprintf(stderr, "Tokenizer error [line %zu, col %d]: Unterminated formatted string.\n", line, column);
                goto cleanup_failure;
            }
            const char* start = current_ptr;
            while (current_ptr < end_ptr && *current_ptr != '\'' && *current_ptr != '{' && *current_ptr != '\n') { current_ptr++; }
            size_t len = current_ptr - start;
            if (len > 0) {
                 if (len >= MAX_LEXEME_LEN) {
                    fprintf(stderr, "Tokenizer error [line %d, col %d]: Literal part of formatted string is too long.\n", line, column);
                    goto cleanup_failure;
                }
                tokens[count].type = FORMATTEDSTRING;
                strncpy(tokens[count].lexeme, start, len); tokens[count].lexeme[len] = '\0';
                tokens[count].line = line; tokens[count].column = column;
                count++;
            }
            continue;
        }
        
        if (c == '\'') { state = STATE_IN_FMT_STRING; current_ptr++; continue; }
        
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

    tokens[count].type = TOKEN_EOF;
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

// --- Execution Engine ---

// Forward-declare the main execution function.
static GraveyardValue execute_node(Graveyard* gy, AstNode* node);

// The main public entry point to start execution.
bool execute(Graveyard *gy) {
    if (!gy || !gy->ast_root) {
        fprintf(stderr, "Execution error: Nothing to execute (no AST).\n");
        return false;
    }
    
    // Silently execute the AST and store the final value in our struct.
    gy->last_executed_value = execute_node(gy, gy->ast_root);
    
    // In the future, we can check for a runtime error flag here.
    return true; 
}

// The recursive AST walker that evaluates each node.
// Every expression-like node will return the GraveyardValue it evaluates to.
static GraveyardValue execute_node(Graveyard* gy, AstNode* node) {
    switch (node->type) {
        case AST_PROGRAM: {
            // A program is a list of statements. Execute each one in order.
            GraveyardValue last_value = create_null_value();
            for (size_t i = 0; i < node->as.program.count; i++) {
                last_value = execute_node(gy, node->as.program.statements[i]);
            }
            return last_value; // In an interactive REPL, this would be the value of the last statement.
        }

        case AST_LITERAL_NUMBER: {
            // Convert the string lexeme to a C double.
            // strtod is from <stdlib.h> and is safer than atof.
            double value = strtod(node->as.literal.value.lexeme, NULL);
            return create_number_value(value);
        }

        case AST_IDENTIFIER: {
            GraveyardValue value;
            // Look up the variable name in our global hash table.
            if (monolith_get(&gy->globals, node->as.identifier.name.lexeme, &value)) {
                return value; // Return the found value.
            }

            // If the variable isn't found, it's a runtime error.
            fprintf(stderr, "Runtime Error [line %d]: Undefined variable '%s'.\n",
                    node->line, node->as.identifier.name.lexeme);
            // In a more advanced interpreter, we would set an error flag.
            // For now, we return NULL, which will likely cause a crash down the line
            // if not handled, clearly indicating a bug.
            return create_null_value();
        }

        case AST_ASSIGNMENT: {
            // First, evaluate the expression on the right-hand side to get the value.
            GraveyardValue value = execute_node(gy, node->as.assignment.value);

            // Now, set that value in our global hash table.
            monolith_set(&gy->globals, node->as.assignment.identifier.lexeme, value);

            // Assignment expressions in Graveyard can return the assigned value,
            // allowing for things like 'a = b = 5;'.
            return value;
        }

        case AST_BINARY_OP: {
            // Recursively execute the left and right children to get their values.
            GraveyardValue left = execute_node(gy, node->as.binary_op.left);
            GraveyardValue right = execute_node(gy, node->as.binary_op.right);

            // For now, we only perform math on numbers.
            // A full implementation would check for string concatenation, etc.
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
                fprintf(stderr, "Runtime Error [line %d]: Operands must be numbers for binary operations.\n", node->line);
                return create_null_value();
            }

            // Perform the operation based on the token type.
            switch (node->as.binary_op.operator.type) {
                case ADDITION:       return create_number_value(left.as.number + right.as.number);
                case SUBTRACTION:    return create_number_value(left.as.number - right.as.number);
                case MULTIPLICATION: return create_number_value(left.as.number * right.as.number);
                case DIVISION:
                    if (right.as.number == 0) {
                        fprintf(stderr, "Runtime Error [line %d]: Division by zero.\n", node->line);
                        return create_null_value();
                    }
                    return create_number_value(left.as.number / right.as.number);
                case EXPONENTIATION:
                    return create_number_value(pow(left.as.number, right.as.number));
                
                default:
                    fprintf(stderr, "Runtime Error [line %d]: Unknown binary operator.\n", node->line);
                    return create_null_value();
            }
        }
    }

    // Default return for now. Every path should eventually return a GraveyardValue.
    return create_null_value();
}

// --- Main Application Logic ---

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: graveyard <mode> <source file>\n");
        fprintf(stderr, "Modes:\n");
        fprintf(stderr, "  --tokenize, -t      Tokenize and print tokens to console\n");
        fprintf(stderr, "  --parse, -p         Parse and write the AST to a .gyc file\n");
        fprintf(stderr, "  --execute, -e       Execute the source code\n");
        fprintf(stderr, "  --debug, -d         Execute and print final value for debugging\n");
        return 1;
    }

    Graveyard *gy = graveyard_init(argv[1], argv[2]);
    if (!gy) { return 1; }

    const char *ext = strrchr(gy->filename, '.');
    if (!ext || strcmp(ext, ".gy") != 0) {
        fprintf(stderr, "Error: %s is not Graveyard source code, use the .gy extension\n", gy->filename);
        graveyard_free(gy);
        return 1;
    }

    FILE *file = fopen(gy->filename, "r");
    if (!file) {
        perror("Error opening source file");
        graveyard_free(gy);
        return 1;
    }

    long source_length = 0;
    gy->source_code = load(file, &source_length);
    fclose(file);
    if (!gy->source_code) {
        fprintf(stderr, "Failed to load source file.\n");
        graveyard_free(gy);
        return 1;
    }

    bool success = true;

    if (strcmp(gy->mode, "--tokenize") == 0 || strcmp(gy->mode, "-t") == 0) {
        // This mode ONLY tokenizes for debugging purposes.
        if (!tokenize(gy)) {
            fprintf(stderr, "Tokenization failed.\n");
            success = false;
        } else {
            printf("Tokenization successful. Found %zu tokens.\n", gy->token_count);
        }
    } else if (strcmp(gy->mode, "--parse") == 0 || strcmp(gy->mode, "-p") == 0) {
        // This mode runs the pipeline up to producing the .gyc file.
        if (!tokenize(gy) || !parse(gy)) {
            fprintf(stderr, "Compilation failed.\n");
            success = false;
        } else {
            printf("Parsing successful. AST created.\n");
            // (Code to generate .gyc filename and save AST...)
        }
    } else if (strcmp(gy->mode, "--execute") == 0 || strcmp(gy->mode, "-e") == 0) {
        // This mode runs the full pipeline silently.
        if (!tokenize(gy) || !parse(gy) || !execute(gy)) {
            fprintf(stderr, "Execution failed.\n");
            success = false;
        }
        // No output on success.
    } else if (strcmp(gy->mode, "--debug") == 0 || strcmp(gy->mode, "-d") == 0) {
        // This mode runs the full pipeline and prints the final result.
        if (!tokenize(gy) || !parse(gy) || !execute(gy)) {
            fprintf(stderr, "Debug execution failed.\n");
            success = false;
        } else {
            printf("--- Debug Execution Result ---\n");
            printf("Final statement evaluated to: ");
            print_value(gy->last_executed_value);
            printf("\n");
        }
    } else {
        fprintf(stderr, "Unknown mode: %s\n", gy->mode);
        success = false;
    }
    
    graveyard_free(gy);
    
    return success ? 0 : 1;
}
