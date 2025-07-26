#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef _WIN32
#include <wincrypt.h>
#endif

#ifndef _WIN32
#include <sys/wait.h>
#endif

#ifndef _WIN32
#include <dirent.h>
#endif

#define MAX_LEXEME_LEN 65
#define MAX_STATE_STACK 16

typedef enum {
    SEMICOLON,
    IDENTIFIER,
    TRUEVALUE,
    FALSEVALUE,
    NULLVALUE,
    NUMBER,
    STRING,
    FORMATTEDSTRING,
    ASSIGNMENT,
    PLUS,
    MINUS,
    ASTERISK,
    FORWARDSLASH,
    EXPONENTIATION,
    MODULO,
    PLUSASSIGNMENT,
    SUBTRACTIONASSIGNMENT,
    MULTIPLICATIONASSIGNMENT,
    DIVISIONASSIGNMENT,
    EXPONENTIATIONASSIGNMENT,
    MODULOASSIGNMENT,
    INCREMENT,
    DECREMENT,
    EQUALITY,
    INEQUALITY,
    GREATERTHAN,
    LEFTANGLEBRACKET,
    GREATERTHANEQUAL,
    LESSTHANEQUAL,
    NOT,
    AND,
    OR,
    XOR,
    LEFTPARENTHESES,
    RIGHTPARENTHESES,
    LEFTBRACKET,
    RIGHTBRACKET,
    LEFTBRACE,
    RIGHTBRACE,
    PARAMETER,
    RETURN,
    QUESTIONMARK,
    COMMA,
    COLON,
    WHILE,
    CARET,
    BACKTICK,
    AT,
    NULLCOALESCE,
    PERIOD,
    NAMESPACE,
    REFERENCE,
    PRINT,
    SCAN,
    FILEREAD,
    FILEWRITE,
    RAISE,
    CASTBOOLEAN,
    CASTINTEGER,
    CASTFLOAT,
    CASTSTRING,
    CASTARRAY,
    CASTHASHTABLE,
    TYPEOF,
    TIME,
    WAIT,
    RANDOM,
    EXECUTE,
    CATCONSTANT,
    TOKENEOF,
    FORMATTEDSTART,
    FORMATTEDEND,
    FORMATTEDPART,
    TYPE,
    UNKNOWN
} GraveyardTokenType;

typedef struct {
    GraveyardTokenType type;
    char lexeme[MAX_LEXEME_LEN];
    int line;
    int column;
} Token;

typedef enum {
    STATE_DEFAULT,
    STATE_IN_FMT_STRING
} TokenizerState;

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
    AST_SCAN_STATEMENT,
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
    AST_TERNARY_EXPRESSION,
    AST_ASSERT_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_BREAK_STATEMENT,
    AST_CONTINUE_STATEMENT,
    AST_FOR_STATEMENT,
    AST_RAISE_STATEMENT,
    AST_TIME_EXPRESSION,
    AST_NAMESPACE_DECLARATION,
    AST_NAMESPACE_ACCESS,
    AST_FILEREAD_STATEMENT,
    AST_TYPE_DECLARATION,
    AST_MEMBER_ACCESS,
    AST_THIS_EXPRESSION,
    AST_EXECUTE_EXPRESSION,
    AST_CAT_CONSTANT_EXPRESSION,
    AST_WAIT_STATEMENT,
    AST_RANDOM_EXPRESSION,
    AST_GLOBAL_ACCESS,
    AST_STATIC_ACCESS,
    AST_UID_EXPRESSION,
    AST_SLICE_EXPRESSION,
    AST_ARGV_EXPRESSION,
    AST_EVAL_EXPRESSION,
    AST_EXISTS_EXPRESSION,
    AST_LISTDIR_EXPRESSION,
    AST_TRY_EXCEPT_STATEMENT
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

typedef struct {
    Token variable;
    AstNode* prompt;
} AstNodeScanStatement;

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

typedef struct {
    AstNode* condition;
    Token keyword;
} AstNodeAssertStatement;

typedef struct {
    AstNode* condition;
    AstNode* body;
} AstNodeWhileStatement;

typedef struct {
    Token keyword;
} AstNodeBreakStatement;

typedef struct {
    Token keyword;
} AstNodeContinueStatement;

typedef struct {
    Token iterator;
    AstNode** range_expressions;
    size_t range_count;
    AstNode* body;
} AstNodeForStatement;

typedef struct {
    Token keyword;
    AstNode* error_expr;
} AstNodeRaiseStatement;

typedef struct {
    Token keyword;
} AstNodeTimeExpression;

typedef struct {
    Token name;
    AstNode* body;
} AstNodeNamespaceDeclaration;

typedef struct {
    Token namespace_name;
    Token member_name;
} AstNodeNamespaceAccess;

typedef struct {
    Token variable;
    AstNode* path_expr;
} AstNodeFilereadStatement;

typedef struct {
    Token name;
    AstNode* body;
} AstNodeTypeDeclaration;

typedef struct {
    AstNode* object;
    Token member;
} AstNodeMemberAccess;

typedef struct {
    Token keyword;
} AstNodeThisExpression;

typedef struct {
    AstNode* command_expr;
} AstNodeExecuteExpression;

typedef struct {
    Token keyword;
} AstNodeCatConstantExpression;

typedef struct {
    AstNode* duration_expr;
} AstNodeWaitStatement;

typedef struct {
    Token keyword;
} AstNodeRandomExpression;

typedef struct {
    Token member_name;
} AstNodeGlobalAccess;

typedef struct {
    Token type_name;
    Token member_name;
} AstNodeStaticAccess;

typedef struct {
    AstNode* length_expr;
} AstNodeUidExpression;

typedef struct {
    AstNode* collection;
    AstNode* start_expr;
    AstNode* stop_expr;
    AstNode* step_expr;
} AstNodeSliceExpression;

typedef struct {
    Token keyword;
} AstNodeArgvExpression;

typedef struct {
    AstNode* code_expr;
} AstNodeEvalExpression;

typedef struct {
    AstNode* path_expr;
} AstNodeExistsExpression;

typedef struct {
    AstNode* path_expr;
} AstNodeListdirExpression;

typedef struct {
    Token error_variable;
    AstNode* body;
} AstNodeExceptClause;

typedef struct {
    AstNode* try_block;
    AstNodeExceptClause* except_clause;
    AstNode* finally_block;
} AstNodeTryExceptStatement;


struct AstNode {
    AstNodeType type;
    int line;

    union {
        AstNodeProgram               program;
        AstNodeBinaryOp              binary_op;
        AstNodeUnaryOp               unary_op;
        AstNodeLogicalOp             logical_op;
        AstNodeAssignment            assignment;
        AstNodeIdentifier            identifier;
        AstNodeLiteral               literal;
        AstNodeFormattedString       formatted_string;
        AstNodeArrayLiteral          array_literal;
        AstNodePrintStmt             print_stmt;
        AstNodeScanStatement         scan_statement;
        AstNodeSubscript             subscript;
        AstNodeHashtableLiteral      hashtable_literal;
        AstNodeFunctionDeclaration   function_declaration;
        AstNodeCallExpression        call_expression;
        AstNodeReturnStatement       return_statement;
        AstNodeBlock                 block;
        AstNodeExpressionStatement   expression_statement;
        AstNodeIfStatement           if_statement;
        AstNodeTernaryExpression     ternary_expression;
        AstNodeAssertStatement       assert_statement;
        AstNodeWhileStatement        while_statement;
        AstNodeBreakStatement        break_statement;
        AstNodeContinueStatement     continue_statement;
        AstNodeForStatement          for_statement;
        AstNodeRaiseStatement        raise_statement;
        AstNodeTimeExpression        time_expression;
        AstNodeNamespaceDeclaration  namespace_declaration;
        AstNodeNamespaceAccess       namespace_access;
        AstNodeFilereadStatement     fileread_statement;
        AstNodeTypeDeclaration       type_declaration;
        AstNodeMemberAccess          member_access;
        AstNodeThisExpression        this_expression;
        AstNodeExecuteExpression     execute_expression;
        AstNodeCatConstantExpression cat_constant_expression;
        AstNodeWaitStatement         wait_statement;
        AstNodeRandomExpression      random_expression;
        AstNodeGlobalAccess          global_access;
        AstNodeStaticAccess          static_access;
        AstNodeUidExpression         uid_expression;
        AstNodeSliceExpression       slice_expression;
        AstNodeArgvExpression        argv_expression;
        AstNodeEvalExpression        eval_expression;
        AstNodeExistsExpression      exists_expression;
        AstNodeListdirExpression     listdir_expression;
        AstNodeTryExceptStatement    try_except_statement;
    } as;
};

typedef struct GraveyardString GraveyardString;
typedef struct GraveyardValue GraveyardValue;
typedef struct GraveyardArray GraveyardArray;
typedef struct GraveyardHashtable GraveyardHashtable;
typedef struct GraveyardFunction GraveyardFunction;
typedef struct Environment Environment;
typedef struct GraveyardType GraveyardType;
typedef struct GraveyardInstance GraveyardInstance;
typedef struct GraveyardBoundMethod GraveyardBoundMethod;

typedef enum {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_ARRAY,
    VAL_HASHTABLE,
    VAL_FUNCTION,
    VAL_ENVIRONMENT,
    VAL_TYPE,
    VAL_INSTANCE,
    VAL_BOUND_METHOD
} ValueType;

struct GraveyardValue {
    ValueType type;
    union {
        bool                  boolean;
        double                number;
        GraveyardString*      string;
        GraveyardArray*       array;
        GraveyardHashtable*   hashtable;
        GraveyardFunction*    function;
        Environment*          environment;
        GraveyardType*        type;
        GraveyardInstance*    instance;
        GraveyardBoundMethod* bound_method;
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

struct GraveyardType {
    int ref_count;
    GraveyardString* name;
    Monolith fields;
    Monolith methods;
};

struct GraveyardInstance {
    int ref_count;
    GraveyardType* type;
    Monolith fields;
};

struct GraveyardBoundMethod {
    int ref_count;
    GraveyardValue receiver;
    GraveyardValue function;
};

typedef struct {
    char** lines;
    int count;
} Lines;

typedef struct {
    char** items;
    size_t count;
    size_t capacity;
} StringList;

typedef struct {
    StringList included_files;
    char* output_buffer;
    size_t output_len;
    size_t output_capacity;
} Preprocessor;

typedef struct {
    const char *mode;
    const char *filename;
    char *source_code;
    Token *tokens;
    size_t token_count;
    AstNode *ast_root;
    Monolith namespaces;
    GraveyardValue arguments;
    Environment* environment;
    GraveyardValue last_executed_value;
    bool is_returning;
    GraveyardValue return_value;
    bool encountered_break;
    bool encountered_continue;
    bool had_runtime_error;
    char error_message[1024];
    int error_line;
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

//PREPROCESS-----------------------------------------------------------------------------------

static char* extract_graveyard_code(const char* file_content) {
    const char* start_marker = strstr(file_content, "::{");
    if (!start_marker) return NULL;
    const char* start_ptr = start_marker + 3;

    const char* scanner = start_ptr;
    int brace_depth = 1;
    const char* end_ptr = NULL;
    while (*scanner != '\0') {
        if (*scanner == '"') { scanner++; while (*scanner != '\0' && (*scanner != '"' || *(scanner - 1) == '\\')) scanner++; }
        else if (*scanner == '\'') { scanner++; while (*scanner != '\0' && (*scanner != '\'' || *(scanner - 1) == '\\')) scanner++; }
        else if (*scanner == '{') brace_depth++;
        else if (*scanner == '}') {
            brace_depth--;
            if (brace_depth == 0) {
                end_ptr = scanner;
                break;
            }
        }
        if (*scanner != '\0') scanner++;
    }
    if (end_ptr == NULL) return NULL;

    size_t len = end_ptr - start_ptr;
    char* code = malloc(len + 1);
    strncpy(code, start_ptr, len);
    code[len] = '\0';
    return code;
}

static bool preprocess_source(Preprocessor* pp, const char* source) {
    const char* current = source;
    while (*current != '\0') {
        if (*current == '@' && *(current + 1) == '"') {
            const char* path_start = current + 2;
            const char* path_end = strchr(path_start, '"');
            if (!path_end) {
                fprintf(stderr, "Preprocessor error: Unterminated path string in import statement.\n");
                return false;
            }

            size_t path_len = path_end - path_start;
            char* path = malloc(path_len + 1);
            if (!path) {
                perror("Preprocessor: malloc for path failed");
                return false;
            }
            strncpy(path, path_start, path_len);
            path[path_len] = '\0';

            bool already_included = false;
            for (size_t i = 0; i < pp->included_files.count; i++) {
                if (strcmp(pp->included_files.items[i], path) == 0) {
                    already_included = true;
                    break;
                }
            }

            if (!already_included) {
                if (pp->included_files.count >= pp->included_files.capacity) {
                    size_t new_capacity = pp->included_files.capacity * 2;
                    if (new_capacity == 0) new_capacity = 8;
                    char** temp_items = realloc(pp->included_files.items, new_capacity * sizeof(char*));
                    if (!temp_items) {
                        perror("Preprocessor: realloc for included_files failed");
                        free(path);
                        return false;
                    }
                    pp->included_files.items = temp_items;
                    pp->included_files.capacity = new_capacity;
                }
                pp->included_files.items[pp->included_files.count++] = strdup(path);

                FILE* import_file = fopen(path, "r");
                if (!import_file) {
                    fprintf(stderr, "Preprocessor error: Cannot open import file '%s'.\n", path);
                    free(path);
                    return false;
                }
                char* import_content = load(import_file, NULL);
                fclose(import_file);
                
                char* import_code = extract_graveyard_code(import_content);
                if (import_code) {
                    if (!preprocess_source(pp, import_code)) {
                        free(import_code);
                        free(import_content);
                        free(path);
                        return false;
                    }
                    free(import_code);
                } else {
                    fprintf(stderr, "Preprocessor error: Import file '%s' does not contain a valid '::{...}' scope.\n", path);
                    free(import_content);
                    free(path);
                    return false;
                }
                free(import_content);
            }
            
            free(path); 
            
            current = path_end + 1;
            while (*current != '\0' && isspace((unsigned char)*current)) {
                current++;
            }
            if (*current == ';') current++;

        } else {
            if (pp->output_len + 1 >= pp->output_capacity) {
                size_t new_capacity = pp->output_capacity * 2;
                if (new_capacity == 0) new_capacity = 256;
                char* temp_buffer = realloc(pp->output_buffer, new_capacity);
                if (!temp_buffer) {
                    perror("Preprocessor: realloc for output_buffer failed");
                    return false;
                }
                pp->output_buffer = temp_buffer;
                pp->output_capacity = new_capacity;
            }
            pp->output_buffer[pp->output_len++] = *current;
            current++;
        }
    }
    return true;
}

char* run_preprocessor(const char* initial_source) {
    char* main_code = extract_graveyard_code(initial_source);
    if (!main_code) {
        fprintf(stderr, "Preprocessor error: Could not find main '::{...}' scope in the initial file.\n");
        return NULL;
    }

    Preprocessor pp;
    pp.included_files.count = 0;
    pp.included_files.capacity = 8;
    pp.included_files.items = malloc(pp.included_files.capacity * sizeof(char*));
    
    pp.output_len = 0;
    pp.output_capacity = strlen(main_code) + 1024;
    pp.output_buffer = malloc(pp.output_capacity);

    if (!pp.included_files.items || !pp.output_buffer) {
        perror("Preprocessor: Initial malloc failed");
        free(main_code);
        free(pp.included_files.items);
        free(pp.output_buffer);
        return NULL;
    }

    bool success = preprocess_source(&pp, main_code);
    
    free(main_code);
    for (size_t i = 0; i < pp.included_files.count; i++) {
        free(pp.included_files.items[i]);
    }
    free(pp.included_files.items);

    if (!success) {
        free(pp.output_buffer);
        return NULL;
    }

    pp.output_buffer[pp.output_len] = '\0';
    return pp.output_buffer;
}

//TOKENIZE-----------------------------------------------------------------------------------

GraveyardTokenType identify_three_char_token(char c1, char c2, char c3) {
    if (c1 == '*' && c2 == '*' && c3 == '=') return EXPONENTIATIONASSIGNMENT;
    if (c1 == '!' && c2 == '|' && c3 == '|') return XOR;
    if (c1 == '!' && c2 == '>' && c3 == '>') return RAISE;
    if (c1 == ':' && c2 == '<' && c3 == '<') return FILEREAD;
    if (c1 == ':' && c2 == '>' && c3 == '>') return FILEWRITE;
    if (c1 == '/' && c2 == '%' && c3 == '=') return MODULOASSIGNMENT;
    if (c1 == '^' && c2 == '_' && c3 == '^') return CATCONSTANT;
    return UNKNOWN;
}

GraveyardTokenType identify_two_char_token(char c1, char c2) {
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
    if (c1 == '?' && c2 == '?') return NULLCOALESCE;
    if (c1 == ':' && c2 == '~') return WAIT;
    if (c1 == ':' && c2 == '?') return RANDOM;
    if (c1 == '!' && c2 == '!') return EXECUTE;
    return UNKNOWN;
}

GraveyardTokenType identify_single_char_token(char c) {
    switch (c) {
        case '=': return ASSIGNMENT;
        case ';': return SEMICOLON;
        case '+': return PLUS;
        case '-': return MINUS;
        case '*': return ASTERISK;
        case '/': return FORWARDSLASH;
        case '(': return LEFTPARENTHESES;
        case ')': return RIGHTPARENTHESES;
        case '>': return GREATERTHAN;
        case '<': return LEFTANGLEBRACKET;
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
        case '^': return CARET;
        case '`': return BACKTICK;
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

    size_t capacity = 16;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    if (!tokens) {
        perror("tokenize: malloc failed");
        return false;
    }

    TokenizerState state_stack[MAX_STATE_STACK];
    int state_top = -1; 

    int fstring_brace_depth_stack[MAX_STATE_STACK];
    int fstring_depth_top = -1;

    if (state_top + 1 >= MAX_STATE_STACK) {
        fprintf(stderr, "Tokenizer error: State stack overflow (too much nesting).\n");
        goto cleanup_failure;
    }
    state_stack[++state_top] = STATE_DEFAULT;
    
    size_t line = 1;
    const char* line_start_ptr = source;
    const char* current_ptr = source;

    while (*current_ptr != '\0') {
        if (count + 1 >= capacity) {
            size_t new_capacity = capacity * 2;
            Token *new_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!new_tokens) { perror("tokenize: realloc failed"); goto cleanup_failure; }
            tokens = new_tokens;
            capacity = new_capacity;
        }

        TokenizerState current_state = state_stack[state_top];
        int column = (current_ptr - line_start_ptr) + 1;
        char c = *current_ptr;

        if (current_state == STATE_IN_FMT_STRING) {
            if (c == '\'') {
                state_top--;
                tokens[count].type = FORMATTEDEND;
                tokens[count].lexeme[0] = '\''; tokens[count].lexeme[1] = '\0';
                tokens[count].line = line; tokens[count].column = column;
                count++;
                current_ptr++;
                continue;
            }
            if (c == '{') {
                state_stack[++state_top] = STATE_DEFAULT; 
                fstring_brace_depth_stack[++fstring_depth_top] = 1;
                
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
            while (*current_ptr != '\0' && *current_ptr != '\'' && *current_ptr != '{' && *current_ptr != '\n') {
                current_ptr++;
            }
            size_t len = current_ptr - start;
            if (len > 0) {
                 if (len >= MAX_LEXEME_LEN) {
                    fprintf(stderr, "Tokenizer error [line %zu, col %d]: Literal part of formatted string is too long.\n", line, column);
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

        if (strncmp(current_ptr, "//", 2) == 0) {
            while (*current_ptr != '\0' && *current_ptr != '\n') { current_ptr++; }
            continue;
        }
        if (strncmp(current_ptr, "/*", 2) == 0) {
            int comment_start_line = line;
            int comment_start_col = column;
            current_ptr += 2;
            while (*current_ptr != '\0' && strncmp(current_ptr, "*/", 2) != 0) {
                 if (*current_ptr == '\n') { line++; line_start_ptr = current_ptr + 1; }
                 current_ptr++;
            }
            if (*current_ptr == '\0') {
                fprintf(stderr, "Tokenizer error [line %d, col %d]: Unterminated block comment.\n", comment_start_line, comment_start_col);
                goto cleanup_failure;
            }
            current_ptr += 2;
            continue;
        }
        if (isspace((unsigned char)c)) {
            if (c == '\n') { line++; line_start_ptr = current_ptr + 1; }
            current_ptr++;
            continue;
        }

        if (c == '\'') {
            if (state_top + 1 >= MAX_STATE_STACK) {
                fprintf(stderr, "Tokenizer error [line %zu]: Formatted string nesting level too deep.\n", line);
                goto cleanup_failure;
            }
            state_stack[++state_top] = STATE_IN_FMT_STRING;
            
            tokens[count].type = FORMATTEDSTART;
            tokens[count].lexeme[0] = '\''; tokens[count].lexeme[1] = '\0';
            tokens[count].line = line; tokens[count].column = column;
            count++;
            current_ptr++;
            continue;
        }
        
        if (fstring_depth_top > -1) {
            if (c == '{') {
                fstring_brace_depth_stack[fstring_depth_top]++;
            } else if (c == '}') {
                fstring_brace_depth_stack[fstring_depth_top]--;
                if (fstring_brace_depth_stack[fstring_depth_top] == 0) {
                    fstring_depth_top--;
                    state_top--;
                }
            }
        }
        
        if (isalpha((unsigned char)c) || c == '_') {
            const char* start = current_ptr;
            while (*current_ptr != '\0' && (isalnum((unsigned char)*current_ptr) || *current_ptr == '_')) { current_ptr++; }
            size_t len = current_ptr - start;
            if (len >= MAX_LEXEME_LEN) {
                fprintf(stderr, "Tokenizer error [line %d, col %d]: Identifier is too long.\n", line, column); goto cleanup_failure;
            }
            tokens[count].type = IDENTIFIER;
            strncpy(tokens[count].lexeme, start, len); tokens[count].lexeme[len] = '\0';
            tokens[count].line = line; tokens[count].column = column;
            count++;

        } else if (isdigit((unsigned char)c) || (c == '.' && *(current_ptr + 1) != '\0' && isdigit((unsigned char)*(current_ptr+1)))) {
            const char* start = current_ptr;
            while (*current_ptr != '\0' && isdigit((unsigned char)*current_ptr)) { current_ptr++; }
            if (*current_ptr == '.' && *(current_ptr + 1) != '\0' && isdigit((unsigned char)*(current_ptr + 1))) {
                current_ptr++;
                while (*current_ptr != '\0' && isdigit((unsigned char)*current_ptr)) { current_ptr++; }
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
            int start_line = line;
            int start_col = column;
            current_ptr++;

            char lexeme_buffer[MAX_LEXEME_LEN];
            size_t len = 0;

            while (*current_ptr != '\0' && *current_ptr != '"') {
                char ch = *current_ptr;
                if (ch == '\n') {
                    fprintf(stderr, "Tokenizer error [line %d, col %d]: Unterminated string literal (newline encountered).\n", line, column);
                    goto cleanup_failure;
                }
                if (len >= MAX_LEXEME_LEN - 1) {
                    fprintf(stderr, "Tokenizer error [line %d, col %d]: String literal is too long.\n", start_line, start_col);
                    goto cleanup_failure;
                }
                if (ch == '\\' && *(current_ptr + 1) != '\0') {
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

            if (*current_ptr != '"') {
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
                if (*lookahead_ptr != '\0' && (isalpha((unsigned char)*lookahead_ptr) || *lookahead_ptr == '_')) {
                    lookahead_ptr++;
                    while (*lookahead_ptr != '\0' && (isalnum((unsigned char)*lookahead_ptr) || *lookahead_ptr == '_')) { lookahead_ptr++; }
                    if (*lookahead_ptr != '\0' && *lookahead_ptr == '>') {
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

            GraveyardTokenType ttype = UNKNOWN;

            if (*(start + 1) != '\0' && *(start + 2) != '\0') {
                ttype = identify_three_char_token(start[0], start[1], start[2]);
                if (ttype != UNKNOWN) {
                    snprintf(tokens[count].lexeme, 4, "%c%c%c", start[0], start[1], start[2]);
                    tokens[count].type = ttype; tokens[count].line = line; tokens[count].column = column;
                    count++; current_ptr += 3; continue;
                }
            }
            if (*(start + 1) != '\0') {
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

    if (state_top != 0) {
        fprintf(stderr, "Tokenizer error [line %zu]: Unterminated formatted string at end of file.\n", line);
        goto cleanup_failure;
    }

    tokens[count].type = TOKENEOF;
    tokens[count].lexeme[0] = '\0';
    tokens[count].line = line;
    tokens[count].column = (current_ptr - line_start_ptr) + 1;
    count++;
    
    Token *shrunk_tokens = realloc(tokens, count * sizeof(Token));
    if (shrunk_tokens) { tokens = shrunk_tokens; }
    
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
        case AST_SCAN_STATEMENT:
            free_ast(node->as.scan_statement.prompt);
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
            free_ast(node->as.assignment.left);
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
        case AST_ASSERT_STATEMENT:
            free_ast(node->as.assert_statement.condition);
            break;
        case AST_WHILE_STATEMENT:
            free_ast(node->as.while_statement.condition);
            free_ast(node->as.while_statement.body);
            break;
        case AST_FOR_STATEMENT:
            for (size_t i = 0; i < node->as.for_statement.range_count; i++) {
                free_ast(node->as.for_statement.range_expressions[i]);
            }
            free(node->as.for_statement.range_expressions);
            free_ast(node->as.for_statement.body);
            break;
        case AST_RAISE_STATEMENT:
            free_ast(node->as.raise_statement.error_expr);
            break;
        case AST_NAMESPACE_DECLARATION:
            free_ast(node->as.namespace_declaration.body);
            break;
        case AST_FILEREAD_STATEMENT:
            free_ast(node->as.fileread_statement.path_expr);
            break;
        case AST_TYPE_DECLARATION:
            free_ast(node->as.type_declaration.body);
            break;
        case AST_MEMBER_ACCESS:
            free_ast(node->as.member_access.object);
            break;
        case AST_EXECUTE_EXPRESSION:
            free_ast(node->as.execute_expression.command_expr);
            break;
        case AST_WAIT_STATEMENT:
            free_ast(node->as.wait_statement.duration_expr);
            break;
        case AST_UID_EXPRESSION:
            free_ast(node->as.uid_expression.length_expr);
            break;
        case AST_SLICE_EXPRESSION:
            free_ast(node->as.slice_expression.collection);
            free_ast(node->as.slice_expression.start_expr);
            free_ast(node->as.slice_expression.stop_expr);
            free_ast(node->as.slice_expression.step_expr);
            break;
        case AST_EVAL_EXPRESSION:
            free_ast(node->as.eval_expression.code_expr);
            break;
        case AST_EXISTS_EXPRESSION:
            free_ast(node->as.exists_expression.path_expr);
            break;
        case AST_LISTDIR_EXPRESSION:
            free_ast(node->as.listdir_expression.path_expr);
            break;
        case AST_TRY_EXCEPT_STATEMENT:
            free_ast(node->as.try_except_statement.try_block);
            if (node->as.try_except_statement.except_clause) {
                free_ast(node->as.try_except_statement.except_clause->body);
                free(node->as.try_except_statement.except_clause);
            }
            free_ast(node->as.try_except_statement.finally_block);
            break;
        case AST_ARGV_EXPRESSION:
        case AST_STATIC_ACCESS:
        case AST_GLOBAL_ACCESS:
        case AST_RANDOM_EXPRESSION:
        case AST_CAT_CONSTANT_EXPRESSION:
        case AST_THIS_EXPRESSION:
        case AST_NAMESPACE_ACCESS:
        case AST_TIME_EXPRESSION:
        case AST_BREAK_STATEMENT:
        case AST_CONTINUE_STATEMENT:
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

static Token* expect(Parser* parser, GraveyardTokenType type, const char* message) {
    if (peek(parser)->type == type) {
        return consume(parser);
    }
    error_at_token(parser, peek(parser), message);
    return NULL;
}

static bool match(Parser* parser, GraveyardTokenType type) {
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

static int get_operator_precedence(GraveyardTokenType type) {
    switch (type) {
        case ASSIGNMENT:
            return 1;
        case QUESTIONMARK:
            return 2;
        case NULLCOALESCE:
            return 3;
        case FILEWRITE:
            return 4;
        case OR:
            return 5;
        case XOR:
            return 6;
        case AND:
            return 7;
        case EQUALITY:
        case INEQUALITY:
            return 8;
        case LEFTANGLEBRACKET:
        case GREATERTHAN:
        case LESSTHANEQUAL:
        case GREATERTHANEQUAL:
            return 9;
        case PLUS:
        case MINUS:
            return 10;
        case ASTERISK:
        case FORWARDSLASH:
        case MODULO:
            return 11;
        case EXPONENTIATION:
            return 12;
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
    if (!node->as.block.statements) {
        perror("AST block statements malloc failed");
        free(node);
        parser->had_error = true;
        return NULL;
    }

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
        if (!statement) {
            break;
        }
        
        node->as.block.statements[node->as.block.count++] = statement;

        if (statement->type != AST_FUNCTION_DECLARATION &&
            statement->type != AST_IF_STATEMENT &&
            statement->type != AST_WHILE_STATEMENT)
        {
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

static AstNode* parse_namespace_declaration(Parser* parser) {
    AstNode* node = create_node(parser, AST_NAMESPACE_DECLARATION);
    node->line = parser->tokens[parser->current - 1].line;

    Token name = *expect(parser, IDENTIFIER, "Expected namespace name after '::'.");
    node->as.namespace_declaration.name = name;

    expect(parser, LEFTBRACE, "Expected '{' to begin namespace body.");
    node->as.namespace_declaration.body = parse_block(parser);

    return node;
}

static AstNode* parse_fileread_statement(Parser* parser) {
    Token variable = parser->tokens[parser->current - 2];
    AstNode* node = create_node(parser, AST_FILEREAD_STATEMENT);
    node->line = variable.line;
    node->as.fileread_statement.variable = variable;
    node->as.fileread_statement.path_expr = parse_expression(parser, 1);
    
    return node;
}

static AstNode* parse_uid_expression(Parser* parser) {
    AstNode* node = create_node(parser, AST_UID_EXPRESSION);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.uid_expression.length_expr = parse_expression(parser, 1);
    return node;
}

static AstNode* parse_execute_or_eval_expression(Parser* parser) {
    Token op_token = parser->tokens[parser->current - 1];

    if (match(parser, AT)) {
        AstNode* node = create_node(parser, AST_EXECUTE_EXPRESSION);
        node->line = op_token.line;
        node->as.execute_expression.command_expr = parse_expression(parser, 1);
        return node;
    } else {
        AstNode* node = create_node(parser, AST_EVAL_EXPRESSION);
        node->line = op_token.line;
        node->as.eval_expression.code_expr = parse_expression(parser, 1);
        return node;
    }
}

static AstNode* parse_primary(Parser* parser) {
    if (peek(parser)->type == FILEREAD && parser->tokens[parser->current + 1].type == AT) {
        consume(parser);
        consume(parser);
        
        AstNode* node = create_node(parser, AST_LISTDIR_EXPRESSION);
        node->line = parser->tokens[parser->current - 1].line;
        node->as.listdir_expression.path_expr = parse_expression(parser, 1);
        return node;
    }

    if (peek(parser)->type == NULLCOALESCE && parser->tokens[parser->current + 1].type == AT) {
        consume(parser);
        consume(parser);
        
        AstNode* node = create_node(parser, AST_EXISTS_EXPRESSION);
        node->line = parser->tokens[parser->current - 1].line;
        node->as.exists_expression.path_expr = parse_expression(parser, 1);
        return node;
    }

    if (match(parser, COLON)) {
        AstNode* node = create_node(parser, AST_ARGV_EXPRESSION);
        node->line = parser->tokens[parser->current - 1].line;
        node->as.argv_expression.keyword = parser->tokens[parser->current - 1];
        return node;
    }

    if (match(parser, REFERENCE)) {
        return parse_uid_expression(parser);
    }

    if (match(parser, NAMESPACE)) {
        if (peek(parser)->type == REFERENCE) {
            consume(parser);
            Token member_name = *expect(parser, IDENTIFIER, "Expected member name after '::#'.");
            AstNode* node = create_node(parser, AST_GLOBAL_ACCESS);
            node->line = member_name.line;
            node->as.global_access.member_name = member_name;
            return node;

        } else if (peek(parser)->type == TYPE) {
            Token type_name = *consume(parser);
            expect(parser, REFERENCE, "Expected '#' after type name for static member access.");
            Token member_name = *expect(parser, IDENTIFIER, "Expected member name after '#'.");

            AstNode* node = create_node(parser, AST_STATIC_ACCESS);
            node->line = type_name.line;
            node->as.static_access.type_name = type_name;
            node->as.static_access.member_name = member_name;
            return node;

        } else {
            AstNode* node = create_node(parser, AST_NAMESPACE_ACCESS);
            node->line = parser->tokens[parser->current - 1].line;
            Token namespace_name = *expect(parser, IDENTIFIER, "Expected namespace name after '::'.");
            expect(parser, REFERENCE, "Expected '#' after namespace name for member access.");
            Token member_name = *expect(parser, IDENTIFIER, "Expected member name after '#'.");
            node->as.namespace_access.namespace_name = namespace_name;
            node->as.namespace_access.member_name = member_name;
            return node;
        }
    }
    if (match(parser, RANDOM)) {
        AstNode* node = create_node(parser, AST_RANDOM_EXPRESSION);
        node->line = parser->tokens[parser->current - 1].line;
        node->as.random_expression.keyword = parser->tokens[parser->current - 1];
        return node;
    }
    
    if (match(parser, CATCONSTANT)) {
        AstNode* node = create_node(parser, AST_CAT_CONSTANT_EXPRESSION);
        node->line = parser->tokens[parser->current - 1].line;
        node->as.cat_constant_expression.keyword = parser->tokens[parser->current - 1];
        return node;
    }

    if (match(parser, EXECUTE)) {
        return parse_execute_or_eval_expression(parser);
    }

    if (match(parser, TYPE)) {
        Token type_token = parser->tokens[parser->current - 1];
        AstNode* node = create_node(parser, AST_LITERAL);
        if (!node) return NULL;
        node->line = type_token.line;
        node->as.literal.value = type_token;
        return node;
    }

    if (match(parser, PERIOD)) {
        Token dot_token = parser->tokens[parser->current - 1];
        
        AstNode* this_node = create_node(parser, AST_THIS_EXPRESSION);
        if (!this_node) return NULL;
        this_node->line = dot_token.line;
        this_node->as.this_expression.keyword = dot_token;

        Token member_name = *expect(parser, IDENTIFIER, "Expected member name after '.'.");
        
        AstNode* access_node = create_node(parser, AST_MEMBER_ACCESS);
        if (!access_node) { free_ast(this_node); return NULL; }

        access_node->line = member_name.line;
        access_node->as.member_access.object = this_node;
        access_node->as.member_access.member = member_name;
        
        return access_node;
    }

    if (match(parser, LEFTBRACE)) return parse_hashtable_literal(parser);
    if (match(parser, LEFTBRACKET)) return parse_array_literal(parser);
    if (match(parser, FORMATTEDSTART)) return parse_formatted_string(parser);
    if (match(parser, LEFTPARENTHESES)) {
        AstNode* expr = parse_expression(parser, 1);
        expect(parser, RIGHTPARENTHESES, "Expected ')' after expression.");
        return expr;
    }
    if (match(parser, MINUS) || match(parser, NOT) || match(parser, TYPEOF) ||
        match(parser, CASTBOOLEAN) || match(parser, CASTINTEGER) || match(parser, CASTFLOAT) ||
        match(parser, CASTSTRING) || match(parser, CASTARRAY) || match(parser, CASTHASHTABLE) ||
        match(parser, ASTERISK) || match(parser, CARET) || match(parser, BACKTICK)) {
        Token operator_token = parser->tokens[parser->current - 1];
        AstNode* right = parse_expression(parser, 12);
        if (!right) return NULL;
        AstNode* node = create_node(parser, AST_UNARY_OP);
        if (!node) { free_ast(right); return NULL; }
        node->line = operator_token.line;
        node->as.unary_op.operator = operator_token;
        node->as.unary_op.right = right;
        return node;
    }
    if (match(parser, TIME)) {
        AstNode* node = create_node(parser, AST_TIME_EXPRESSION);
        node->line = parser->tokens[parser->current - 1].line;
        node->as.time_expression.keyword = parser->tokens[parser->current - 1];
        return node;
    }
    if (match(parser, STRING) || match(parser, NUMBER) || 
        match(parser, TRUEVALUE) || match(parser, FALSEVALUE) || match(parser, NULLVALUE)) {
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
            AstNode* start_expr = NULL;
            AstNode* stop_expr = NULL;
            AstNode* step_expr = NULL;
            bool is_slice = false;

            if (peek(parser)->type == NAMESPACE) {
                consume(parser);
                is_slice = true;
                if (peek(parser)->type != RIGHTBRACKET) {
                    step_expr = parse_expression(parser, 1);
                }
            } else {
                if (peek(parser)->type != COLON) {
                    start_expr = parse_expression(parser, 1);
                }

                if (match(parser, COLON)) {
                    is_slice = true;
                    if (peek(parser)->type != COLON && peek(parser)->type != RIGHTBRACKET) {
                        stop_expr = parse_expression(parser, 1);
                    }
                    if (match(parser, COLON)) {
                        if (peek(parser)->type != RIGHTBRACKET) {
                            step_expr = parse_expression(parser, 1);
                        }
                    }
                }
            }

            expect(parser, RIGHTBRACKET, "Expected ']' to close subscript or slice.");

            if (is_slice) {
                AstNode* slice_node = create_node(parser, AST_SLICE_EXPRESSION);
                slice_node->line = expr->line;
                slice_node->as.slice_expression.collection = expr;
                slice_node->as.slice_expression.start_expr = start_expr;
                slice_node->as.slice_expression.stop_expr = stop_expr;
                slice_node->as.slice_expression.step_expr = step_expr;
                expr = slice_node;
            } else {
                AstNode* subscript_node = create_node(parser, AST_SUBSCRIPT);
                subscript_node->line = expr->line;
                subscript_node->as.subscript.array = expr;
                subscript_node->as.subscript.index = start_expr;
                expr = subscript_node;
            }

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

        } else if (match(parser, PERIOD)) {
            Token member_name = *expect(parser, IDENTIFIER, "Expected member name after '.'.");
            
            AstNode* access_node = create_node(parser, AST_MEMBER_ACCESS);
            if (!access_node) { free_ast(expr); return NULL; }

            access_node->line = member_name.line;
            access_node->as.member_access.object = expr;
            access_node->as.member_access.member = member_name;
            
            expr = access_node;

        } else {
            break;
        }
    }

    return expr;
}

static AstNode* parse_for_statement(Parser* parser) {
    Token iterator = parser->tokens[parser->current - 2];
    AstNode* node = create_node(parser, AST_FOR_STATEMENT);
    node->line = iterator.line;
    node->as.for_statement.iterator = iterator;
    node->as.for_statement.body = NULL;
    
    node->as.for_statement.range_expressions = malloc(3 * sizeof(AstNode*));
    node->as.for_statement.range_count = 0;

    do {
        if (node->as.for_statement.range_count >= 3) {
            error_at_token(parser, peek(parser), "For loop can have at most 3 range arguments.");
            free_ast(node);
            return NULL;
        }
        node->as.for_statement.range_expressions[node->as.for_statement.range_count++] = parse_expression(parser, 1);
    } while (match(parser, COMMA));

    expect(parser, LEFTBRACE, "Expected '{' to begin for loop body.");
    node->as.for_statement.body = parse_block(parser);

    return node;
}

static AstNode* parse_while_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_WHILE_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.while_statement.condition = parse_expression(parser, 1);
    expect(parser, LEFTBRACE, "Expected '{' after while condition.");
    node->as.while_statement.body = parse_block(parser);
    return node;
}

static AstNode* parse_break_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_BREAK_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.break_statement.keyword = parser->tokens[parser->current - 1];
    return node;
}

static AstNode* parse_continue_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_CONTINUE_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.continue_statement.keyword = parser->tokens[parser->current - 1];
    return node;
}

static AstNode* parse_expression(Parser* parser, int min_precedence) {
    AstNode* left = parse_postfix(parser);
    if (!left) return NULL;

    while (true) {
        GraveyardTokenType op_type = peek(parser)->type;
        int current_precedence = get_operator_precedence(op_type);

        if (current_precedence == 0 || current_precedence < min_precedence) {
            break;
        }

        if (op_type == ASSIGNMENT) {
            consume(parser);
            
            if (left->type != AST_IDENTIFIER && left->type != AST_SUBSCRIPT && left->type != AST_MEMBER_ACCESS &&
                !(left->type == AST_BINARY_OP && left->as.binary_op.operator.type == REFERENCE)) {
                error_at_token(parser, &parser->tokens[parser->current - 2], "Invalid assignment target.");
                free_ast(left);
                return NULL;
            }

            AstNode* value = parse_expression(parser, current_precedence);
            
            AstNode* assignment_node = create_node(parser, AST_ASSIGNMENT);
            assignment_node->line = left->line;
            assignment_node->as.assignment.left = left;
            assignment_node->as.assignment.value = value;
            left = assignment_node;
            continue;
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
        if (op_type == AND || op_type == OR) {
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

static AstNode* parse_scan_statement(Parser* parser) {
    Token variable = parser->tokens[parser->current - 2];
    AstNode* node = create_node(parser, AST_SCAN_STATEMENT);
    node->line = variable.line;
    node->as.scan_statement.variable = variable;
    node->as.scan_statement.prompt = parse_expression(parser, 1);
    
    return node;
}

static AstNode* parse_raise_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_RAISE_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.raise_statement.keyword = parser->tokens[parser->current - 1];
    node->as.raise_statement.error_expr = parse_expression(parser, 1);
    return node;
}

static bool is_compound_assignment(GraveyardTokenType type) {
    switch (type) {
        case PLUSASSIGNMENT:
        case SUBTRACTIONASSIGNMENT:
        case MULTIPLICATIONASSIGNMENT:
        case DIVISIONASSIGNMENT:
        case EXPONENTIATIONASSIGNMENT:
        case MODULOASSIGNMENT:
            return true;
        default:
            return false;
    }
}

static GraveyardTokenType get_base_operator(GraveyardTokenType compound_type) {
    switch (compound_type) {
        case PLUSASSIGNMENT:            return PLUS;
        case SUBTRACTIONASSIGNMENT:     return MINUS;
        case MULTIPLICATIONASSIGNMENT:  return ASTERISK;
        case DIVISIONASSIGNMENT:        return FORWARDSLASH;
        case EXPONENTIATIONASSIGNMENT:  return EXPONENTIATION;
        case MODULOASSIGNMENT:          return MODULO;
        default:                        return UNKNOWN;
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

    GraveyardTokenType base_op_type = get_base_operator(compound_op.type);
    binary_op_node->as.binary_op.operator.type = base_op_type;
    
    switch (base_op_type) {
        case PLUS:           snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, "+"); break;
        case MINUS:          snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, "-"); break;
        case ASTERISK:       snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, "*"); break;
        case FORWARDSLASH:   snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, "/"); break;
        case EXPONENTIATION: snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, "**"); break;
        case MODULO:         snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, "/%"); break;
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
    
    snprintf(right_side->as.literal.value.lexeme, MAX_LEXEME_LEN, "1");

    AstNode* binary_op_node = create_node(parser, AST_BINARY_OP);
    if (!binary_op_node) { free_ast(left_side); free_ast(right_side); return NULL; }
    binary_op_node->line = op.line;
    binary_op_node->as.binary_op.operator.type = (op.type == INCREMENT) ? PLUS : MINUS;
    
    snprintf(binary_op_node->as.binary_op.operator.lexeme, MAX_LEXEME_LEN, (op.type == INCREMENT) ? "+" : "-");
    
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

static AstNode* parse_if_statement_after_condition(Parser* parser, AstNode* condition) {
    AstNode* node = create_node(parser, AST_IF_STATEMENT);
    node->line = condition->line;
    node->as.if_statement.condition = condition;

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

static AstNode* parse_type_declaration(Parser* parser) {
    Token type_name_token = *consume(parser);

    AstNode* node = create_node(parser, AST_TYPE_DECLARATION);
    if (!node) return NULL;
    
    node->line = type_name_token.line;
    node->as.type_declaration.name = type_name_token;

    expect(parser, LEFTBRACE, "Expected '{' to begin type body.");
    node->as.type_declaration.body = parse_block(parser);

    return node;
}

static AstNode* parse_wait_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_WAIT_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.wait_statement.duration_expr = parse_expression(parser, 1);
    return node;
}

static AstNode* parse_try_except_statement(Parser* parser) {
    AstNode* node = create_node(parser, AST_TRY_EXCEPT_STATEMENT);
    node->line = parser->tokens[parser->current - 1].line;
    node->as.try_except_statement.except_clause = NULL;
    node->as.try_except_statement.finally_block = NULL;

    expect(parser, LEFTBRACE, "Expected '{' to begin 'try' block.");
    node->as.try_except_statement.try_block = parse_block(parser);

    if (match(parser, COMMA)) {
        node->as.try_except_statement.except_clause = malloc(sizeof(AstNodeExceptClause));
        expect(parser, PARAMETER, "Expected '&' to introduce error variable in 'except' block.");
        node->as.try_except_statement.except_clause->error_variable = *expect(parser, IDENTIFIER, "Expected variable name for error.");
        expect(parser, LEFTBRACE, "Expected '{' to begin 'except' block.");
        node->as.try_except_statement.except_clause->body = parse_block(parser);
    }

    if (match(parser, COLON)) {
        expect(parser, LEFTBRACE, "Expected '{' to begin 'finally' block.");
        node->as.try_except_statement.finally_block = parse_block(parser);
    }

    return node;
}

static AstNode* parse_statement(Parser* parser) {
    if (match(parser, WAIT))      return parse_wait_statement(parser);
    if (match(parser, RAISE))     return parse_raise_statement(parser);
    if (match(parser, WHILE))     return parse_while_statement(parser);
    if (match(parser, BACKTICK))  return parse_break_statement(parser);
    if (match(parser, CARET))     return parse_continue_statement(parser);
    if (match(parser, RETURN))    return parse_return_statement(parser);
    if (match(parser, PRINT))     return parse_print_statement(parser);
    if (peek(parser)->type == TYPE && parser->tokens[parser->current + 1].type == LEFTBRACE) {
        return parse_type_declaration(parser);
    }
    if (peek(parser)->type == NAMESPACE &&
        parser->tokens[parser->current + 1].type == IDENTIFIER &&
        parser->tokens[parser->current + 2].type == LEFTBRACE) {
        consume(parser);
        return parse_namespace_declaration(parser);
    }
    if (peek(parser)->type == NAMESPACE &&
        parser->tokens[parser->current + 1].type == LEFTBRACE) {
        consume(parser);
        error_at_token(parser, peek(parser), "Cannot declare a nested global scope. Use a named namespace instead.");
        return NULL;
    }
    if (match(parser, QUESTIONMARK)) {
        Token keyword = parser->tokens[parser->current - 1];
        
        if (peek(parser)->type == LEFTBRACE) {
            return parse_try_except_statement(parser);
        } else {
            AstNode* condition = parse_expression(parser, 1);
            if (peek(parser)->type == LEFTBRACE) {
                return parse_if_statement_after_condition(parser, condition);
            } else {
                AstNode* node = create_node(parser, AST_ASSERT_STATEMENT);
                node->line = keyword.line;
                node->as.assert_statement.keyword = keyword;
                node->as.assert_statement.condition = condition;
                return node;
            }
        }
    }

    if (peek(parser)->type == IDENTIFIER) {
        GraveyardTokenType next_token = parser->tokens[parser->current + 1].type;
        
        if (next_token == FILEREAD) {
            consume(parser); consume(parser);
            return parse_fileread_statement(parser);
        }
        if (next_token == AT) {
            consume(parser); consume(parser);
            return parse_for_statement(parser);
        }
        if (next_token == SCAN) {
            consume(parser); consume(parser);
            return parse_scan_statement(parser);
        }
        if (next_token == PARAMETER || next_token == LEFTBRACE) {
            Token name = *consume(parser);
            return parse_function_declaration(parser, name);
        }
        if (is_compound_assignment(next_token)) {
            AstNode* assignment_node = parse_compound_assignment(parser);
            AstNode* stmt_node = create_node(parser, AST_EXPRESSION_STATEMENT);
            stmt_node->line = assignment_node->line;
            stmt_node->as.expression_statement.expression = assignment_node;
            return stmt_node;
        }
        if (next_token == INCREMENT || next_token == DECREMENT) {
            AstNode* assignment_node = parse_inc_dec_statement(parser);
            AstNode* stmt_node = create_node(parser, AST_EXPRESSION_STATEMENT);
            stmt_node->line = assignment_node->line;
            stmt_node->as.expression_statement.expression = assignment_node;
            return stmt_node;
        }
    }
    
    AstNode* expr = parse_expression(parser, 1);
    if (!expr) {
        return NULL;
    }

    if (expr->type == AST_ASSIGNMENT || expr->type == AST_CALL_EXPRESSION ||
       (expr->type == AST_BINARY_OP && expr->as.binary_op.operator.type == FILEWRITE)) {
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

        if (statement->type != AST_FUNCTION_DECLARATION &&
            statement->type != AST_IF_STATEMENT &&
            statement->type != AST_WHILE_STATEMENT &&
            statement->type != AST_FOR_STATEMENT &&
            statement->type != AST_NAMESPACE_DECLARATION &&
            statement->type != AST_TYPE_DECLARATION &&
            statement->type != AST_TRY_EXCEPT_STATEMENT) {
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

        case AST_SCAN_STATEMENT: {
            fprintf(file, "(SCAN_STATEMENT variable=\"%s\" line=%d\n",
                node->as.scan_statement.variable.lexeme,
                node->line);
            write_ast_node(file, node->as.scan_statement.prompt, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

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

        case AST_TIME_EXPRESSION: {
            fprintf(file, "(TIME_EXPRESSION line=%d)\n", node->line);
            break;
        }

        case AST_LITERAL: {
            Token literal_token = node->as.literal.value;
            switch (literal_token.type) {
                case TYPE:
                    fprintf(file, "(LITERAL_TYPE value=\"%s\" line=%d)\n", literal_token.lexeme, node->line);
                    break;
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

        case AST_ASSERT_STATEMENT: {
            fprintf(file, "(ASSERT_STATEMENT line=%d\n", node->line);
            write_ast_node(file, node->as.assert_statement.condition, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_WHILE_STATEMENT: {
            fprintf(file, "(WHILE_STATEMENT line=%d\n", node->line);
            write_ast_node(file, node->as.while_statement.condition, indent + 1);
            write_ast_node(file, node->as.while_statement.body, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_BREAK_STATEMENT: {
            fprintf(file, "(BREAK_STATEMENT line=%d)\n", node->line);
            break;
        }

        case AST_CONTINUE_STATEMENT: {
            fprintf(file, "(CONTINUE_STATEMENT line=%d)\n", node->line);
            break;
        }

        case AST_FOR_STATEMENT: {
            fprintf(file, "(FOR_STATEMENT iterator=\"%s\" range_count=%zu line=%d\n",
                node->as.for_statement.iterator.lexeme,
                node->as.for_statement.range_count,
                node->line);
            for (size_t i = 0; i < node->as.for_statement.range_count; i++) {
                write_ast_node(file, node->as.for_statement.range_expressions[i], indent + 1);
            }
            write_ast_node(file, node->as.for_statement.body, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_RAISE_STATEMENT: {
            fprintf(file, "(RAISE_STATEMENT line=%d\n", node->line);
            write_ast_node(file, node->as.raise_statement.error_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_NAMESPACE_DECLARATION: {
            fprintf(file, "(NAMESPACE_DECLARATION name=\"%s\" line=%d\n",
                node->as.namespace_declaration.name.lexeme,
                node->line);
            write_ast_node(file, node->as.namespace_declaration.body, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_NAMESPACE_ACCESS: {
            fprintf(file, "(NAMESPACE_ACCESS namespace=\"%s\" member=\"%s\" line=%d)\n",
                node->as.namespace_access.namespace_name.lexeme,
                node->as.namespace_access.member_name.lexeme,
                node->line);
            break;
        }

        case AST_FILEREAD_STATEMENT: {
            fprintf(file, "(FILEREAD_STATEMENT variable=\"%s\" line=%d\n",
                node->as.fileread_statement.variable.lexeme,
                node->line);
            write_ast_node(file, node->as.fileread_statement.path_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }

        case AST_TYPE_DECLARATION: {
            fprintf(file, "(TYPE_DECLARATION name=\"%s\" line=%d\n", node->as.type_declaration.name.lexeme, node->line);
            write_ast_node(file, node->as.type_declaration.body, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_MEMBER_ACCESS: {
            fprintf(file, "(MEMBER_ACCESS member=\"%s\" line=%d\n", node->as.member_access.member.lexeme, node->line);
            write_ast_node(file, node->as.member_access.object, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_THIS_EXPRESSION: {
            fprintf(file, "(THIS_EXPRESSION line=%d)\n", node->line);
            break;
        }
        case AST_EXECUTE_EXPRESSION: {
            fprintf(file, "(EXECUTE_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.execute_expression.command_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_CAT_CONSTANT_EXPRESSION: {
            fprintf(file, "(CAT_CONSTANT_EXPRESSION line=%d)\n", node->line);
            break;
        }
        case AST_WAIT_STATEMENT: {
            fprintf(file, "(WAIT_STATEMENT line=%d\n", node->line);
            write_ast_node(file, node->as.wait_statement.duration_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_RANDOM_EXPRESSION: {
            fprintf(file, "(RANDOM_EXPRESSION line=%d)\n", node->line);
            break;
        }
        case AST_GLOBAL_ACCESS: {
            fprintf(file, "(GLOBAL_ACCESS member=\"%s\" line=%d)\n",
                node->as.global_access.member_name.lexeme,
                node->line);
            break;
        }
        case AST_STATIC_ACCESS: {
            fprintf(file, "(STATIC_ACCESS type=\"%s\" member=\"%s\" line=%d)\n",
                node->as.static_access.type_name.lexeme,
                node->as.static_access.member_name.lexeme,
                node->line);
            break;
        }
        case AST_UID_EXPRESSION: {
            fprintf(file, "(UID_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.uid_expression.length_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_SLICE_EXPRESSION: {
            fprintf(file, "(SLICE_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.slice_expression.collection, indent + 1);
            if (node->as.slice_expression.start_expr) write_ast_node(file, node->as.slice_expression.start_expr, indent + 1);
            else { for (int i=0; i<indent+1; ++i) fprintf(file, "  "); fprintf(file, "(NULL_EXPR)\n"); }
            
            if (node->as.slice_expression.stop_expr) write_ast_node(file, node->as.slice_expression.stop_expr, indent + 1);
            else { for (int i=0; i<indent+1; ++i) fprintf(file, "  "); fprintf(file, "(NULL_EXPR)\n"); }

            if (node->as.slice_expression.step_expr) write_ast_node(file, node->as.slice_expression.step_expr, indent + 1);
            else { for (int i=0; i<indent+1; ++i) fprintf(file, "  "); fprintf(file, "(NULL_EXPR)\n"); }

            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_EVAL_EXPRESSION: {
            fprintf(file, "(EVAL_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.eval_expression.code_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_EXISTS_EXPRESSION: {
            fprintf(file, "(EXISTS_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.exists_expression.path_expr, indent + 1);
            for (int i = 0; i < indent; ++i) { fprintf(file, "  "); }
            fprintf(file, ")\n");
            break;
        }
        case AST_LISTDIR_EXPRESSION: {
            fprintf(file, "(LISTDIR_EXPRESSION line=%d\n", node->line);
            write_ast_node(file, node->as.listdir_expression.path_expr, indent + 1);
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
    if (strcmp(type_str, "ASSERT_STATEMENT") == 0) return AST_ASSERT_STATEMENT;
    if (strcmp(type_str, "WHILE_STATEMENT") == 0) return AST_WHILE_STATEMENT;
    if (strcmp(type_str, "BREAK_STATEMENT") == 0) return AST_BREAK_STATEMENT;
    if (strcmp(type_str, "CONTINUE_STATEMENT") == 0) return AST_CONTINUE_STATEMENT;
    if (strcmp(type_str, "FOR_STATEMENT") == 0) return AST_FOR_STATEMENT;
    if (strcmp(type_str, "SCAN_STATEMENT") == 0) return AST_SCAN_STATEMENT;
    if (strcmp(type_str, "RAISE_STATEMENT") == 0) return AST_RAISE_STATEMENT;
    if (strcmp(type_str, "TIME_EXPRESSION") == 0) return AST_TIME_EXPRESSION;
    if (strcmp(type_str, "NAMESPACE_DECLARATION") == 0) return AST_NAMESPACE_DECLARATION;
    if (strcmp(type_str, "NAMESPACE_ACCESS") == 0) return AST_NAMESPACE_ACCESS;
    if (strcmp(type_str, "FILEREAD_STATEMENT") == 0) return AST_FILEREAD_STATEMENT;
    if (strcmp(type_str, "TYPE_DECLARATION") == 0) return AST_TYPE_DECLARATION;
    if (strcmp(type_str, "MEMBER_ACCESS") == 0) return AST_MEMBER_ACCESS;
    if (strcmp(type_str, "THIS_EXPRESSION") == 0) return AST_THIS_EXPRESSION;
    if (strcmp(type_str, "LITERAL_TYPE") == 0) return AST_LITERAL;
    if (strcmp(type_str, "EXECUTE_EXPRESSION") == 0) return AST_EXECUTE_EXPRESSION;
    if (strcmp(type_str, "CAT_CONSTANT_EXPRESSION") == 0) return AST_CAT_CONSTANT_EXPRESSION;
    if (strcmp(type_str, "WAIT_STATEMENT") == 0) return AST_WAIT_STATEMENT;
    if (strcmp(type_str, "RANDOM_EXPRESSION") == 0) return AST_RANDOM_EXPRESSION;
    if (strcmp(type_str, "GLOBAL_ACCESS") == 0) return AST_GLOBAL_ACCESS;
    if (strcmp(type_str, "STATIC_ACCESS") == 0) return AST_STATIC_ACCESS;
    if (strcmp(type_str, "UID_EXPRESSION") == 0) return AST_UID_EXPRESSION;
    if (strcmp(type_str, "SLICE_EXPRESSION") == 0) return AST_SLICE_EXPRESSION;
    if (strcmp(type_str, "EVAL_EXPRESSION") == 0) return AST_EVAL_EXPRESSION;
    if (strcmp(type_str, "EXISTS_EXPRESSION") == 0) return AST_EXISTS_EXPRESSION;
    if (strcmp(type_str, "LISTDIR_EXPRESSION") == 0) return AST_LISTDIR_EXPRESSION;
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

static AstNode* parse_optional_expr(Lines* lines, int* idx, int indent, Parser* p) {
    if (*idx < lines->count && strstr(lines->lines[*idx], "(NULL_EXPR)")) {
        (*idx)++;
        return NULL;
    }
    return parse_node_recursive(lines, idx, indent, p);
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

            char* op_str = node->as.unary_op.operator.lexeme;
            size_t op_len = strlen(op_str);

            if (op_len == 2) {
                node->as.unary_op.operator.type = identify_two_char_token(op_str[0], op_str[1]);
            } else if (op_len == 1) {
                node->as.unary_op.operator.type = identify_single_char_token(op_str[0]);
            } else {
                node->as.unary_op.operator.type = UNKNOWN;
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
            } else if (strcmp(type_str, "LITERAL_TYPE") == 0) {
                node->as.literal.value.type = TYPE;
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

        case AST_ASSERT_STATEMENT: {
            node->as.assert_statement.condition = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_WHILE_STATEMENT: {
            node->as.while_statement.condition = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.while_statement.body = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_FOR_STATEMENT: {
            get_attribute_string(line, "iterator=", node->as.for_statement.iterator.lexeme, MAX_LEXEME_LEN);
            size_t range_count = get_attribute_int(line, "range_count=");
            node->as.for_statement.range_count = range_count;
            node->as.for_statement.range_expressions = malloc(range_count * sizeof(AstNode*));

            for (size_t i = 0; i < range_count; i++) {
                node->as.for_statement.range_expressions[i] = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            }
            node->as.for_statement.body = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_SCAN_STATEMENT: {
            get_attribute_string(line, "variable=", node->as.scan_statement.variable.lexeme, MAX_LEXEME_LEN);
            node->as.scan_statement.prompt = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_RAISE_STATEMENT: {
            node->as.raise_statement.error_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_NAMESPACE_DECLARATION: {
            get_attribute_string(line, "name=", node->as.namespace_declaration.name.lexeme, MAX_LEXEME_LEN);
            node->as.namespace_declaration.body = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }
        case AST_NAMESPACE_ACCESS: {
            get_attribute_string(line, "namespace=", node->as.namespace_access.namespace_name.lexeme, MAX_LEXEME_LEN);
            get_attribute_string(line, "member=", node->as.namespace_access.member_name.lexeme, MAX_LEXEME_LEN);
            break;
        }

        case AST_FILEREAD_STATEMENT: {
            get_attribute_string(line, "variable=", node->as.fileread_statement.variable.lexeme, MAX_LEXEME_LEN);
            node->as.fileread_statement.variable.type = IDENTIFIER;
            node->as.fileread_statement.path_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_TYPE_DECLARATION: {
            get_attribute_string(line, "name=", node->as.type_declaration.name.lexeme, MAX_LEXEME_LEN);
            node->as.type_declaration.body = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_MEMBER_ACCESS: {
            get_attribute_string(line, "member=", node->as.member_access.member.lexeme, MAX_LEXEME_LEN);
            node->as.member_access.object = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_EXECUTE_EXPRESSION: {
            node->as.execute_expression.command_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_WAIT_STATEMENT: {
            node->as.wait_statement.duration_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_GLOBAL_ACCESS: {
            get_attribute_string(line, "member=", node->as.global_access.member_name.lexeme, MAX_LEXEME_LEN);
            node->as.global_access.member_name.type = IDENTIFIER;
            break;
        }

        case AST_STATIC_ACCESS: {
            get_attribute_string(line, "type=", node->as.static_access.type_name.lexeme, MAX_LEXEME_LEN);
            get_attribute_string(line, "member=", node->as.static_access.member_name.lexeme, MAX_LEXEME_LEN);
            node->as.static_access.type_name.type = TYPE;
            node->as.static_access.member_name.type = IDENTIFIER;
            break;
        }

        case AST_UID_EXPRESSION: {
            node->as.uid_expression.length_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }
        
        case AST_SLICE_EXPRESSION: {
            node->as.slice_expression.collection = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            node->as.slice_expression.start_expr = parse_optional_expr(lines, current_line_idx, expected_indent + 1, parser);
            node->as.slice_expression.stop_expr = parse_optional_expr(lines, current_line_idx, expected_indent + 1, parser);
            node->as.slice_expression.step_expr = parse_optional_expr(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_EVAL_EXPRESSION: {
            node->as.eval_expression.code_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }
        
        case AST_EXISTS_EXPRESSION: {
            node->as.exists_expression.path_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_LISTDIR_EXPRESSION: {
            node->as.listdir_expression.path_expr = parse_node_recursive(lines, current_line_idx, expected_indent + 1, parser);
            break;
        }

        case AST_RANDOM_EXPRESSION:
        case AST_CAT_CONSTANT_EXPRESSION:
        case AST_THIS_EXPRESSION:
        case AST_TIME_EXPRESSION:
        case AST_BREAK_STATEMENT:
        case AST_CONTINUE_STATEMENT:
            break;

        default: break;
    }
    
    if (parser->had_error) { free_ast(node); return NULL; }
    
    bool is_block_node = false;
    switch(type) {
        case AST_PROGRAM:
        case AST_ASSIGNMENT:
        case AST_BINARY_OP:
        case AST_UNARY_OP:
        case AST_PRINT_STATEMENT:
        case AST_LOGICAL_OP:
        case AST_FORMATTED_STRING:
        case AST_ARRAY_LITERAL:
        case AST_SUBSCRIPT:
        case AST_HASHTABLE_LITERAL:
        case AST_EXPRESSION_STATEMENT:
        case AST_BLOCK:
        case AST_RETURN_STATEMENT:
        case AST_FUNCTION_DECLARATION:
        case AST_CALL_EXPRESSION:
        case AST_IF_STATEMENT:
        case AST_TERNARY_EXPRESSION:
        case AST_ASSERT_STATEMENT:
        case AST_WHILE_STATEMENT:
        case AST_FOR_STATEMENT:
        case AST_SCAN_STATEMENT:
        case AST_RAISE_STATEMENT:
        case AST_NAMESPACE_DECLARATION:
        case AST_FILEREAD_STATEMENT:
        case AST_TYPE_DECLARATION:
        case AST_MEMBER_ACCESS:
        case AST_EXECUTE_EXPRESSION:
        case AST_WAIT_STATEMENT:
        case AST_UID_EXPRESSION:
        case AST_SLICE_EXPRESSION:
        case AST_EVAL_EXPRESSION:
        case AST_EXISTS_EXPRESSION:
        case AST_LISTDIR_EXPRESSION:
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

//EXECUTE-------------------------------------------------------
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

static void dec_ref(GraveyardValue value);
static void free_value(GraveyardValue value);

void monolith_free(Monolith* monolith) {
    for (int i = 0; i < monolith->capacity; i++) {
        MonolithEntry* entry = &monolith->entries[i];
        if (entry->key != NULL) {
            dec_ref(entry->value);
            free(entry->key);
        }
    }
    free(monolith->entries);
    monolith->entries = NULL;
    monolith->count = 0;
    monolith->capacity = 0;
}

static void inc_ref(GraveyardValue value) {
    switch (value.type) {
        case VAL_STRING:     if (value.as.string) value.as.string->ref_count++;         break;
        case VAL_ARRAY:      if (value.as.array) value.as.array->ref_count++;           break;
        case VAL_HASHTABLE:  if (value.as.hashtable) value.as.hashtable->ref_count++;   break;
        case VAL_FUNCTION:   if (value.as.function) value.as.function->ref_count++;     break;
        case VAL_TYPE:       if (value.as.type) value.as.type->ref_count++;             break;
        case VAL_INSTANCE:   if (value.as.instance) value.as.instance->ref_count++;     break;
        case VAL_BOUND_METHOD: if (value.as.bound_method) value.as.bound_method->ref_count++; break;
        default:
            break;
    }
}

static GraveyardValue create_type_value_from_ptr(GraveyardType* type) {
    GraveyardValue val;
    val.type = VAL_TYPE;
    val.as.type = type;
    return val;
}

static void free_value(GraveyardValue value) {
    switch (value.type) {
        case VAL_STRING: {
            GraveyardString* string = value.as.string;
            free(string->chars);
            free(string);
            break;
        }
        case VAL_ARRAY: {
            GraveyardArray* array = value.as.array;
            for (size_t i = 0; i < array->count; i++) {
                dec_ref(array->values[i]);
            }
            free(array->values);
            free(array);
            break;
        }
        case VAL_HASHTABLE: {
            GraveyardHashtable* ht = value.as.hashtable;
            for (int i = 0; i < ht->capacity; i++) {
                if (ht->entries[i].is_in_use) {
                    dec_ref(ht->entries[i].key);
                    dec_ref(ht->entries[i].value);
                }
            }
            free(ht->entries);
            free(ht);
            break;
        }
        case VAL_FUNCTION: {
            GraveyardFunction* func = value.as.function;
            free(func->name->chars);
            free(func->name);
            free(func);
            break;
        }
        case VAL_TYPE: {
            GraveyardType* type = value.as.type;
            free(type->name->chars);
            free(type->name);
            monolith_free(&type->fields);
            monolith_free(&type->methods);
            free(type);
            break;
        }
        case VAL_INSTANCE: {
            GraveyardInstance* instance = value.as.instance;
            dec_ref(create_type_value_from_ptr(instance->type));
            monolith_free(&instance->fields);
            free(instance);
            break;
        }
        case VAL_BOUND_METHOD: {
            GraveyardBoundMethod* bound = value.as.bound_method;
            dec_ref(bound->receiver);
            dec_ref(bound->function);
            free(bound);
            break;
        }
        default:
            break; 
    }
}

static void dec_ref(GraveyardValue value) {
    switch (value.type) {
        case VAL_STRING:
            if (value.as.string && --value.as.string->ref_count == 0) free_value(value);
            break;
        case VAL_ARRAY:
            if (value.as.array && --value.as.array->ref_count == 0) free_value(value);
            break;
        case VAL_HASHTABLE:
            if (value.as.hashtable && --value.as.hashtable->ref_count == 0) free_value(value);
            break;
        case VAL_FUNCTION:
            if (value.as.function && --value.as.function->ref_count == 0) free_value(value);
            break;
        case VAL_TYPE:
             if (value.as.type && --value.as.type->ref_count == 0) free_value(value);
            break;
        case VAL_INSTANCE:
             if (value.as.instance && --value.as.instance->ref_count == 0) free_value(value);
            break;
        case VAL_BOUND_METHOD:
             if (value.as.bound_method && --value.as.bound_method->ref_count == 0) free_value(value);
            break;
        default:
            break;
    }
}

static void runtime_error(Graveyard* gy, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(gy->error_message, sizeof(gy->error_message), format, args);
    va_end(args);
    
    gy->error_line = line;
    gy->had_runtime_error = true;
}

static char* generate_random_hex_string(int length) {
    if (length <= 0) return NULL;

    int num_bytes = (length + 1) / 2;
    unsigned char* random_bytes = malloc(num_bytes);
    if (!random_bytes) return NULL;

#ifdef _WIN32
    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        free(random_bytes);
        return NULL;
    }
    if (!CryptGenRandom(hCryptProv, num_bytes, random_bytes)) {
        CryptReleaseContext(hCryptProv, 0);
        free(random_bytes);
        return NULL;
    }
    CryptReleaseContext(hCryptProv, 0);
#else
    FILE* urandom = fopen("/dev/urandom", "r");
    if (!urandom) {
        free(random_bytes);
        return NULL;
    }
    fread(random_bytes, 1, num_bytes, urandom);
    fclose(urandom);
#endif

    char* hex_string = malloc(length + 1);
    if (!hex_string) {
        free(random_bytes);
        return NULL;
    }

    for (int i = 0; i < num_bytes; i++) {
        sprintf(hex_string + (i * 2), "%02x", random_bytes[i]);
    }
    hex_string[length] = '\0';
    
    free(random_bytes);
    return hex_string;
}

static Environment* get_global_environment(Graveyard* gy) {
    Environment* env = gy->environment;
    while (env->enclosing != NULL) {
        env = env->enclosing;
    }
    return env;
}

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
    ht->ref_count = 1;
    ht->entries = malloc(ht->capacity * sizeof(HashtableEntry));
    for (int i = 0; i < ht->capacity; i++) {
        ht->entries[i].is_in_use = false;
        ht->entries[i].key.type = VAL_NULL;
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
        inc_ref(key);
        entry->key = key;
    } else {
        dec_ref(entry->value);
    }
    
    inc_ref(value);
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

    func->ref_count = 1;
    func->arity = node->as.function_declaration.param_count;
    func->body = node->as.function_declaration.body;
    func->params = node->as.function_declaration.params;
    
    func->closure = gy->environment;

    size_t len = strlen(node->as.function_declaration.name.lexeme);
    GraveyardString* name_str = malloc(sizeof(GraveyardString));
    name_str->chars = malloc(len + 1);
    memcpy(name_str->chars, node->as.function_declaration.name.lexeme, len + 1);
    name_str->length = len;
    name_str->ref_count = 0;
    func->name = name_str;

    val.as.function = func;
    return val;
}

static GraveyardValue execute_node(Graveyard* gy, AstNode* node);

static bool calculate_slice_bounds(long length, AstNode* start_expr, AstNode* stop_expr, AstNode* step_expr,
                                   long* out_start, long* out_stop, long* out_step, Graveyard* gy) {

    *out_step = 1;
    if (step_expr) {
        GraveyardValue step_val = execute_node(gy, step_expr);
        if (step_val.type != VAL_NUMBER) {
            return false; 
        }
        *out_step = (long)step_val.as.number;
    }
    if (*out_step == 0) {
        return false;
    }
    *out_start = (*out_step > 0) ? 0 : length - 1;
    if (start_expr) {
        GraveyardValue start_val = execute_node(gy, start_expr);
        if (start_val.type == VAL_NUMBER) {
            *out_start = (long)start_val.as.number;
            if (*out_start < 0) *out_start += length;
        }
    }

    *out_stop = (*out_step > 0) ? length : -1;
    if (stop_expr) {
        GraveyardValue stop_val = execute_node(gy, stop_expr);
        if (stop_val.type == VAL_NUMBER) {
            *out_stop = (long)stop_val.as.number;
            if (*out_stop < 0) *out_stop += length;
        }
    }

    if (*out_start < 0) *out_start = 0;
    if (*out_start > length) *out_start = length;
    if (*out_stop > length) *out_stop = length;

    return true;
}

static GraveyardValue execute_block(Graveyard* gy, AstNode* block_node, Environment* environment) {
    Environment* previous = gy->environment;
    gy->environment = environment;
    GraveyardValue last_val = create_null_value();

    for (size_t i = 0; i < block_node->as.block.count; i++) {
        last_val = execute_node(gy, block_node->as.block.statements[i]);
        if (gy->is_returning || gy->encountered_break || gy->encountered_continue) {
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

bool monolith_set(Monolith* monolith, const char* key, GraveyardValue value) {
    if (monolith->count + 1 > monolith->capacity * 0.75) {
        int new_capacity = monolith->capacity < 8 ? 8 : monolith->capacity * 2;
        monolith_resize(monolith, new_capacity);
    }

    MonolithEntry* entry = find_entry(monolith->entries, monolith->capacity, key);
    bool is_new_key = entry->key == NULL;

    if (!is_new_key) {
        dec_ref(entry->value);
    }
    
    inc_ref(value);
    entry->value = value;

    if (is_new_key) {
        monolith->count++;
        entry->key = strdup(key);
        if (entry->key == NULL) {
            perror("monolith_set: strdup failed");
            return false;
        }
    }
    
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

static GraveyardValue create_type_value(GraveyardString* name) {
    GraveyardValue val;
    val.type = VAL_TYPE;
    GraveyardType* type = malloc(sizeof(GraveyardType));
    type->ref_count = 1;
    type->name = name;
    monolith_init(&type->fields);
    monolith_init(&type->methods);
    val.as.type = type;
    return val;
}

static GraveyardValue create_instance_value(GraveyardValue type_value) {
    GraveyardValue val;
    val.type = VAL_INSTANCE;
    GraveyardInstance* instance = malloc(sizeof(GraveyardInstance));
    instance->ref_count = 1;
    instance->type = type_value.as.type;
    
    inc_ref(type_value);

    monolith_init(&instance->fields);
    val.as.instance = instance;
    return val;
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
    string_obj->ref_count = 1;

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
    array_obj->ref_count = 1;

    val.as.array = array_obj;
    return val;
}

static void value_to_string(GraveyardValue value, char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return;

    switch (value.type) {
        case VAL_NULL:
            strncpy(buffer, "null", buffer_size - 1);
            break;
        case VAL_BOOL:
            strncpy(buffer, value.as.boolean ? "true" : "false", buffer_size - 1);
            break;
        case VAL_NUMBER:
            snprintf(buffer, buffer_size, "%g", value.as.number);
            break;
        case VAL_STRING:
            snprintf(buffer, buffer_size, "\"%s\"", value.as.string->chars);
            break;
        case VAL_FUNCTION:
            snprintf(buffer, buffer_size, "%s", value.as.function->name->chars);
            break;

        case VAL_ARRAY: {
            size_t capacity = 128;
            char* result = malloc(capacity);
            if (!result) { buffer[0] = '\0'; return; }
            strcpy(result, "[");
            size_t len = 1;

            GraveyardArray* arr = value.as.array;
            for (size_t i = 0; i < arr->count; i++) {
                char element_str[256];
                value_to_string(arr->values[i], element_str, sizeof(element_str));
                
                size_t part_len = strlen(element_str);
                size_t comma_len = (i < arr->count - 1) ? 2 : 0;

                if (len + part_len + comma_len + 2 > capacity) {
                    capacity = (len + part_len + comma_len + 2) * 2;
                    char* temp = realloc(result, capacity);
                    if (!temp) { free(result); buffer[0] = '\0'; return; }
                    result = temp;
                }
                
                strcat(result, element_str);
                len += part_len;
                if (comma_len > 0) {
                    strcat(result, ", ");
                    len += comma_len;
                }
            }
            strcat(result, "]");
            strncpy(buffer, result, buffer_size - 1);
            free(result);
            break;
        }
        case VAL_HASHTABLE: {
            size_t capacity = 128;
            char* result = malloc(capacity);
            if (!result) { buffer[0] = '\0'; return; }
            strcpy(result, "{");
            size_t len = 1;

            GraveyardHashtable* ht = value.as.hashtable;
            int printed = 0;
            for (int i = 0; i < ht->capacity; i++) {
                if (ht->entries[i].is_in_use) {
                    char key_str[256], val_str[256];
                    value_to_string(ht->entries[i].key, key_str, sizeof(key_str));
                    value_to_string(ht->entries[i].value, val_str, sizeof(val_str));

                    size_t part_len = strlen(key_str) + strlen(val_str) + 2;
                    size_t comma_len = (printed > 0) ? 2 : 0;

                    if (len + part_len + comma_len + 2 > capacity) {
                        capacity = (len + part_len + comma_len + 2) * 2;
                        char* temp = realloc(result, capacity);
                        if (!temp) { free(result); buffer[0] = '\0'; return; }
                        result = temp;
                    }

                    if (comma_len > 0) strcat(result, ", ");
                    strcat(result, key_str);
                    strcat(result, ": ");
                    strcat(result, val_str);
                    len += part_len + comma_len;
                    printed++;
                }
            }
            strcat(result, "}");
            strncpy(buffer, result, buffer_size - 1);
            free(result);
            break;
        }
        default:
            strncpy(buffer, "(unknown)", buffer_size - 1);
            break;
    }
    buffer[buffer_size - 1] = '\0';
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
    if (array->count >= array->capacity) {
        size_t new_capacity = (array->capacity == 0) ? 8 : array->capacity * 2;
        GraveyardValue* temp = realloc(array->values, new_capacity * sizeof(GraveyardValue));
        if (!temp) {
            perror("array_append: realloc failed");
            exit(1);
        }
        array->values = temp;
        array->capacity = new_capacity;
    }
    
    inc_ref(value);
    array->values[array->count++] = value;
}

void monolith_print(Monolith* monolith, int indent);

void print_value(GraveyardValue value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(value.as.boolean ? "$" : "%%");
            break;
        case VAL_NULL:
            printf("|");
            break;
        case VAL_NUMBER: {
            double num = value.as.number;
            if (fmod(num, 1.0) == 0) {
                printf("%.0f", num);
            } else {
                char buffer[64];
                snprintf(buffer, sizeof(buffer), "%.15f", num);
                char* end = buffer + strlen(buffer) - 1;
                while (end > buffer && *end == '0') { *end = '\0'; end--; }
                if (end > buffer && *end == '.') { *end = '\0'; }
                printf("%s", buffer);
            }
            break;
        }
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
        case VAL_TYPE:
            printf("<type: %s>", value.as.type->name->chars);
            break;
        case VAL_INSTANCE:
            printf("<instance of %s> {\n", value.as.instance->type->name->chars);
            monolith_print(&value.as.instance->fields, 2);
            printf("  }");
            break;
        case VAL_FUNCTION:
            printf("<function: %s>", value.as.function->name->chars);
            break;
        case VAL_BOUND_METHOD:
            printf("<method: %s bound to instance>", value.as.bound_method->function.as.function->name->chars);
            break;
        case VAL_ENVIRONMENT:
             printf("<namespace>");
             break;
    }
}

void monolith_print(Monolith* monolith, int indent) {
    char indent_str[32] = "";
    for (int i = 0; i < indent; i++) {
        strcat(indent_str, "  ");
    }

    if (monolith->count == 0) {
        printf("%s(empty)\n", indent_str);
        return;
    }

    for (int i = 0; i < monolith->capacity; i++) {
        MonolithEntry* entry = &monolith->entries[i];
        if (entry->key != NULL) {
            printf("%s%s = ", indent_str, entry->key);
            print_value(entry->value);
            printf("\n");
        }
    }
}

void print_environment_recursive(Environment* env, int depth) {
    if (env == NULL) return;

    printf("\n--- Scope Level %d ", depth);
    if (depth == 0) {
        printf("(Global) ---\n");
    } else {
        printf("---\n");
    }
    
    monolith_print(&env->values, 1);
    
    print_environment_recursive(env->enclosing, depth + 1);
}

void graveyard_debug_print(Graveyard* gy) {
    printf("========================================\n");
    printf("        GRAVEYARD DEBUG DUMP\n");
    printf("========================================\n");
    
    printf("\n--- Defined Namespaces ---\n");
    monolith_print(&gy->namespaces, 1);
    
    Environment* global_env = get_global_environment(gy);
    printf("\n--- Defined Types ---\n");
    bool has_types = false;
    for (int i = 0; i < global_env->values.capacity; i++) {
        MonolithEntry* entry = &global_env->values.entries[i];
        if (entry->key != NULL && entry->value.type == VAL_TYPE) {
            printf("  %s\n", entry->key);
            has_types = true;
        }
    }
    if (!has_types) printf("  (none)\n");

    printf("\n--- Final Environment Chain ---\n");
    print_environment_recursive(gy->environment, 0);
    
    printf("\n========================================\n");
}

void graveyard_free(Graveyard *gy) {
    if (!gy) return;
    free(gy->source_code);
    free(gy->tokens);
    free_ast(gy->ast_root);
    
    dec_ref(gy->arguments);
    dec_ref(gy->last_executed_value);
    dec_ref(gy->return_value);

    Environment* env = gy->environment;
    while (env != NULL) {
        Environment* next = env->enclosing;
        monolith_free(&env->values);
        free(env);
        env = next;
    }
    
    for (int i = 0; i < gy->namespaces.capacity; i++) {
        MonolithEntry* entry = &gy->namespaces.entries[i];
        if (entry->key != NULL) {
            Environment* ns_env = entry->value.as.environment;
            monolith_free(&ns_env->values);
            free(ns_env);
        }
    }
    monolith_free(&gy->namespaces);
    
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
    monolith_init(&gy->namespaces);
    gy->is_returning = false;
    gy->return_value = create_null_value();
    gy->encountered_break = false;
    gy->encountered_continue = false;
    gy->had_runtime_error = false;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand((unsigned int)ts.tv_sec ^ (unsigned int)ts.tv_nsec);
    return gy;
}

bool execute(Graveyard *gy);

static GraveyardValue execute_node(Graveyard* gy, AstNode* node) {
    switch (node->type) {
        case AST_PROGRAM: {
            GraveyardValue last_value = create_null_value();
            for (size_t i = 0; i < node->as.program.count; i++) {
                last_value = execute_node(gy, node->as.program.statements[i]);
                if (gy->had_runtime_error) {
                    break;
                }
            }
            return last_value;
        }

        case AST_PRINT_STATEMENT: {
            for (size_t i = 0; i < node->as.print_stmt.count; i++) {
                GraveyardValue value = execute_node(gy, node->as.print_stmt.expressions[i]);
                if (gy->had_runtime_error) {
                    dec_ref(value);
                    return create_null_value();
                }
                print_value(value);
                
                dec_ref(value);

                if (i < node->as.print_stmt.count - 1) {
                    printf(" ");
                }
            }
            if (!gy->had_runtime_error) {
                printf("\n");
            }
            return create_null_value();
        }

        case AST_SCAN_STATEMENT: {
            GraveyardValue prompt = execute_node(gy, node->as.scan_statement.prompt);
            print_value(prompt);
            fflush(stdout);

            char input_buffer[1024];
            if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
                input_buffer[strcspn(input_buffer, "\n")] = 0;

                GraveyardValue input_val = create_string_value(input_buffer);
                const char* var_name = node->as.scan_statement.variable.lexeme;

                environment_define(gy->environment, var_name, input_val);
            }
            return create_null_value();
        }

        case AST_LITERAL: {
            Token literal_token = node->as.literal.value;
            switch (literal_token.type) {
                case TYPE: {
                    char type_name_buffer[MAX_LEXEME_LEN];
                    const char* lexeme = literal_token.lexeme;
                    size_t len = strlen(lexeme);

                    if (len > 2) {
                        size_t type_name_len = len - 2;
                        strncpy(type_name_buffer, lexeme + 1, type_name_len);
                        type_name_buffer[type_name_len] = '\0';
                    } else {
                        type_name_buffer[0] = '\0';
                    }

                    GraveyardValue type_val;
                    if (!environment_get(gy->environment, type_name_buffer, &type_val)) {
                        runtime_error(gy, node->line, "Type <%s> is not defined", type_name_buffer);
                        return create_null_value();
                    }
                    if (type_val.type != VAL_TYPE) {
                        runtime_error(gy, node->line, "<%s> is not a type", type_name_buffer);
                        return create_null_value();
                    }
                    
                    GraveyardValue instance_val = create_instance_value(type_val);
                    
                    GraveyardType* type = type_val.as.type;
                    GraveyardInstance* instance = instance_val.as.instance;
                    for (int i = 0; i < type->fields.capacity; i++) {
                        MonolithEntry* entry = &type->fields.entries[i];
                        if (entry->key != NULL) {
                            monolith_set(&instance->fields, entry->key, entry->value);
                        }
                    }
                    
                    return instance_val;
                }
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
                runtime_error(gy, node->line, "Only arrays are subscriptable");
                return create_null_value();
            }

            GraveyardValue index_val = execute_node(gy, node->as.subscript.index);
            if (index_val.type != VAL_NUMBER) {
                runtime_error(gy, node->line, "Array index must be an integer");
                return create_null_value();
            }
            
            double raw_index = index_val.as.number;
            if (raw_index < 0 || fmod(raw_index, 1.0) != 0) {
                runtime_error(gy, node->line, "Array index must be a non-negative integer");
                return create_null_value();
            }
            
            int index = (int)raw_index;
            GraveyardArray* array = array_val.as.array;

            if (index >= array->count) {
                runtime_error(gy, node->line, "Array index out of bounds (index %d is beyond array of size %zu)", index, array->count);
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
                    runtime_error(gy, node->line, "Invalid type used as a hashtable key");
                    free(ht->entries);
                    free(ht);
                    return create_null_value();
                }
                
                if (key.type == VAL_NUMBER && fmod(key.as.number, 1.0) != 0) {
                    runtime_error(gy, node->line, "Cannot use float as hashtable key");
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

            runtime_error(gy, node->line, "Undefined variable '%s'", node->as.identifier.name.lexeme);
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
                environment_define(gy->environment, name, value_to_assign);
                return value_to_assign;

            } else if (target_node->type == AST_SUBSCRIPT) {
                AstNode* array_node = target_node->as.subscript.array;
                AstNode* index_node = target_node->as.subscript.index;

                GraveyardValue array_val = execute_node(gy, array_node);
                if (array_val.type != VAL_ARRAY) {
                    runtime_error(gy, target_node->line, "Cannot assign to subscript '[]' of a non-array type");
                    return create_null_value();
                }

                GraveyardValue index_val = execute_node(gy, index_node);
                if (index_val.type != VAL_NUMBER) {
                    runtime_error(gy, index_node->line, "Array index must be a number");
                    return create_null_value();
                }

                double raw_index = index_val.as.number;
                if (raw_index < 0 || fmod(raw_index, 1.0) != 0) {
                    runtime_error(gy, index_node->line, "Array index must be a non-negative integer");
                    return create_null_value();
                }
                
                int index = (int)raw_index;
                GraveyardArray* array = array_val.as.array;

                if (index >= array->count) {
                    runtime_error(gy, target_node->line, "Array index out of bounds. Cannot assign to index %d in an array of size %zu", index, array->count);
                    return create_null_value();
                }

                array->values[index] = value_to_assign;
                return value_to_assign;
            } else if (target_node->type == AST_BINARY_OP && target_node->as.binary_op.operator.type == REFERENCE) {
                AstNode* ht_node = target_node->as.binary_op.left;
                AstNode* key_node = target_node->as.binary_op.right;

                GraveyardValue ht_val = execute_node(gy, ht_node);
                if (ht_val.type != VAL_HASHTABLE) {
                    runtime_error(gy, ht_node->line, "Cannot assign to key of a non-hashtable type");
                    return create_null_value();
                }

                GraveyardValue key_val = execute_node(gy, key_node);

                if (key_val.type != VAL_STRING && key_val.type != VAL_NUMBER &&
                    key_val.type != VAL_BOOL && key_val.type != VAL_NULL) {
                    runtime_error(gy, key_node->line, "Invalid type used as a hashtable key");
                    return create_null_value();
                }
                if (key_val.type == VAL_NUMBER && fmod(key_val.as.number, 1.0) != 0) {
                    runtime_error(gy, key_node->line, "Float cannot be used as a hashtable key");
                    return create_null_value();
                }

                hashtable_set(ht_val.as.hashtable, key_val, value_to_assign);
                return value_to_assign;
            } else if (target_node->type == AST_MEMBER_ACCESS) {
                GraveyardValue object = execute_node(gy, target_node->as.member_access.object);
                if (object.type != VAL_INSTANCE) {
                    runtime_error(gy, target_node->line, "Can only assign to members of an instance");
                    return create_null_value();
                }
                
                monolith_set(&object.as.instance->fields, target_node->as.member_access.member.lexeme, value_to_assign);
                return value_to_assign;
            } else if (target_node->type == AST_GLOBAL_ACCESS) {
                const char* member_name = target_node->as.global_access.member_name.lexeme;
                Environment* global_env = get_global_environment(gy);
                
                environment_define(global_env, member_name, value_to_assign);
                return value_to_assign;
            } else if (target_node->type == AST_STATIC_ACCESS) {
                char* name_str = target_node->as.static_access.type_name.lexeme;
                name_str[strlen(name_str) - 1] = '\0';
                const char* type_name = name_str + 1;
                const char* member_name = target_node->as.static_access.member_name.lexeme;

                GraveyardValue type_val;
                if (!environment_get(gy->environment, type_name, &type_val) || type_val.type != VAL_TYPE) {
                    runtime_error(gy, target_node->line, "Type <%s> is not defined", type_name);
                    return create_null_value();
                }

                monolith_set(&type_val.as.type->fields, member_name, value_to_assign);
                return value_to_assign;
            }
            
            return create_null_value();
        }
        
        case AST_LOGICAL_OP: {
            GraveyardValue left = execute_node(gy, node->as.logical_op.left);
            GraveyardTokenType op_type = node->as.logical_op.operator.type;

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
            GraveyardValue result = create_null_value();

            switch (node->as.unary_op.operator.type) {
                case MINUS:
                    if (right.type != VAL_NUMBER) {
                        runtime_error(gy, node->line, "Operand for negation must be a number");
                    } else {
                        result = create_number_value(-right.as.number);
                    }
                    break;

                case NOT:
                    result = create_bool_value(is_value_falsy(right));
                    break;

                case TYPEOF: {
                    switch (right.type) {
                        case VAL_BOOL:         result = create_string_value("boolean"); break;
                        case VAL_NULL:         result = create_string_value("null"); break;
                        case VAL_STRING:       result = create_string_value("string"); break;
                        case VAL_ARRAY:        result = create_string_value("array"); break;
                        case VAL_HASHTABLE:    result = create_string_value("hashtable"); break;
                        case VAL_FUNCTION:
                        case VAL_BOUND_METHOD: result = create_string_value("function"); break;
                        case VAL_TYPE:         result = create_string_value("type"); break;
                        case VAL_INSTANCE:     result = create_string_value(right.as.instance->type->name->chars); break;
                        case VAL_NUMBER:
                            if (fmod(right.as.number, 1.0) == 0) {
                                result = create_string_value("integer");
                            } else {
                                result = create_string_value("float");
                            }
                            break;
                        default:
                            result = create_string_value("unknown"); break;
                    }
                    break;
                }

                case CASTBOOLEAN: {
                    result = create_bool_value(!is_value_falsy(right));
                    break;
                }

                case CASTINTEGER: {
                    switch (right.type) {
                        case VAL_NUMBER: result = create_number_value((int)right.as.number); break;
                        case VAL_BOOL:   result = create_number_value(right.as.boolean ? 1 : 0); break;
                        case VAL_NULL:   result = create_number_value(0); break;
                        case VAL_STRING: {
                            char* end;
                            long val = strtol(right.as.string->chars, &end, 10);
                            if (*end != '\0') {
                                runtime_error(gy, node->line, "Cannot cast non-numeric string to integer");
                            } else {
                                result = create_number_value(val);
                            }
                            break;
                        }
                        default:
                            runtime_error(gy, node->line, "Cannot cast this type to integer");
                            break;
                    }
                    break;
                }

                case CASTFLOAT: {
                    switch (right.type) {
                        case VAL_NUMBER: result = create_number_value((double)right.as.number); break;
                        case VAL_BOOL:   result = create_number_value(right.as.boolean ? 1.0 : 0.0); break;
                        case VAL_NULL:   result = create_number_value(0.0); break;
                        case VAL_STRING: {
                            char* end;
                            double val = strtod(right.as.string->chars, &end);
                            if (*end != '\0') {
                                runtime_error(gy, node->line, "Cannot cast non-numeric string to float");
                            } else {
                                result = create_number_value(val);
                            }
                            break;
                        }
                        default:
                            runtime_error(gy, node->line, "Cannot cast this type to float");
                            break;
                    }
                    break;
                }
                
                case CASTSTRING: {
                    char buffer[1024];
                    value_to_string(right, buffer, sizeof(buffer));
                    result = create_string_value(buffer);
                    break;
                }

                case CASTARRAY: {
                    switch (right.type) {
                        case VAL_ARRAY:       inc_ref(right); result = right; break;
                        case VAL_NULL:        result = create_array_value(); break;
                        case VAL_HASHTABLE: {
                            GraveyardValue arr_val = create_array_value();
                            GraveyardHashtable* ht = right.as.hashtable;
                            for (int i = 0; i < ht->capacity; i++) {
                                if (ht->entries[i].is_in_use) {
                                    array_append(arr_val.as.array, ht->entries[i].value);
                                }
                            }
                            result = arr_val;
                            break;
                        }
                        case VAL_STRING: {
                            GraveyardValue arr_val = create_array_value();
                            GraveyardString* str = right.as.string;
                            for (size_t i = 0; i < str->length; i++) {
                                char char_buf[2] = { str->chars[i], '\0' };
                                array_append(arr_val.as.array, create_string_value(char_buf));
                            }
                            result = arr_val;
                            break;
                        }
                        default: {
                            GraveyardValue arr_val = create_array_value();
                            array_append(arr_val.as.array, right);
                            result = arr_val;
                            break;
                        }
                    }
                    break;
                }

                case CASTHASHTABLE: {
                    switch (right.type) {
                        case VAL_HASHTABLE:
                            inc_ref(right);
                            result = right;
                            break;
                        case VAL_NULL:
                            result = create_hashtable_value();
                            break;
                        case VAL_ARRAY: {
                            GraveyardValue ht_val = create_hashtable_value();
                            GraveyardArray* arr = right.as.array;
                            bool cast_error = false;

                            for (size_t i = 0; i < arr->count; i++) {
                                GraveyardValue key = arr->values[i];
                                
                                bool is_valid_key = (key.type == VAL_BOOL || key.type == VAL_NULL || key.type == VAL_STRING ||
                                                    (key.type == VAL_NUMBER && fmod(key.as.number, 1.0) == 0));

                                if (!is_valid_key) {
                                    runtime_error(gy, node->line, "Array contains an invalid type for a hashtable key");
                                    cast_error = true;
                                    break;
                                }

                                if (hashtable_find_entry(ht_val.as.hashtable->entries, ht_val.as.hashtable->capacity, key)->is_in_use) {
                                    runtime_error(gy, node->line, "Duplicate key found when casting array to hashtable");
                                    cast_error = true;
                                    break;
                                }
                                hashtable_set(ht_val.as.hashtable, key, create_null_value());
                            }

                            if (cast_error) {
                                dec_ref(ht_val);
                            } else {
                                result = ht_val;
                            }
                            break;
                        }
                        default: {
                            GraveyardValue key = right;
                            bool is_valid_key = (key.type == VAL_BOOL || key.type == VAL_NULL || key.type == VAL_STRING ||
                                                (key.type == VAL_NUMBER && fmod(key.as.number, 1.0) == 0));

                            if (!is_valid_key) {
                                runtime_error(gy, node->line, "Invalid type used as a hashtable key");
                            } else {
                                GraveyardValue ht_val = create_hashtable_value();
                                hashtable_set(ht_val.as.hashtable, key, create_null_value());
                                result = ht_val;
                            }
                            break;
                        }
                    }
                    break;
                }

                case ASTERISK: {
                    switch (right.type) {
                        case VAL_STRING:    result = create_number_value(right.as.string->length); break;
                        case VAL_ARRAY:     result = create_number_value(right.as.array->count); break;
                        case VAL_HASHTABLE: result = create_number_value(right.as.hashtable->count); break;
                        case VAL_NUMBER:    result = create_number_value(trunc(right.as.number)); break;
                        case VAL_BOOL:      result = create_number_value(right.as.boolean ? 1 : 0); break;
                        case VAL_NULL:      result = create_number_value(0); break;
                        default:
                            runtime_error(gy, node->line, "This type does not have a length");
                            break;
                    }
                    break;
                }

                case CARET: {
                    if (right.type != VAL_HASHTABLE) {
                        runtime_error(gy, node->line, "The keys-of operator (^) can only be used on a hashtable");
                    } else {
                        GraveyardValue keys_array = create_array_value();
                        GraveyardHashtable* ht = right.as.hashtable;
                        for (int i = 0; i < ht->capacity; i++) {
                            if (ht->entries[i].is_in_use) {
                                array_append(keys_array.as.array, ht->entries[i].key);
                            }
                        }
                        result = keys_array;
                    }
                    break;
                }

                case BACKTICK: {
                    if (right.type != VAL_HASHTABLE) {
                        runtime_error(gy, node->line, "The values-of operator (`) can only be used on a hashtable");
                    } else {
                        GraveyardValue values_array = create_array_value();
                        GraveyardHashtable* ht = right.as.hashtable;
                        for (int i = 0; i < ht->capacity; i++) {
                            if (ht->entries[i].is_in_use) {
                                array_append(values_array.as.array, ht->entries[i].value);
                            }
                        }
                        result = values_array;
                    }
                    break;
                }

                default:
                    runtime_error(gy, node->line, "Unknown unary operator");
                    break;
            }

            dec_ref(right);
            
            return result;
        }

        case AST_BINARY_OP: {
            GraveyardTokenType op_type = node->as.binary_op.operator.type;

            if (op_type == NULLCOALESCE) {
                GraveyardValue left = execute_node(gy, node->as.binary_op.left);
                if (left.type != VAL_NULL) {
                    return left;
                }
                return execute_node(gy, node->as.binary_op.right);
            }

            GraveyardValue left = execute_node(gy, node->as.binary_op.left);
            GraveyardValue right = execute_node(gy, node->as.binary_op.right);
            GraveyardValue result = create_null_value();

            if (op_type == REFERENCE) {
                if (left.type != VAL_HASHTABLE) {
                    runtime_error(gy, node->line, "The '#' operator can only be used on a hashtable");
                } else {
                    GraveyardHashtable* ht = left.as.hashtable;
                    HashtableEntry* entry = hashtable_find_entry(ht->entries, ht->capacity, right);
                    if (entry->is_in_use) {
                        inc_ref(entry->value);
                        result = entry->value;
                    }
                }
            } else if (op_type == FILEWRITE) {
                if (left.type != VAL_STRING) {
                    runtime_error(gy, node->line, "Content for file write operation must be a string");
                } else if (right.type != VAL_STRING) {
                    runtime_error(gy, node->line, "File path for write operation must be a string");
                } else {
                    FILE* file = fopen(right.as.string->chars, "w");
                    if (!file) {
                        runtime_error(gy, node->line, "Cannot open or create file '%s' for writing", right.as.string->chars);
                    } else {
                        fprintf(file, "%s", left.as.string->chars);
                        fclose(file);
                    }
                }
            } else if (op_type == EQUALITY) {
                result = create_bool_value(are_values_equal(left, right));
            } else if (op_type == INEQUALITY) {
                result = create_bool_value(!are_values_equal(left, right));
            } else if (op_type == XOR) {
                bool left_is_truthy = !is_value_falsy(left);
                bool right_is_truthy = !is_value_falsy(right);
                result = create_bool_value(left_is_truthy != right_is_truthy);
            } else if (op_type == PLUS) {
                if (left.type == VAL_ARRAY) {
                    GraveyardValue new_array_val = create_array_value();
                    for (size_t i = 0; i < left.as.array->count; i++) {
                        array_append(new_array_val.as.array, left.as.array->values[i]);
                    }
                    array_append(new_array_val.as.array, right);
                    result = new_array_val;
                } else if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                    result = create_number_value(left.as.number + right.as.number);
                } else if (left.type == VAL_STRING || right.type == VAL_STRING) {
                    char left_str_temp[1024];
                    char right_str_temp[1024];

                    if (left.type == VAL_STRING) {
                        strncpy(left_str_temp, left.as.string->chars, sizeof(left_str_temp) - 1);
                        left_str_temp[sizeof(left_str_temp) - 1] = '\0';
                    } else {
                        value_to_string(left, left_str_temp, sizeof(left_str_temp));
                    }

                    if (right.type == VAL_STRING) {
                        strncpy(right_str_temp, right.as.string->chars, sizeof(right_str_temp) - 1);
                        right_str_temp[sizeof(right_str_temp) - 1] = '\0';
                    } else {
                        value_to_string(right, right_str_temp, sizeof(right_str_temp));
                    }
                    
                    size_t total_len = strlen(left_str_temp) + strlen(right_str_temp);
                    char* result_buffer = malloc(total_len + 1);

                    if (!result_buffer) {
                        runtime_error(gy, node->line, "Memory allocation failed for string concatenation");
                    } else {
                        strcpy(result_buffer, left_str_temp);
                        strcat(result_buffer, right_str_temp);
                        
                        result = create_string_value(result_buffer);
                        free(result_buffer);
                    }
                } else {
                    runtime_error(gy, node->line, "Operands have incompatible types for '+' operation");
                }
            } else if (op_type == FORWARDSLASH && (left.type == VAL_STRING || right.type == VAL_STRING)) {
                char left_str_temp[1024];
                char right_str_temp[1024];

                if (left.type == VAL_STRING) {
                    strncpy(left_str_temp, left.as.string->chars, sizeof(left_str_temp) - 1);
                    left_str_temp[sizeof(left_str_temp) - 1] = '\0';
                } else {
                    value_to_string(left, left_str_temp, sizeof(left_str_temp));
                }

                if (right.type == VAL_STRING) {
                    strncpy(right_str_temp, right.as.string->chars, sizeof(right_str_temp) - 1);
                    right_str_temp[sizeof(right_str_temp) - 1] = '\0';
                } else {
                    value_to_string(right, right_str_temp, sizeof(right_str_temp));
                }

                size_t left_len = strlen(left_str_temp);
                while (left_len > 0 && (left_str_temp[left_len - 1] == '/' || left_str_temp[left_len - 1] == '\\')) {
                    left_str_temp[--left_len] = '\0';
                }
                
                size_t right_offset = 0;
                while (right_str_temp[right_offset] == '/' || right_str_temp[right_offset] == '\\') {
                    right_offset++;
                }
                
                size_t total_len = strlen(left_str_temp) + strlen(right_str_temp + right_offset) + 1;
                char* result_buffer = malloc(total_len + 1);

                if (!result_buffer) {
                    runtime_error(gy, node->line, "Memory allocation failed for path joining");
                } else {
                    snprintf(result_buffer, total_len + 1, "%s/%s", left_str_temp, right_str_temp + right_offset);
                    
                    result = create_string_value(result_buffer);
                    free(result_buffer);
                }
            } else {
                if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
                    runtime_error(gy, node->line, "Operands must be numbers for this operation");
                } else {
                    switch (op_type) {
                        case MINUS:          result = create_number_value(left.as.number - right.as.number); break;
                        case ASTERISK:       result = create_number_value(left.as.number * right.as.number); break;
                        case EXPONENTIATION: result = create_number_value(pow(left.as.number, right.as.number)); break;
                        case FORWARDSLASH:
                            if (right.as.number == 0) { runtime_error(gy, node->line, "Division by zero"); }
                            else { result = create_number_value(left.as.number / right.as.number); }
                            break;
                        case MODULO:
                            if (right.as.number == 0) { runtime_error(gy, node->line, "Division by zero in modulo operation"); }
                            else { result = create_number_value(fmod(left.as.number, right.as.number)); }
                            break;
                        case GREATERTHAN:      result = create_bool_value(left.as.number > right.as.number); break;
                        case LEFTANGLEBRACKET: result = create_bool_value(left.as.number < right.as.number); break;
                        case GREATERTHANEQUAL: result = create_bool_value(left.as.number >= right.as.number); break;
                        case LESSTHANEQUAL:    result = create_bool_value(left.as.number <= right.as.number); break;
                        default: break;
                    }
                }
            }

            dec_ref(left);
            dec_ref(right);

            return result;
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
            
            dec_ref(gy->return_value);
            gy->return_value = value;
            
            return value;
        }

        case AST_CALL_EXPRESSION: {
            GraveyardValue callee = execute_node(gy, node->as.call_expression.callee);

            GraveyardFunction* function;
            Environment* call_environment;

            if (callee.type == VAL_FUNCTION) {
                function = callee.as.function;
                call_environment = environment_new(function->closure);
            } else if (callee.type == VAL_BOUND_METHOD) {
                GraveyardBoundMethod* bound = callee.as.bound_method;
                function = bound->function.as.function;
                
                call_environment = environment_new(function->closure);
                environment_define(call_environment, "this", bound->receiver);
            } else {
                runtime_error(gy, node->line, "Can only call functions and methods");
                return create_null_value();
            }

            int arg_count = node->as.call_expression.arg_count;

            if (arg_count != function->arity) {
                runtime_error(gy, node->line, "Expected %d arguments but got %d", function->arity, arg_count);
                monolith_free(&call_environment->values);
                free(call_environment);
                return create_null_value();
            }
            
            Environment* new_env = call_environment;
            
            for (int i = 0; i < function->arity; i++) {
                GraveyardValue arg_value = execute_node(gy, node->as.call_expression.arguments[i]);
                environment_define(new_env, function->params[i].lexeme, arg_value);
            }
            
            execute_block(gy, function->body, new_env);

            monolith_free(&call_environment->values);
            free(call_environment);

            GraveyardValue result = create_null_value();
            if (gy->is_returning) {
                result = gy->return_value;
                gy->is_returning = false;
                gy->return_value = create_null_value(); 
            }

            return result;
        }

        case AST_IF_STATEMENT: {
            GraveyardValue condition_val = execute_node(gy, node->as.if_statement.condition);
            bool is_truthy = !is_value_falsy(condition_val);
            
            dec_ref(condition_val);

            if (is_truthy) {
                execute_block(gy, node->as.if_statement.then_branch, environment_new(gy->environment));
                return create_null_value();
            }

            for (size_t i = 0; i < node->as.if_statement.else_if_count; i++) {
                AstNodeElseIfClause* clause = &node->as.if_statement.else_if_clauses[i];
                
                GraveyardValue else_if_condition = execute_node(gy, clause->condition);
                bool else_if_is_truthy = !is_value_falsy(else_if_condition);
                
                dec_ref(else_if_condition);

                if (else_if_is_truthy) {
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
            bool is_falsy = is_value_falsy(condition);

            dec_ref(condition);

            if (!is_falsy) {
                return execute_node(gy, node->as.ternary_expression.then_expr);
            } else {
                return execute_node(gy, node->as.ternary_expression.else_expr);
            }
        }

        case AST_ASSERT_STATEMENT: {
            GraveyardValue condition = execute_node(gy, node->as.assert_statement.condition);
            if (is_value_falsy(condition)) {
                runtime_error(gy, node->line, "Assertion failed");
            }
            return create_null_value();
        }

        case AST_WHILE_STATEMENT: {
            while (true) {
                GraveyardValue condition = execute_node(gy, node->as.while_statement.condition);
                bool is_falsy = is_value_falsy(condition);
                
                dec_ref(condition);

                if (is_falsy) {
                    break;
                }

                execute_block(gy, node->as.while_statement.body, environment_new(gy->environment));

                if (gy->is_returning || gy->encountered_break) {
                    break;
                }
            }
            gy->encountered_break = false;
            gy->encountered_continue = false;
            return create_null_value();
        }

        case AST_BREAK_STATEMENT: {
            gy->encountered_break = true;
            return create_null_value();
        }

        case AST_CONTINUE_STATEMENT: {
            gy->encountered_continue = true;
            return create_null_value();
        }

        case AST_FOR_STATEMENT: {
            const char* iterator_name = node->as.for_statement.iterator.lexeme;
            size_t range_count = node->as.for_statement.range_count;
            AstNode** range_exprs = node->as.for_statement.range_expressions;

            if (range_count == 1) {
                GraveyardValue collection = execute_node(gy, range_exprs[0]);
                
                if (collection.type == VAL_ARRAY) {
                    GraveyardArray* array = collection.as.array;
                    for (size_t i = 0; i < array->count; i++) {
                        Environment* loop_env = environment_new(gy->environment);
                        environment_define(loop_env, iterator_name, array->values[i]);
                        execute_block(gy, node->as.for_statement.body, loop_env);
                        
                        monolith_free(&loop_env->values);
                        free(loop_env);

                        if (gy->is_returning || gy->encountered_break) break;
                    }
                } else if (collection.type == VAL_HASHTABLE) {
                    GraveyardHashtable* ht = collection.as.hashtable;
                    for (int i = 0; i < ht->capacity; i++) {
                        if (!ht->entries[i].is_in_use) continue;
                        Environment* loop_env = environment_new(gy->environment);
                        environment_define(loop_env, iterator_name, ht->entries[i].key);
                        execute_block(gy, node->as.for_statement.body, loop_env);

                        monolith_free(&loop_env->values);
                        free(loop_env);

                        if (gy->is_returning || gy->encountered_break) break;
                    }
                } else if (collection.type == VAL_NUMBER) {
                    double stop_val = collection.as.number;
                    for (double i = 0; i < stop_val; i += 1) {
                        Environment* loop_env = environment_new(gy->environment);
                        environment_define(loop_env, iterator_name, create_number_value(i));
                        execute_block(gy, node->as.for_statement.body, loop_env);

                        monolith_free(&loop_env->values);
                        free(loop_env);

                        if (gy->is_returning || gy->encountered_break) break;
                    }
                } else {
                    runtime_error(gy, node->line, "Invalid type for single-argument for loop");
                }
                
                dec_ref(collection);

            } else {
                GraveyardValue start_gv = execute_node(gy, range_exprs[0]);
                if (start_gv.type != VAL_NUMBER) {
                    runtime_error(gy, range_exprs[0]->line, "For loop range arguments must be numbers");
                    dec_ref(start_gv);
                    return create_null_value();
                }

                GraveyardValue stop_gv = execute_node(gy, range_exprs[1]);
                if (stop_gv.type != VAL_NUMBER) {
                    runtime_error(gy, range_exprs[1]->line, "For loop range arguments must be numbers");
                    dec_ref(start_gv);
                    dec_ref(stop_gv);
                    return create_null_value();
                }

                GraveyardValue step_gv = create_number_value(1.0);
                if (range_count == 3) {
                    dec_ref(step_gv);
                    step_gv = execute_node(gy, range_exprs[2]);
                    if (step_gv.type != VAL_NUMBER) {
                        runtime_error(gy, range_exprs[2]->line, "For loop step argument must be a number");
                        dec_ref(start_gv);
                        dec_ref(stop_gv);
                        dec_ref(step_gv);
                        return create_null_value();
                    }
                }

                double start_val = start_gv.as.number;
                double stop_val = stop_gv.as.number;
                double step_val = step_gv.as.number;
                
                if (step_val == 0) {
                    runtime_error(gy, node->line, "For loop step cannot be zero");
                } else {
                    for (double i = start_val; (step_val > 0) ? (i < stop_val) : (i > stop_val); i += step_val) {
                        Environment* loop_env = environment_new(gy->environment);
                        environment_define(loop_env, iterator_name, create_number_value(i));
                        execute_block(gy, node->as.for_statement.body, loop_env);
                        
                        monolith_free(&loop_env->values);
                        free(loop_env);

                        if (gy->is_returning || gy->encountered_break) break;
                    }
                }

                dec_ref(start_gv);
                dec_ref(stop_gv);
                dec_ref(step_gv);
            }
            
            gy->encountered_break = false;
            gy->encountered_continue = false;
            return create_null_value();
        }


        case AST_RAISE_STATEMENT: {
            GraveyardValue error_val = execute_node(gy, node->as.raise_statement.error_expr);
            char error_buffer[1024];
            value_to_string(error_val, error_buffer, sizeof(error_buffer));
            
            runtime_error(gy, node->line, "%s", error_buffer);
            
            return create_null_value();
        }

        case AST_TIME_EXPRESSION: {
            struct timespec ts;
            timespec_get(&ts, TIME_UTC);
            double epoch_time_precise = (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
            return create_number_value(epoch_time_precise);
        }

        case AST_NAMESPACE_DECLARATION: {
            const char* name = node->as.namespace_declaration.name.lexeme;
            GraveyardValue ns_val;
            Environment* ns_env;

            if (monolith_get(&gy->namespaces, name, &ns_val)) {
                ns_env = ns_val.as.environment;
            } else {
                Environment* global_env = get_global_environment(gy);
                ns_env = environment_new(global_env);
                
                GraveyardValue new_ns_val;
                new_ns_val.type = VAL_ENVIRONMENT;
                new_ns_val.as.environment = ns_env;
                monolith_set(&gy->namespaces, name, new_ns_val);
            }

            execute_block(gy, node->as.namespace_declaration.body, ns_env);
            return create_null_value();
        }

        case AST_NAMESPACE_ACCESS: {
            const char* ns_name = node->as.namespace_access.namespace_name.lexeme;
            const char* member_name = node->as.namespace_access.member_name.lexeme;
            GraveyardValue ns_val;

            if (!monolith_get(&gy->namespaces, ns_name, &ns_val)) {
                runtime_error(gy, node->line, "Namespace '%s' is not defined", ns_name);
                return create_null_value();
            }

            Environment* ns_env = ns_val.as.environment;
            GraveyardValue member_val;

            if (!environment_get(ns_env, member_name, &member_val)) {
                runtime_error(gy, node->line, "Member '%s' not found in namespace '%s'", member_name, ns_name);
                return create_null_value();
            }
            
            inc_ref(member_val);
            
            return member_val;
        }

        case AST_FILEREAD_STATEMENT: {
            GraveyardValue path_val = execute_node(gy, node->as.fileread_statement.path_expr);
            if (path_val.type != VAL_STRING) {
                runtime_error(gy, node->line, "File path for read operation must be a string");
                return create_null_value();
            }
            
            FILE* file = fopen(path_val.as.string->chars, "rb");
            if (!file) {
                runtime_error(gy, node->line, "Cannot open file '%s'", path_val.as.string->chars);
                return create_null_value();
            }

            char* buffer = load(file, NULL);
            fclose(file);
            if (!buffer) {
                runtime_error(gy, node->line, "Failed to read file '%s'", path_val.as.string->chars);
                return create_null_value();
            }

            GraveyardValue file_contents = create_string_value(buffer);
            free(buffer);
            
            const char* var_name = node->as.fileread_statement.variable.lexeme;
            environment_define(gy->environment, var_name, file_contents);
            
            return file_contents;
        }

        case AST_TYPE_DECLARATION: {
            char type_name_buffer[MAX_LEXEME_LEN];
            const char* lexeme = node->as.type_declaration.name.lexeme;
            size_t len = strlen(lexeme);
            
            if (len > 2) {
                size_t type_name_len = len - 2;
                strncpy(type_name_buffer, lexeme + 1, type_name_len);
                type_name_buffer[type_name_len] = '\0';
            } else {
                type_name_buffer[0] = '\0';
            }

            GraveyardValue type_val = create_type_value(create_string_value(type_name_buffer).as.string);
            
            environment_define(gy->environment, type_name_buffer, type_val);

            Environment* type_env = environment_new(gy->environment);
            execute_block(gy, node->as.type_declaration.body, type_env);
            
            GraveyardType* type = type_val.as.type;
            for (int i = 0; i < type_env->values.capacity; i++) {
                MonolithEntry* entry = &type_env->values.entries[i];
                if (entry->key == NULL) continue;

                if (entry->value.type == VAL_FUNCTION) {
                    monolith_set(&type->methods, entry->key, entry->value);
                } else {
                    monolith_set(&type->fields, entry->key, entry->value);
                }
            }
            monolith_free(&type_env->values);
            free(type_env);

            return type_val;
        }

        case AST_THIS_EXPRESSION: {
            GraveyardValue this_val;
            if (!environment_get(gy->environment, "this", &this_val)) {
                runtime_error(gy, node->line, "Cannot use '.' outside of a type's method");
                return create_null_value();
            }
            return this_val;
        }

        case AST_MEMBER_ACCESS: {
            GraveyardValue object = execute_node(gy, node->as.member_access.object);
            const char* member_name = node->as.member_access.member.lexeme;

            if (object.type != VAL_INSTANCE) {
                runtime_error(gy, node->line, "Can only access members on an instance of a type");
                return create_null_value();
            }

            GraveyardInstance* instance = object.as.instance;
            GraveyardValue member_val;

            if (monolith_get(&instance->fields, member_name, &member_val)) {
                return member_val;
            }

            if (monolith_get(&instance->type->methods, member_name, &member_val)) {
                GraveyardValue bound_method_val;
                bound_method_val.type = VAL_BOUND_METHOD;
                GraveyardBoundMethod* bound = malloc(sizeof(GraveyardBoundMethod));
                
                bound->ref_count = 1;
                bound->receiver = object;
                bound->function = member_val;
                
                inc_ref(bound->receiver);
                inc_ref(bound->function);
                
                bound_method_val.as.bound_method = bound;
                return bound_method_val;
            }

            runtime_error(gy, node->line, "Member '%s' not found on instance", member_name);
            return create_null_value();
        }

        case AST_EXECUTE_EXPRESSION: {
            GraveyardValue command_val = execute_node(gy, node->as.execute_expression.command_expr);
            if (command_val.type != VAL_STRING) {
                runtime_error(gy, node->line, "Command for execute operation must be a string");
                return create_null_value();
            }

            char full_command[2048];
            snprintf(full_command, sizeof(full_command), "%s 2>&1", command_val.as.string->chars);

            #ifdef _WIN32
                FILE* pipe = _popen(full_command, "r");
            #else
                FILE* pipe = popen(full_command, "r");
            #endif

            if (!pipe) {
                runtime_error(gy, node->line, "Failed to execute command");
                return create_null_value();
            }

            char buffer[128];
            size_t output_capacity = 256;
            size_t output_len = 0;
            char* output_str = malloc(output_capacity);
            output_str[0] = '\0';

            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                size_t buffer_len = strlen(buffer);
                if (output_len + buffer_len + 1 > output_capacity) {
                    output_capacity *= 2;
                    output_str = realloc(output_str, output_capacity);
                }
                strcat(output_str, buffer);
                output_len += buffer_len;
            }

            int exit_code;
            #ifdef _WIN32
                exit_code = _pclose(pipe);
            #else
                int status = pclose(pipe);
                exit_code = WEXITSTATUS(status);
            #endif

            GraveyardValue result_ht = create_hashtable_value();
            hashtable_set(result_ht.as.hashtable, create_string_value("stdout"), create_string_value(output_str));
            hashtable_set(result_ht.as.hashtable, create_string_value("stderr"), create_string_value(""));
            hashtable_set(result_ht.as.hashtable, create_string_value("exit_code"), create_number_value(exit_code));

            free(output_str);

            return result_ht;
        }

        case AST_CAT_CONSTANT_EXPRESSION: {
            return create_number_value(65458655);
        }

        case AST_WAIT_STATEMENT: {
            GraveyardValue duration_val = execute_node(gy, node->as.wait_statement.duration_expr);
            if (duration_val.type != VAL_NUMBER) {
                runtime_error(gy, node->line, "Duration for wait operation must be a number");
                return create_null_value();
            }

            long milliseconds = (long)duration_val.as.number;
            if (milliseconds < 0) {
                milliseconds = 0;
            }

            #ifdef _WIN32
                Sleep(milliseconds);
            #else
                usleep(milliseconds * 1000);
            #endif

            return create_null_value();
        }

        case AST_RANDOM_EXPRESSION: {
            double random_val = (double)rand() / (double)RAND_MAX;
            return create_number_value(random_val);
        }

        case AST_GLOBAL_ACCESS: {
            const char* member_name = node->as.global_access.member_name.lexeme;
            Environment* global_env = get_global_environment(gy);
            GraveyardValue member_val;

            if (!environment_get(global_env, member_name, &member_val)) {
                runtime_error(gy, node->line, "Global variable '%s' not found", member_name);
                return create_null_value();
            }
            return member_val;
        }

        case AST_STATIC_ACCESS: {
            char type_name_buffer[MAX_LEXEME_LEN];
            const char* lexeme = node->as.static_access.type_name.lexeme;
            size_t len = strlen(lexeme);

            if (len > 2) {
                size_t type_name_len = len - 2;
                strncpy(type_name_buffer, lexeme + 1, type_name_len);
                type_name_buffer[type_name_len] = '\0';
            } else {
                type_name_buffer[0] = '\0';
            }
            
            const char* member_name = node->as.static_access.member_name.lexeme;

            GraveyardValue type_val;
            if (!environment_get(gy->environment, type_name_buffer, &type_val) || type_val.type != VAL_TYPE) {
                runtime_error(gy, node->line, "Type <%s> is not defined", type_name_buffer);
                return create_null_value();
            }
            GraveyardType* type = type_val.as.type;
            GraveyardValue member_val;

            if (monolith_get(&type->methods, member_name, &member_val)) {
                return member_val;
            }

            if (monolith_get(&type->fields, member_name, &member_val)) {
                return member_val;
            }
            
            runtime_error(gy, node->line, "Static member '%s' not found on type <%s>", member_name, type_name_buffer);
            return create_null_value();
        }

        case AST_UID_EXPRESSION: {
            GraveyardValue length_val = execute_node(gy, node->as.uid_expression.length_expr);
            GraveyardValue result = create_null_value();

            if (length_val.type != VAL_NUMBER) {
                runtime_error(gy, node->line, "Length for UID operation must be a number");
            } else {
                int length = (int)length_val.as.number;
                if (length <= 0) {
                    runtime_error(gy, node->line, "UID length must be a positive number");
                } else {
                    char* uid_str = generate_random_hex_string(length);
                    if (!uid_str) {
                        runtime_error(gy, node->line, "Failed to generate random UID");
                    } else {
                        result = create_string_value(uid_str);
                        free(uid_str);
                    }
                }
            }

            dec_ref(length_val);
            
            return result;
        }

        case AST_SLICE_EXPRESSION: {
            GraveyardValue collection = execute_node(gy, node->as.slice_expression.collection);
            GraveyardValue result = create_null_value();

            if (collection.type == VAL_ARRAY) {
                GraveyardArray* arr = collection.as.array;
                long start, stop, step;
                
                if (!calculate_slice_bounds(arr->count, node->as.slice_expression.start_expr,
                                            node->as.slice_expression.stop_expr, node->as.slice_expression.step_expr,
                                            &start, &stop, &step, gy)) {
                    runtime_error(gy, node->line, "Slice step cannot be zero");
                } else {
                    GraveyardValue result_array = create_array_value();
                    if (step > 0 && start < stop) {
                        for (long i = start; i < stop; i += step) {
                            if (i < 0 || i >= arr->count) continue;
                            array_append(result_array.as.array, arr->values[i]);
                        }
                    } else if (step < 0 && start > stop) {
                        for (long i = start; i > stop; i += step) {
                            if (i < 0 || i >= arr->count) continue;
                            array_append(result_array.as.array, arr->values[i]);
                        }
                    }
                    result = result_array;
                }
            } else if (collection.type == VAL_STRING) {
                GraveyardString* str = collection.as.string;
                long start, stop, step;

                if (!calculate_slice_bounds(str->length, node->as.slice_expression.start_expr,
                                            node->as.slice_expression.stop_expr, node->as.slice_expression.step_expr,
                                            &start, &stop, &step, gy)) {
                    runtime_error(gy, node->line, "Slice step cannot be zero");
                } else {
                    char* new_chars = malloc(str->length + 1);
                    if (!new_chars) {
                        runtime_error(gy, node->line, "Memory allocation failed for string slice");
                    } else {
                        size_t new_len = 0;
                        if (step > 0 && start < stop) {
                            for (long i = start; i < stop; i += step) {
                                if (i < 0 || i >= str->length) continue;
                                new_chars[new_len++] = str->chars[i];
                            }
                        } else if (step < 0 && start > stop) {
                            for (long i = start; i > stop; i += step) {
                                if (i < 0 || i >= str->length) continue;
                                new_chars[new_len++] = str->chars[i];
                            }
                        }
                        new_chars[new_len] = '\0';
                        
                        result = create_string_value(new_chars);
                        free(new_chars);
                    }
                }
            } else {
                runtime_error(gy, node->line, "Slicing can only be applied to arrays and strings");
            }

            dec_ref(collection);
            
            return result;
        }

        case AST_ARGV_EXPRESSION: {
            return gy->arguments;
        }

        case AST_EVAL_EXPRESSION: {
            GraveyardValue code_val = execute_node(gy, node->as.eval_expression.code_expr);
            if (code_val.type != VAL_STRING) {
                runtime_error(gy, node->line, "Argument for eval operation must be a string");
                return create_null_value();
            }

            char* saved_source = gy->source_code;
            Token* saved_tokens = gy->tokens;
            size_t       saved_token_count = gy->token_count;
            AstNode* saved_ast_root = gy->ast_root;
            Environment* saved_env = gy->environment;

            gy->source_code = strdup(code_val.as.string->chars);
            gy->tokens = NULL;
            gy->token_count = 0;
            gy->ast_root = NULL;
            
            gy->environment = environment_new(saved_env);

            GraveyardValue result = create_null_value();
            if (tokenize(gy) && parse(gy) && execute(gy)) {
                result = gy->last_executed_value;
            } else {
                runtime_error(gy, node->line, "Failed to evaluate string");
            }

            free(gy->source_code);
            free(gy->tokens);
            free_ast(gy->ast_root);
            monolith_free(&gy->environment->values);
            free(gy->environment);

            gy->source_code = saved_source;
            gy->tokens = saved_tokens;
            gy->token_count = saved_token_count;
            gy->ast_root = saved_ast_root;
            gy->environment = saved_env;

            return result;
        }

        case AST_EXISTS_EXPRESSION: {
            GraveyardValue path_val = execute_node(gy, node->as.exists_expression.path_expr);
            if (path_val.type != VAL_STRING) {
                runtime_error(gy, node->line, "Path for exists check must be a string");
                return create_null_value();
            }

            struct stat buffer;
            bool exists = (stat(path_val.as.string->chars, &buffer) == 0);
            
            return create_bool_value(exists);
        }

        case AST_LISTDIR_EXPRESSION: {
            GraveyardValue path_val = execute_node(gy, node->as.listdir_expression.path_expr);
            if (path_val.type != VAL_STRING) {
                runtime_error(gy, node->line, "Path for list directory must be a string");
                return create_null_value();
            }
            const char* path = path_val.as.string->chars;

            GraveyardValue result_array = create_array_value();

        #ifdef _WIN32
            char search_path[1024];
            snprintf(search_path, sizeof(search_path), "%s\\*", path);
            WIN32_FIND_DATA find_data;
            HANDLE find_handle = FindFirstFile(search_path, &find_data);

            if (find_handle == INVALID_HANDLE_VALUE) {
                runtime_error(gy, node->line, "Cannot open directory '%s'", path);
                return create_null_value();
            }

            do {
                if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
                    array_append(result_array.as.array, create_string_value(find_data.cFileName));
                }
            } while (FindNextFile(find_handle, &find_data) != 0);

            FindClose(find_handle);
        #else
            DIR* dir = opendir(path);
            if (!dir) {
                runtime_error(gy, node->line, "Cannot open directory '%s'", path);
                return create_null_value();
            }

            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    array_append(result_array.as.array, create_string_value(entry->d_name));
                }
            }
            closedir(dir);
        #endif

            return result_array;
        }

        case AST_TRY_EXCEPT_STATEMENT: {
            execute_node(gy, node->as.try_except_statement.try_block);
            
            bool error_occurred = gy->had_runtime_error;
            if (error_occurred) {
                gy->had_runtime_error = false;

                if (node->as.try_except_statement.except_clause) {
                    AstNodeExceptClause* clause = node->as.try_except_statement.except_clause;
                    
                    GraveyardValue error_obj = create_hashtable_value();
                    
                    GraveyardValue key_msg = create_string_value("message");
                    GraveyardValue val_msg = create_string_value(gy->error_message);
                    hashtable_set(error_obj.as.hashtable, key_msg, val_msg);
                    dec_ref(key_msg);
                    dec_ref(val_msg);

                    GraveyardValue key_line = create_string_value("line");
                    GraveyardValue val_line = create_number_value(gy->error_line);
                    hashtable_set(error_obj.as.hashtable, key_line, val_line);
                    dec_ref(key_line);
                    dec_ref(val_line);

                    Environment* except_env = environment_new(gy->environment);
                    environment_define(except_env, clause->error_variable.lexeme, error_obj);
                    
                    dec_ref(error_obj);

                    execute_block(gy, clause->body, except_env);
                    
                    monolith_free(&except_env->values);
                    free(except_env);
                }
            }

            if (node->as.try_except_statement.finally_block) {
                execute_node(gy, node->as.try_except_statement.finally_block);
            }

            return create_null_value();
        }
    }

    return create_null_value();
}

bool execute(Graveyard *gy) {
    if (!gy || !gy->ast_root) {
        fprintf(stderr, "Execution error: Nothing to execute (no AST).\n");
        return false;
    }
    
    gy->had_runtime_error = false;
    gy->last_executed_value = execute_node(gy, gy->ast_root);
    
    return !gy->had_runtime_error; 
}

//MAIN----------------------------------------------------------------------------

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

const char* token_type_to_string(GraveyardTokenType type) {
    switch (type) {
        case SEMICOLON: return "SEMICOLON";
        case IDENTIFIER: return "IDENTIFIER";
        case TRUEVALUE: return "TRUEVALUE";
        case FALSEVALUE: return "FALSEVALUE";
        case NULLVALUE: return "NULLVALUE";
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case FORMATTEDSTRING: return "FORMATTEDSTRING";
        case ASSIGNMENT: return "ASSIGNMENT";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case ASTERISK: return "ASTERISK";
        case FORWARDSLASH: return "FORWARDSLASH";
        case EXPONENTIATION: return "EXPONENTIATION";
        case MODULO: return "MODULO";
        case PLUSASSIGNMENT: return "PLUSASSIGNMENT";
        case SUBTRACTIONASSIGNMENT: return "SUBTRACTIONASSIGNMENT";
        case MULTIPLICATIONASSIGNMENT: return "MULTIPLICATIONASSIGNMENT";
        case DIVISIONASSIGNMENT: return "DIVISIONASSIGNMENT";
        case EXPONENTIATIONASSIGNMENT: return "EXPONENTIATIONASSIGNMENT";
        case MODULOASSIGNMENT: return "MODULOASSIGNMENT";
        case INCREMENT: return "INCREMENT";
        case DECREMENT: return "DECREMENT";
        case EQUALITY: return "EQUALITY";
        case INEQUALITY: return "INEQUALITY";
        case GREATERTHAN: return "GREATERTHAN";
        case LEFTANGLEBRACKET: return "LEFTANGLEBRACKET";
        case GREATERTHANEQUAL: return "GREATERTHANEQUAL";
        case LESSTHANEQUAL: return "LESSTHANEQUAL";
        case NOT: return "NOT";
        case AND: return "AND";
        case OR: return "OR";
        case XOR: return "XOR";
        case LEFTPARENTHESES: return "LEFTPARENTHESES";
        case RIGHTPARENTHESES: return "RIGHTPARENTHESES";
        case LEFTBRACKET: return "LEFTBRACKET";
        case RIGHTBRACKET: return "RIGHTBRACKET";
        case LEFTBRACE: return "LEFTBRACE";
        case RIGHTBRACE: return "RIGHTBRACE";
        case PARAMETER: return "PARAMETER";
        case RETURN: return "RETURN";
        case QUESTIONMARK: return "QUESTIONMARK";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case WHILE: return "WHILE";
        case CARET: return "CARET";
        case BACKTICK: return "BACKTICK";
        case AT: return "AT";
        case NULLCOALESCE: return "NULLCOALESCE";
        case PERIOD: return "PERIOD";
        case NAMESPACE: return "NAMESPACE";
        case REFERENCE: return "REFERENCE";
        case PRINT: return "PRINT";
        case SCAN: return "SCAN";
        case FILEREAD: return "FILEREAD";
        case FILEWRITE: return "FILEWRITE";
        case RAISE: return "RAISE";
        case CASTBOOLEAN: return "CASTBOOLEAN";
        case CASTINTEGER: return "CASTINTEGER";
        case CASTFLOAT: return "CASTFLOAT";
        case CASTSTRING: return "CASTSTRING";
        case CASTARRAY: return "CASTARRAY";
        case CASTHASHTABLE: return "CASTHASHTABLE";
        case TYPEOF: return "TYPEOF";
        case TIME: return "TIME";
        case WAIT: return "WAIT";
        case RANDOM: return "RANDOM";
        case EXECUTE: return "EXECUTE";
        case CATCONSTANT: return "CATCONSTANT";
        case TOKENEOF: return "TOKENEOF";
        case FORMATTEDSTART: return "FORMATTEDSTART";
        case FORMATTEDEND: return "FORMATTEDEND";
        case FORMATTEDPART: return "FORMATTEDPART";
        case TYPE: return "TYPE";
        case UNKNOWN: return "UNKNOWN";
        default: return "INVALID_TOKEN";
    }
}

void print_tokens(Graveyard *gy) {
    printf("--- Token Stream ---\n");
    printf("LINE | COL  | %-25s | LEXEME\n", "TYPE");
    printf("---------------------------------------------------------\n");
    for (size_t i = 0; i < gy->token_count; i++) {
        Token token = gy->tokens[i];
        printf("%-4d | %-4d | %-25s | '%s'\n",
            token.line,
            token.column,
            token_type_to_string(token.type),
            token.lexeme
        );
    }
    printf("---------------------------------------------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: graveyard <mode> <source file> [args...]\n");
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

    gy->arguments = create_hashtable_value();
    GraveyardValue args_list = create_array_value();
    GraveyardValue kwargs_ht = create_hashtable_value();

    for (int i = 3; i < argc; i++) {
        char* arg = argv[i];
        if (strncmp(arg, "--", 2) == 0) {
            char* key = arg + 2;
            char* value_ptr = strchr(key, '=');
            
            if (value_ptr != NULL) {
                *value_ptr = '\0';
                char* value = value_ptr + 1;
                hashtable_set(kwargs_ht.as.hashtable, create_string_value(key), create_string_value(value));
            } else {
                hashtable_set(kwargs_ht.as.hashtable, create_string_value(key), create_bool_value(true));
            }
        } else if (strncmp(arg, "-", 1) == 0) {
            char* key = arg + 1;
            hashtable_set(kwargs_ht.as.hashtable, create_string_value(key), create_bool_value(true));
        } else {
            array_append(args_list.as.array, create_string_value(arg));
        }
    }
    
    hashtable_set(gy->arguments.as.hashtable, create_string_value("args"), args_list);
    hashtable_set(gy->arguments.as.hashtable, create_string_value("kwargs"), kwargs_ht);

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
                    if (gy->had_runtime_error) {
                        fprintf(stderr, "Runtime Error [line %d]: %s\n", gy->error_line, gy->error_message);
                    }
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
            char* raw_source = load(file, NULL);
            fclose(file);
            if (!raw_source) {
                fprintf(stderr, "Failed to load source file.\n");
                success = false;
            } else {
                gy->source_code = run_preprocessor(raw_source);
                
                free(raw_source);

                if (!gy->source_code) {
                    fprintf(stderr, "Fatal error: Halting due to preprocessing failure.\n");
                    success = false;
                }

                if (success) {
                    if (strcmp(gy->mode, "--tokenize") == 0 || strcmp(gy->mode, "-t") == 0) {
                        if (!tokenize(gy)) {
                            success = false;
                        } else {
                            print_tokens(gy);
                        }
                    } else if (strcmp(gy->mode, "--parse") == 0 || strcmp(gy->mode, "-p") == 0) {
                        if (!compile_source(gy)) { success = false; }
                    } else if (strcmp(gy->mode, "--execute") == 0 || strcmp(gy->mode, "-e") == 0) {
                        if (!compile_source(gy) || !execute(gy)) {
                            if (gy->had_runtime_error) {
                                fprintf(stderr, "Runtime Error [line %d]: %s\n", gy->error_line, gy->error_message);
                            }
                            success = false;
                        }
                    } else if (strcmp(gy->mode, "--debug") == 0 || strcmp(gy->mode, "-d") == 0) {
                        if (compile_source(gy) && execute(gy)) {
                            graveyard_debug_print(gy);
                        } else {
                            if (gy->had_runtime_error) {
                                fprintf(stderr, "Runtime Error [line %d]: %s\n", gy->error_line, gy->error_message);
                            }
                            success = false;
                        }
                    } else {
                        fprintf(stderr, "Unknown mode for .gy file: %s\n", gy->mode);
                        success = false;
                    }
                }
            }
        }
    }
    
    graveyard_free(gy);
    
    return success ? 0 : 1;
}
