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

    // Future state can be added here
    // AstNode *ast_root;
} Graveyard;


// --- Forward Declarations for Helper Functions ---

char *load(FILE *file, long *out_length);
char *entry(const char *source_code);
char *uncomment(const char *source_code);
char *unwhitespace(const char *source_code);
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
    // This function takes the raw source from gy->source_code,
    // runs it through the preprocessing steps, and stores the
    // final result in gy->preprocessed_source.
    
    char *cropped = entry(gy->source_code);
    if (!cropped) {
        fprintf(stderr, "Preprocessor error: Failed to find global scope.\n");
        return false;
    }

    char *uncommented = uncomment(cropped);
    free(cropped);
    if (!uncommented) {
        fprintf(stderr, "Preprocessor error: Failed to remove comments.\n");
        return false;
    }

    char *unwhitespaced = unwhitespace(uncommented);
    free(uncommented);
    if (!unwhitespaced) {
        fprintf(stderr, "Preprocessor error: Failed to remove whitespace.\n");
        return false;
    }
    
    gy->preprocessed_source = unwhitespaced;
    return true;
}

bool tokenize(Graveyard *gy) {
    const char* source_code = gy->preprocessed_source;
    size_t source_len = strlen(source_code); // Get length for safe bounds checking
    size_t capacity = 16;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    if (!tokens) { /* Malloc failure handling */
        perror("tokenize: malloc failed");
        return false;
    }

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
            // --- Logic for Operators, including robust <sometype> vs < check ---
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

            // Look ahead for three-character tokens
            if (i + 2 < source_len) {
                ttype = identify_three_char_token(source_code[i], source_code[i+1], source_code[i+2]);
                if (ttype != UNKNOWN) {
                    tokens[count].type = ttype;
                    snprintf(tokens[count].lexeme, 4, "%c%c%c", source_code[i], source_code[i+1], source_code[i+2]);
                    count++; i += 3; continue;
                }
            }
            // Look ahead for two-character tokens
            if (i + 1 < source_len) {
                ttype = identify_two_char_token(source_code[i], source_code[i+1]);
                if (ttype != UNKNOWN) {
                    tokens[count].type = ttype;
                    snprintf(tokens[count].lexeme, 3, "%c%c", source_code[i], source_code[i+1]);
                    count++; i += 2; continue;
                }
            }
            // Fallback to single-character tokens
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

    if (count == 0) { goto cleanup_failure; }
    
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

void parse(Graveyard *gy) {
    (void)gy; // Suppress unused parameter warning
    printf("Parsing source...\n\n");
}
void execute(Graveyard *gy) {
    (void)gy;
    printf("Executing source...\n\n");
}

// --- Main Application Logic ---

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: graveyard <mode> <source file>\n");
        // (Print modes...)
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
        if (preprocess(gy)) {
            printf("%s\n", gy->preprocessed_source);
        } else {
            fprintf(stderr, "Preprocessing failed.\n");
            success = false;
        }
    } else if (strcmp(gy->mode, "--tokenize") == 0 || strcmp(gy->mode, "-t") == 0) {
        if (!preprocess(gy)) {
            fprintf(stderr, "Preprocessing failed.\n");
            success = false;
        } else if (!tokenize(gy)) {
            fprintf(stderr, "Tokenization failed.\n");
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
// (These are simple and can be placed at the end)

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

char *entry(const char *source_code) {
    const char *start = strstr(source_code, "::{");
    if (!start) { return NULL; }
    start += 3;
    const char *end = strrchr(start, '}');
    if (!end) { return NULL; }
    size_t cropped_length = end - start;
    char *cropped_source_code = malloc(cropped_length + 1);
    if (!cropped_source_code) { return NULL; }
    strncpy(cropped_source_code, start, cropped_length);
    cropped_source_code[cropped_length] = '\0';
    return cropped_source_code;
}

char *uncomment(const char *source_code) {
    size_t len = strlen(source_code);
    char *clean = malloc(len + 1);
    if (!clean) { return NULL; }
    char *dst = clean;
    for (size_t i = 0; i < len;) {
        if (i + 1 < len && source_code[i] == '/' && source_code[i + 1] == '/') {
            i += 2;
            while (i < len && source_code[i] != '\n') i++;
        } else if (i + 1 < len && source_code[i] == '/' && source_code[i + 1] == '*') {
            i += 2;
            while (i + 1 < len && !(source_code[i] == '*' && source_code[i + 1] == '/')) i++;
            if (i + 1 < len) i += 2; else i = len;
        } else {
            *dst++ = source_code[i++];
        }
    }
    *dst = '\0';
    return clean;
}

char *unwhitespace(const char *source_code) {
    size_t len = strlen(source_code);
    char *clean = malloc(len + 1);
    if (!clean) { return NULL; }
    char *dst = clean;
    bool in_double_string = false;
    bool in_single_string = false;
    for (size_t i = 0; i < len; i++) {
        char c = source_code[i];
        if (c == '\\' && (in_double_string || in_single_string)) {
            *dst++ = c;
            if (i + 1 < len) { *dst++ = source_code[++i]; }
            continue;
        }
        if (c == '"' && !in_single_string) { *dst++ = c; in_double_string = !in_double_string; }
        else if (c == '\'' && !in_double_string) { *dst++ = c; in_single_string = !in_single_string; }
        else if (in_double_string || in_single_string || !isspace((unsigned char)c)) { *dst++ = c; }
    }
    *dst = '\0';
    return clean;
}

// --- Token Identification Helpers ---

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