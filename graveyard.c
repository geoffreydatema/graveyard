#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to read an entire file into a string
char *read_file(FILE *file, long *out_length) {
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    size_t read = fread(buffer, 1, length, file);
    buffer[read] = '\0';

    if (out_length) {
        *out_length = length;
    }

    return buffer;
}

char *entry(char *source_code) {
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

char *uncomment(const char *src) {
    size_t len = strlen(src);
    char *clean = malloc(len + 1);
    if (!clean) {
        perror("Memory allocation failed in uncomment");
        return NULL;
    }

    char *dst = clean;
    for (size_t i = 0; i < len;) {
        if (src[i] == '/' && src[i + 1] == '/') {
            i += 2;
            while (src[i] && src[i] != '\n') i++;
        } else if (src[i] == '/' && src[i + 1] == '*') {
            i += 2;
            while (src[i] && !(src[i] == '*' && src[i + 1] == '/')) i++;
            if (src[i]) i += 2;
        } else {
            *dst++ = src[i++];
        }
    }

    *dst = '\0';
    return clean;
}

char *preprocess(char *source_code) {
    printf("Preprocessing...\n");

    char *cropped = entry(source_code);
    if (!cropped) {
        fprintf(stderr, "Failed to find global scope.\n");
        return NULL;
    }
    printf("Cropped source:\n%s\n", cropped);

    char *uncommented = uncomment(cropped);
    free(cropped); // Free 'cropped' regardless of 'uncomment' success

    if (!uncommented) {
        // uncomment() should have printed an error via perror
        fprintf(stderr, "Failed to uncomment source.\n");
        return NULL; // Return NULL if uncommenting failed
    }

    printf("Uncommented source:\n%s\n", uncommented);
    return uncommented;
}

void tokenize(const char *source_code) {
    printf("Tokenizing source...\n");
    // TODO: Implement tokenizing logic
}

void parse(const char *source_code) {
    printf("Parsing source...\n");
    // TODO: Implement parsing logic
}

void execute(const char *source_code) {
    printf("Executing source...\n");
    // TODO: Implement execution logic
}

void compile_only(const char *source_code) {
    printf("Compiling source...\n");
    // TODO: Implement compilation logic
}

void monolith_debug(const char *source_code) {
    printf("Running monolithic debug mode...\n");
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

    // Check that file has .gy extension
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
    char *source_code = read_file(file, &source_length);
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
        tokenize(source_code);
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
