#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

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

void tokenize(const char *source_code) {
    printf("Tokenizing source...\n\n");
    
    printf("%s", source_code);
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
        free(preprocessed_source);

    } else if (strcmp(mode, "--tokenize") == 0 || strcmp(mode, "-t") == 0) {
        char *preprocessed_source = preprocess(source_code);
        if (!preprocessed_source) {
            fprintf(stderr, "Preprocessing failed.\n");
            free(source_code);
            return 1;
        }
        tokenize(preprocessed_source);
        free(preprocessed_source);

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
