#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LEXEME_LEN 65

// --- Type Definitions ---

typedef enum {
    IDENTIFIER, TYPE, NUMBER, SEMICOLON, ASSIGNMENT,
    ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, EXPONENTIATION,
    LEFTPARENTHESES, RIGHTPARENTHESES, EQUALITY, INEQUALITY,
    GREATERTHAN, LESSTHAN, GREATERTHANEQUAL, LESSTHANEQUAL,
    NOT, AND, OR, COMMA, TRUEVALUE, FALSEVALUE, NULLVALUE,
    STRING, LEFTBRACE, RIGHTBRACE, PARAMETER, RETURN,
    QUESTIONMARK, COLON, WHILE, CONTINUE, BREAK, AT,
    FORMATTEDSTRING, LEFTBRACKET, RIGHTBRACKET,
    ADDITIONASSIGNMENT, SUBTRACTIONASSIGNMENT, MULTIPLICATIONASSIGNMENT,
    DIVISIONASSIGNMENT, EXPONENTIATIONASSIGNMENT,
    INCREMENT, DECREMENT, REFERENCE, PERIOD, NAMESPACE,
    PRINT, SCAN, RAISE, CASTBOOLEAN, CASTINTEGER, CASTFLOAT,
    CASTSTRING, CASTARRAY, CASTHASHTABLE, TYPEOF, MODULO,
    FILEREAD, FILEWRITE, TIME, EXECUTE, CATCONSTANT,
    UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
} Token;

// --- Main Interpreter Struct ---

typedef struct {
    // Input parameters
    const char *mode;
    const char *filename;

    // Phase outputs
    char *source_code;
    char *preprocessed_source;
    Token *tokens;
    size_t token_count;
} Graveyard;


// --- Forward Declarations for Helper Functions ---

char *load(FILE *file, long *out_length);
TokenType identify_single_char_token(char c);
TokenType identify_two_char_token(char first, char second);
TokenType identify_three_char_token(char first, char second, char third);

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
    gy->preprocessed_source = NULL;
    gy->tokens = NULL;
    gy->token_count = 0;
    return gy;
}

void graveyard_free(Graveyard *gy) {
    if (!gy) return;
    free(gy->source_code);
    free(gy->preprocessed_source);
    free(gy->tokens);
    free(gy);
}

// --- Core Logic Functions ---

bool preprocess(Graveyard *gy) {
    // This single-pass function finds the global scope, removes comments,
    // and strips whitespace, producing a clean string for the tokenizer.
    
    // 1. Find Boundaries (like the old 'entry' function)
    const char *source = gy->source_code;
    const char *start_ptr = strstr(source, "::{");
    if (!start_ptr) {
        fprintf(stderr, "Preprocessor error: Failed to find global scope start '::{'.\n");
        return false;
    }
    start_ptr += 3; // Move past "::{"

    const char *end_ptr = strrchr(start_ptr, '}');
    if (!end_ptr) {
        fprintf(stderr, "Preprocessor error: Failed to find global scope end '}'.\n");
        return false;
    }

    // 2. Single Allocation (safe upper bound)
    size_t max_len = end_ptr - start_ptr;
    char *result_buffer = malloc(max_len + 1);
    if (!result_buffer) {
        perror("preprocess: malloc failed");
        return false;
    }
    char *dst = result_buffer;

    // 3. Single-Pass State Machine
    typedef enum {
        S_DEFAULT, S_LINE_COMMENT, S_BLOCK_COMMENT,
        S_DOUBLE_QUOTE_STRING, S_SINGLE_QUOTE_STRING
    } PreprocessState;

    PreprocessState state = S_DEFAULT;
    const char *src = start_ptr;

    while (src < end_ptr) {
        char c = *src;
        char next_c = (src + 1 < end_ptr) ? *(src + 1) : '\0';

        switch (state) {
            case S_DEFAULT:
                if (c == '/' && next_c == '/') { state = S_LINE_COMMENT; src += 2; }
                else if (c == '/' && next_c == '*') { state = S_BLOCK_COMMENT; src += 2; }
                else if (c == '"') { state = S_DOUBLE_QUOTE_STRING; *dst++ = c; src++; }
                else if (c == '\'') { state = S_SINGLE_QUOTE_STRING; *dst++ = c; src++; }
                else if (isspace((unsigned char)c)) { src++; } // Skip whitespace
                else { *dst++ = c; src++; }
                break;

            case S_LINE_COMMENT:
                if (c == '\n') { state = S_DEFAULT; }
                src++;
                break;

            case S_BLOCK_COMMENT:
                if (c == '*' && next_c == '/') { state = S_DEFAULT; src += 2; }
                else { src++; }
                break;

            case S_DOUBLE_QUOTE_STRING:
            case S_SINGLE_QUOTE_STRING:
                if (c == '\\') { // Handle escape sequence
                    *dst++ = c; src++;
                    if (src < end_ptr) { *dst++ = *src; src++; }
                } else if ((state == S_DOUBLE_QUOTE_STRING && c == '"') || (state == S_SINGLE_QUOTE_STRING && c == '\'')) {
                    state = S_DEFAULT; *dst++ = c; src++;
                } else {
                    *dst++ = c; src++;
                }
                break;
        }
    }

    // Check for unterminated states
    if (state == S_BLOCK_COMMENT) {
        fprintf(stderr, "Preprocessor error: Unterminated block comment.\n");
        free(result_buffer);
        return false;
    }
    if (state == S_DOUBLE_QUOTE_STRING || state == S_SINGLE_QUOTE_STRING) {
        fprintf(stderr, "Preprocessor error: Unterminated string literal.\n");
        free(result_buffer);
        return false;
    }

    // 4. Finalize and Store
    *dst = '\0';
    gy->preprocessed_source = result_buffer;
    return true;
}

bool tokenize(Graveyard *gy) {
    const char* source_code = gy->preprocessed_source;
    size_t source_len = strlen(source_code);
    size_t capacity = 16;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    if (!tokens) { perror("tokenize: malloc failed"); return false; }

    typedef enum { STATE_DEFAULT, STATE_IN_FMT_STRING } TokenizerState;
    TokenizerState state = STATE_DEFAULT;
    int fstring_brace_depth = 0;
    size_t i = 0;

    while (i < source_len) {
        if (count == capacity) {
            size_t new_capacity = capacity * 2;
            Token *new_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!new_tokens) { perror("tokenize: realloc failed"); goto cleanup_failure; }
            tokens = new_tokens;
            capacity = new_capacity;
        }

        if (state == STATE_IN_FMT_STRING) {
            char c = source_code[i];
            if (c == '\'') { state = STATE_DEFAULT; i++; continue; }
            if (c == '{') {
                state = STATE_DEFAULT; fstring_brace_depth = 1;
                tokens[count].type = LEFTBRACE; tokens[count].lexeme[0] = '{'; tokens[count].lexeme[1] = '\0';
                count++; i++; continue;
            }
            if (c == '\n' || c == '\0') {
                fprintf(stderr, "Tokenizer error: Unterminated formatted string.\n");
                goto cleanup_failure;
            }
            size_t start = i;
            while (i < source_len && source_code[i] != '\'' && source_code[i] != '{' && source_code[i] != '\n') { i++; }
            size_t len = i - start;
            if (len > 0) {
                 if (len >= MAX_LEXEME_LEN) {
                    fprintf(stderr, "Tokenizer error: Literal part of formatted string is too long.\n");
                    goto cleanup_failure;
                }
                tokens[count].type = FORMATTEDSTRING;
                strncpy(tokens[count].lexeme, source_code + start, len);
                tokens[count].lexeme[len] = '\0';
                count++;
            }
            continue;
        }

        if (isspace((unsigned char)source_code[i])) { i++; continue; }

        char c = source_code[i];

        if (c == '\'') { state = STATE_IN_FMT_STRING; i++; continue; }
        
        if (fstring_brace_depth > 0) {
            if (c == '}') {
                fstring_brace_depth--;
                if (fstring_brace_depth == 0) { state = STATE_IN_FMT_STRING; }
            } else if (c == '{') { fstring_brace_depth++; }
        }

        if (isalpha((unsigned char)c) || c == '_') {
            size_t start = i; size_t len = 0;
            while (i < source_len && (isalnum((unsigned char)source_code[i]) || source_code[i] == '_') && len < MAX_LEXEME_LEN - 1) { i++; len++; }
            if (len == MAX_LEXEME_LEN - 1 && (isalnum((unsigned char)source_code[i]) || source_code[i] == '_')) {
                fprintf(stderr, "Tokenizer error: Identifier too long.\n"); goto cleanup_failure;
            }
            tokens[count].type = IDENTIFIER;
            strncpy(tokens[count].lexeme, source_code + start, len); tokens[count].lexeme[len] = '\0'; count++;
        } else if (isdigit((unsigned char)c) || (c == '.' && i + 1 < source_len && isdigit((unsigned char)source_code[i+1]))) {
            size_t start = i; size_t len = 0;
            while (i < source_len && isdigit((unsigned char)source_code[i]) && len < MAX_LEXEME_LEN - 1) { i++; len++; }
            if (i + 1 < source_len && source_code[i] == '.' && isdigit((unsigned char)source_code[i + 1])) {
                i++; len++;
                while (i < source_len && isdigit((unsigned char)source_code[i]) && len < MAX_LEXEME_LEN - 1) { i++; len++; }
            }
            if (len == MAX_LEXEME_LEN - 1 && isdigit((unsigned char)source_code[i])) {
                 fprintf(stderr, "Tokenizer error: Number literal too long.\n"); goto cleanup_failure;
            }
            tokens[count].type = NUMBER;
            strncpy(tokens[count].lexeme, source_code + start, len); tokens[count].lexeme[len] = '\0'; count++;
        } else if (c == '"') {
             size_t start = i + 1; size_t len = 0; i++;
             while (i < source_len && source_code[i] != '"' && source_code[i] != '\n') {
                 if (len >= MAX_LEXEME_LEN - 1) {
                     fprintf(stderr, "Tokenizer error: String literal too long.\n"); goto cleanup_failure;
                 } i++; len++;
             }
             if (i == source_len || source_code[i] != '"') {
                 fprintf(stderr, "Tokenizer error: Unterminated string literal.\n"); goto cleanup_failure;
             }
             i++; // consume closing quote
             tokens[count].type = STRING;
             strncpy(tokens[count].lexeme, source_code + start, len); tokens[count].lexeme[len] = '\0'; count++;
        } else {
            if (c == '<') {
                size_t lookahead_i = i + 1;
                if (lookahead_i < source_len && (isalpha((unsigned char)source_code[lookahead_i]) || source_code[lookahead_i] == '_')) {
                    lookahead_i++;
                    while (lookahead_i < source_len && (isalnum((unsigned char)source_code[lookahead_i]) || source_code[lookahead_i] == '_')) { lookahead_i++; }
                    if (lookahead_i < source_len && source_code[lookahead_i] == '>') {
                        size_t start = i; size_t len = (lookahead_i + 1) - start;
                        if (len >= MAX_LEXEME_LEN) {
                            fprintf(stderr, "Tokenizer error: TYPE_IDENTIFIER too long.\n"); goto cleanup_failure;
                        }
                        tokens[count].type = TYPE;
                        strncpy(tokens[count].lexeme, source_code + start, len);
                        tokens[count].lexeme[len] = '\0';
                        count++;
                        i = lookahead_i + 1;
                        continue;
                    }
                }
            }
            TokenType ttype = UNKNOWN;
            if (i + 2 < source_len) {
                ttype = identify_three_char_token(source_code[i], source_code[i+1], source_code[i+2]);
                if (ttype != UNKNOWN) {
                    snprintf(tokens[count].lexeme, 4, "%c%c%c", source_code[i], source_code[i+1], source_code[i+2]);
                    tokens[count].type = ttype; count++; i += 3; continue;
                }
            }
            if (i + 1 < source_len) {
                ttype = identify_two_char_token(source_code[i], source_code[i+1]);
                if (ttype != UNKNOWN) {
                    snprintf(tokens[count].lexeme, 3, "%c%c", source_code[i], source_code[i+1]);
                    tokens[count].type = ttype; count++; i += 2; continue;
                }
            }
            ttype = identify_single_char_token(c);
            if (ttype == UNKNOWN) {
                fprintf(stderr, "Tokenizer error: Unknown character encountered: '%c'\n", c);
                goto cleanup_failure;
            }
            tokens[count].type = ttype;
            tokens[count].lexeme[0] = c; tokens[count].lexeme[1] = '\0';
            count++; i++;
        }
    }

    if (state == STATE_IN_FMT_STRING) {
        fprintf(stderr, "Tokenizer error: Unterminated formatted string at end of file.\n");
        goto cleanup_failure;
    }
    if (count == 0) { free(tokens); gy->tokens = NULL; return true; } // Not a failure, just no tokens.
    
    if (count < capacity) {
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

// --- Stub Functions ---

void parse(Graveyard *gy) { (void)gy; printf("Parsing source...\n\n"); }
void execute(Graveyard *gy) { (void)gy; printf("Executing source...\n\n"); }

// --- Main Application Logic ---

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: graveyard <mode> <source file>\n");
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

    if (strcmp(gy->mode, "--preprocess") == 0 || strcmp(gy->mode, "-pre") == 0) {
        printf("Loaded source file (%ld bytes)\n", source_length);
        if (preprocess(gy)) {
            printf("Preprocessed Source:\n%s\n", gy->preprocessed_source);
        } else {
            success = false;
        }
    } else if (strcmp(gy->mode, "--tokenize") == 0 || strcmp(gy->mode, "-t") == 0) {
        printf("Loaded source file (%ld bytes)\n", source_length);
        if (!preprocess(gy)) {
            success = false;
        } else if (!tokenize(gy)) {
            success = false;
        } else {
            printf("Tokenization successful. Found %zu tokens.\n", gy->token_count);
            for (size_t i = 0; i < gy->token_count; i++) {
                printf("Token %zu: Type=%d, Lexeme='%s'\n", i, gy->tokens[i].type, gy->tokens[i].lexeme);
            }
        }
    } else {
        fprintf(stderr, "Unknown mode: %s\n", gy->mode);
        success = false;
    }
    
    graveyard_free(gy);
    return success ? 0 : 1;
}

// --- Helper Function Implementations ---

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