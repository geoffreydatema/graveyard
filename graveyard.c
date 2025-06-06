#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LEXEME_LEN 64

typedef enum {
    IDENTIFIER,
    TYPE, //@!
    NUMBER,
    SEMICOLON,
    ASSIGNMENT,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    LEFTPARENTHESES,
    RIGHTPARENTHESES,
    EXPONENTIATION,
    EQUALITY,
    INEQUALITY,
    GREATERTHAN,
    LESSTHAN,
    GREATERTHANEQUAL,
    LESSTHANEQUAL,
    NOT,
    AND,
    OR,
    COMMA,
    TRUEVALUE,
    FALSEVALUE,
    NULLVALUE,
    STRING, //@!
    LEFTBRACE,
    RIGHTBRACE,
    PARAMETER,
    RETURN,
    QUESTIONMARK,
    COLON,
    WHILE,
    CONTINUE,
    BREAK,
    AT,
    FORMATTEDSTRING, //@!
    LEFTBRACKET,
    RIGHTBRACKET,
    ADDITIONASSIGNMENT,
    SUBTRACTIONASSIGNMENT,
    MULTIPLICATIONASSIGNMENT,
    DIVISIONASSIGNMENT,
    EXPONENTIATIONASSIGNMENT, //@!
    INCREMENT,
    DECREMENT,
    REFERENCE,
    PERIOD,
    NAMESPACE,
    PRINT,
    SCAN,
    RAISE, //@!
    CASTBOOLEAN,
    CASTINTEGER,
    CASTFLOAT,
    CASTSTRING,
    CASTARRAY,
    CASTHASHTABLE,
    TYPEOF,
    MODULO,
    FILEREAD, //@!
    FILEWRITE, //@!
    TIME,
    EXECUTE,
    CATCONSTANT,
    // OPEN OPERATOR
    // OPEN OPERATOR
    // OPEN OPERATOR
    // OPEN OPERATOR
    UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LEN];
} Token;

char *load(FILE *file, long *out_length) {
    if (out_length) {
        *out_length = 0;
    }

    if (!file) {
        fprintf(stderr, "load: Received NULL file pointer\n");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("load: fseek to SEEK_END failed");
        return NULL;
    }

    long length = ftell(file);
    if (length == -1L) {
        perror("load: ftell failed after seeking to end");
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("load: fseek to SEEK_SET failed");
        return NULL;
    }

    if (length < 0) {
        fprintf(stderr, "load: ftell returned negative file size unexpectedly.\n");
        return NULL;
    }
    if ((unsigned long)length >= SIZE_MAX) {
        fprintf(stderr, "load: File size (%ld bytes) is too large to allocate memory for.\n", length);
        return NULL;
    }

    char *buffer = malloc((size_t)length + 1);
    if (!buffer) {
        fprintf(stderr, "load: Memory allocation failed for %ld bytes\n", length + 1);
        return NULL;
    }

    size_t items_read = 0;
    if (length > 0) {
        items_read = fread(buffer, 1, (size_t)length, file);

        if (items_read < (size_t)length) {
            if (ferror(file)) {
                perror("load: fread encountered an error");
                free(buffer);
                return NULL;
            }
        }
    }

    buffer[items_read] = '\0';

    if (out_length) {
        *out_length = (long)items_read;
    }

    return buffer;
}

char *entry(const char *source_code) {
    char *start = strstr(source_code, "::{");
    if (!start) {
        fprintf(stderr, "Missing '::{' to open global scope.\n");
        return NULL;
    }

    start += 3;

    char *end = strrchr(start, '}');
    if (!end) {
        fprintf(stderr, "Missing closing '}' for global scope.\n");
        return NULL;
    }

    size_t cropped_length = end - start;
    char *cropped_source_code = malloc(cropped_length + 1);
    if (!cropped_source_code) {
        perror("Memory allocation failed");
        return NULL;
    }

    strncpy(cropped_source_code, start, cropped_length);
    cropped_source_code[cropped_length] = '\0';

    return cropped_source_code;
}

char *uncomment(const char *source_code) {
    size_t len = strlen(source_code);
    char *clean = malloc(len + 1);
    if (!clean) {
        perror("Memory allocation failed in uncomment");
        return NULL;
    }

    char *dst = clean;
    for (size_t i = 0; i < len;) {
        if (source_code[i] == '/' && source_code[i + 1] == '/') {
            i += 2;
            while (source_code[i] && source_code[i] != '\n') i++;
        } else if (source_code[i] == '/' && source_code[i + 1] == '*') {
            i += 2;
            while (source_code[i] && !(source_code[i] == '*' && source_code[i + 1] == '/')) i++;
            if (source_code[i]) i += 2;
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
    if (!clean) {
        perror("Memory allocation failed in unwhitespace");
        return NULL;
    }

    char *dst = clean;
    for (size_t i = 0; i < len; i++) {
        if (!isspace((unsigned char)source_code[i])) {
            *dst++ = source_code[i];
        }
    }

    *dst = '\0';
    return clean;
}

char *preprocess(const char *source_code) {
    printf("Preprocessing...\n");

    char *cropped = entry(source_code);
    if (!cropped) {
        fprintf(stderr, "Graveyard preprocessor failed to find global scope\n");
        return NULL;
    }
    // printf("Cropped source:\n%s\n", cropped);

    char *uncommented = uncomment(cropped);
    free(cropped);
    if (!uncommented) {
        fprintf(stderr, "Graveyard preprocessor failed to remove comments from source.\n");
        return NULL;
    }
    // printf("Uncommented source:\n%s\n", uncommented);

    char *unwhitespaced = unwhitespace(uncommented);
    free(uncommented);
    if (!unwhitespaced) {
        fprintf(stderr, "Graveyard preprocessor failed to remove whitespace from source\n");
        return NULL;
    }
    // printf("Unwhitespaced source:\n%s\n", unwhitespaced);

    return unwhitespaced;
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

TokenType identify_two_char_token(char first, char second) {
    if (first == '*' && second == '*') return EXPONENTIATION;
    if (first == '=' && second == '=') return EQUALITY;
    if (first == '!' && second == '=') return INEQUALITY;
    if (first == '>' && second == '=') return GREATERTHANEQUAL;
    if (first == '<' && second == '=') return LESSTHANEQUAL;
    if (first == '&' && second == '&') return AND;
    if (first == '|' && second == '|') return OR;
    if (first == '-' && second == '>') return RETURN;
    if (first == '+' && second == '=') return ADDITIONASSIGNMENT;
    if (first == '-' && second == '=') return SUBTRACTIONASSIGNMENT;
    if (first == '*' && second == '=') return MULTIPLICATIONASSIGNMENT;
    if (first == '/' && second == '=') return DIVISIONASSIGNMENT;
    if (first == '+' && second == '+') return INCREMENT;
    if (first == '-' && second == '-') return DECREMENT;
    if (first == ':' && second == ':') return NAMESPACE;
    if (first == '>' && second == '>') return PRINT;
    if (first == '<' && second == '<') return SCAN;
    if (first == '>' && second == 'b') return CASTBOOLEAN;
    if (first == '>' && second == 'i') return CASTINTEGER;
    if (first == '>' && second == 'f') return CASTFLOAT;
    if (first == '>' && second == 's') return CASTSTRING;
    if (first == '>' && second == 'a') return CASTARRAY;
    if (first == '>' && second == 'h') return CASTHASHTABLE;
    if (first == '@' && second == '@') return TYPEOF;
    if (first == '/' && second == '%') return MODULO;
    if (first == ':' && second == '@') return TIME;
    if (first == ':' && second == '=') return EXECUTE;
    if (first == ':' && second == '3') return CATCONSTANT;
    return UNKNOWN;
}

Token *tokenize(const char *source_code, size_t *out_token_count) {
    size_t capacity = 16;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));

    if (!tokens) {
        perror("tokenize: malloc failed for initial tokens array");
        if (out_token_count) *out_token_count = 0;
        return NULL;
    }
    printf("Allocated tokens array with initial capacity: %zu\n", capacity);

    size_t i = 0;
    while (source_code[i] != '\0') {

        if (isspace((unsigned char)source_code[i])) {
            i++;
            continue;
        }

        char c = source_code[i];

        if (count == capacity) {
            size_t new_capacity = capacity * 2;
            Token *new_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!new_tokens) {
                perror("tokenize: realloc failed to grow tokens array");
                free(tokens);
                if (out_token_count) *out_token_count = 0;
                return NULL;
            }
            tokens = new_tokens;
            capacity = new_capacity;
            printf("Reallocated tokens array, new capacity: %zu\n", capacity);
        }

        if (isalpha((unsigned char)c) || c == '_') {
            // Parse identifier
            size_t start = i;
            size_t len = 0;
            while ((isalnum((unsigned char)source_code[i]) || source_code[i] == '_') && len < MAX_LEXEME_LEN - 1) {
                i++;
                len++;
            }

            if (len == MAX_LEXEME_LEN - 1 && (isalnum((unsigned char)source_code[i]) || source_code[i] == '_')) {
                fprintf(stderr, "Tokenizer error: Identifier too long starting at: %.10s...\n", source_code + start);
                free(tokens);
                if (out_token_count) *out_token_count = 0;
                return NULL;
            }

            tokens[count].type = IDENTIFIER;
            strncpy(tokens[count].lexeme, source_code + start, len);
            tokens[count].lexeme[len] = '\0';
            count++;
        } else if (isdigit((unsigned char)c)) {
            size_t start = i;
            size_t len = 0;
            bool has_dot = false;

            // Parse digits before the decimal
            while (isdigit((unsigned char)source_code[i]) && len < MAX_LEXEME_LEN - 1) {
                i++;
                len++;
            }

            // Check for decimal point and digits after it
            if (source_code[i] == '.' && isdigit((unsigned char)source_code[i + 1])) {
                has_dot = true;
                i++; // consume the dot
                len++;

                while (isdigit((unsigned char)source_code[i]) && len < MAX_LEXEME_LEN - 1) {
                    i++;
                    len++;
                }
            }

            if (len == MAX_LEXEME_LEN - 1 && isdigit((unsigned char)source_code[i])) {
                fprintf(stderr, "Tokenizer error: Number literal too long starting at: %.10s...\n", source_code + start);
                free(tokens);
                if (out_token_count) *out_token_count = 0;
                return NULL;
            }

            tokens[count].type = NUMBER;
            strncpy(tokens[count].lexeme, source_code + start, len);
            tokens[count].lexeme[len] = '\0';
            count++;
        } else {
            TokenType ttype = UNKNOWN;

            // Look ahead for two-character tokens
            if (source_code[i + 1] != '\0') {
                ttype = identify_two_char_token(source_code[i], source_code[i + 1]);
                if (ttype != UNKNOWN) {
                    tokens[count].type = ttype;
                    tokens[count].lexeme[0] = source_code[i];
                    tokens[count].lexeme[1] = source_code[i + 1];
                    tokens[count].lexeme[2] = '\0';
                    count++;
                    i += 2;
                    continue;
                }
            }

            // Fallback to single-character tokens
            ttype = identify_single_char_token(source_code[i]);
            if (ttype == UNKNOWN) {
                fprintf(stderr, "Tokenizer error: Unknown character encountered: '%c'\n", source_code[i]);
                free(tokens);
                if (out_token_count) *out_token_count = 0;
                return NULL;
            }

            tokens[count].type = ttype;
            tokens[count].lexeme[0] = source_code[i];
            tokens[count].lexeme[1] = '\0';
            count++;
            i++;
        }

    }

    if (count == 0) {
        free(tokens);
        if (out_token_count) *out_token_count = 0;
        return NULL;
    }

    // Attempt to shrink the array to the actual size if smaller than capacity
    if (count < capacity) {
        Token *shrunk_tokens = realloc(tokens, count * sizeof(Token));
        if (shrunk_tokens == NULL) {
            perror("tokenize: realloc failed to shrink tokens array (continuing with larger array)");
        } else {
            tokens = shrunk_tokens;
        }
    }

    if (out_token_count) *out_token_count = count;
    return tokens;
}

void parse(const char *source_code) {
    printf("Parsing source...\n\n");
    // TODO: Implement parsing logic
}

void execute(const char *source_code) {
    printf("Executing source...\n\n");
    // TODO: Implement execution logic
}

void compile_only(const char *source_code) {
    printf("Compiling source...\n\n");
    // TODO: Implement compilation logic
}

void monolith_debug(const char *source_code) {
    printf("Running monolithic debug mode...\n\n");
    // TODO: Implement monolith debug mode
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: graveyard <mode> <source file>\n");
        fprintf(stderr, "Modes:\n");
        fprintf(stderr, "  --preprocess, -pre   Preprocess only\n");
        fprintf(stderr, "  --tokenize, -t       Tokenize only\n");
        fprintf(stderr, "  --parse, -p          Parse only\n");
        fprintf(stderr, "  --execute, -e        Execute\n");
        fprintf(stderr, "  --compile, -c        Compile only\n");
        fprintf(stderr, "  --monolith, -m       Debugging monolith\n");
        return 1;
    }

    char *mode = argv[1];
    char *filename = argv[2];

    const char *ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".gy") != 0) {
        fprintf(stderr, "Error: %s is not Graveyard source code, please use the .gy extension\n", filename);
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening source file");
        return 1;
    }

    long source_length = 0;
    char *source_code = load(file, &source_length);
    fclose(file);
    if (!source_code) {
        return 1;
    }

    // Dispatch mode
    if (strcmp(mode, "--preprocess") == 0 || strcmp(mode, "-pre") == 0) {
        printf("Loaded source file (%ld bytes)\n", source_length);
        char *preprocessed_source = preprocess(source_code);
        if (!preprocessed_source) {
            fprintf(stderr, "Preprocessing failed.\n");
            free(source_code);
            return 1;
        }
        printf("%s", preprocessed_source);
        free(preprocessed_source);

    } else if (strcmp(mode, "--tokenize") == 0 || strcmp(mode, "-t") == 0) {
        char *preprocessed_source = preprocess(source_code);
        if (!preprocessed_source) {
            fprintf(stderr, "Preprocessing failed.\n");
            free(source_code);
            return 1;
        }

        size_t token_count = 0;
        Token *tokens = tokenize(preprocessed_source, &token_count);
        free(preprocessed_source);

        if (!tokens) {
            fprintf(stderr, "Tokenization failed.\n");
            free(source_code);
            return 1;
        }

        // For demonstration: print tokens
        for (size_t i = 0; i < token_count; i++) {
            printf("Token %zu: Type=%d, Lexeme='%s'\n", i, tokens[i].type, tokens[i].lexeme);
        }

        free(tokens);

    } else if (strcmp(mode, "--parse") == 0 || strcmp(mode, "-p") == 0) {
        parse(source_code);
    } else if (strcmp(mode, "--execute") == 0 || strcmp(mode, "-e") == 0) {
        execute(source_code);
    } else if (strcmp(mode, "--compile") == 0 || strcmp(mode, "-c") == 0) {
        compile_only(source_code);
    } else if (strcmp(mode, "--monolith") == 0 || strcmp(mode, "-m") == 0) {
        monolith_debug(source_code);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", mode);
        free(source_code);
        return 1;
    }

    free(source_code);
    return 0;
}
