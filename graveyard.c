#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LEXEME_LEN 65

// --- Type Definitions ---

typedef enum {
    IDENTIFIER, TYPE, NUMBER, SEMICOLON, ASSIGNMENT, ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION, EXPONENTIATION, LEFTPARENTHESES, RIGHTPARENTHESES, EQUALITY, INEQUALITY, GREATERTHAN, LESSTHAN, GREATERTHANEQUAL, LESSTHANEQUAL, NOT, AND, OR, XOR, COMMA, TRUEVALUE, FALSEVALUE, NULLVALUE, STRING, LEFTBRACE, RIGHTBRACE, PARAMETER, RETURN, QUESTIONMARK, COLON, WHILE, CONTINUE, BREAK, AT, FORMATTEDSTRING, LEFTBRACKET, RIGHTBRACKET, ADDITIONASSIGNMENT, SUBTRACTIONASSIGNMENT, MULTIPLICATIONASSIGNMENT, DIVISIONASSIGNMENT, EXPONENTIATIONASSIGNMENT, INCREMENT, DECREMENT, REFERENCE, PERIOD, NAMESPACE, PRINT, SCAN, RAISE, CASTBOOLEAN, CASTINTEGER, CASTFLOAT, CASTSTRING, CASTARRAY, CASTHASHTABLE, TYPEOF, MODULO, FILEREAD, FILEWRITE, TIME, EXECUTE, CATCONSTANT, NULLCOALESCE, LENGTH, PLACEHOLDER, UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
    int line;
    int column;
} Token;

// --- Main Interpreter Struct ---

typedef struct {
    const char *mode;
    const char *filename;
    char *source_code;
    Token *tokens;
    size_t token_count;
} Graveyard;

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
    return gy;
}

void graveyard_free(Graveyard *gy) {
    if (!gy) return;
    free(gy->source_code);
    free(gy->tokens);
    free(gy);
}

// --- Core Logic Functions ---

bool tokenize(Graveyard *gy) {
    // 1. Find boundaries first
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

    if (count == 0 && (end_ptr - start_ptr) > 0) { /* No tokens generated, could be an issue */ }
    
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

    if (strcmp(gy->mode, "--tokenize") == 0 || strcmp(gy->mode, "-t") == 0) {
        printf("Loaded source file (%ld bytes)\n", source_length);
        
        if (!tokenize(gy)) {
            fprintf(stderr, "Compilation failed during tokenization.\n");
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
    } else {
        fprintf(stderr, "Mode '%s' is not supported. Try '--tokenize'.\n", gy->mode);
        success = false;
    }
    
    graveyard_free(gy);
    return success ? 0 : 1;
}
