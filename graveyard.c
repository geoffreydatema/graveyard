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

int entry(char **source_ptr) {
    char *source = *source_ptr;

    // Find the starting token
    char *start = strstr(source, "::{");
    if (!start) {
        fprintf(stderr, "Missing '::{' to open global scope.\n");
        return 1;
    }

    // Move start to the content after "::{"
    start += 3;

    // Find the last '}' in the whole file
    char *last_brace = strrchr(start, '}');
    if (!last_brace) {
        fprintf(stderr, "Missing closing '}' for global scope.\n");
        return 1;
    }

    // Calculate length of content inside the global scope
    size_t new_len = last_brace - start;

    char *trimmed = malloc(new_len + 1);
    if (!trimmed) {
        perror("Memory allocation failed");
        return 1;
    }

    strncpy(trimmed, start, new_len);
    trimmed[new_len] = '\0';

    *source_ptr = trimmed;
    return 0;
}

void uncomment(char **source_ptr) {
    char *src = *source_ptr;
    size_t len = strlen(src);
    char *clean = malloc(len + 1);
    if (!clean) {
        perror("Memory allocation failed in uncomment");
        return;
    }

    char *dst = clean;
    for (size_t i = 0; i < len;) {
        if (src[i] == '/' && src[i + 1] == '/') {
            // Skip until newline
            i += 2;
            while (src[i] && src[i] != '\n') i++;
        } else if (src[i] == '/' && src[i + 1] == '*') {
            // Skip until closing */
            i += 2;
            while (src[i] && !(src[i] == '*' && src[i + 1] == '/')) i++;
            if (src[i]) i += 2; // skip the closing */
        } else {
            *dst++ = src[i++];
        }
    }

    *dst = '\0';

    // Replace original
    free(*source_ptr);
    *source_ptr = clean;
}

void preprocess(char **source_ptr) {
    printf("Preprocessing...\n");

    char *original = *source_ptr;
    if (entry(source_ptr) != 0) {
        fprintf(stderr, "Failed to find global scope.\n");
        return;
    }

    if (*source_ptr != original) {
        free(original);
    }

    uncomment(source_ptr);

    printf("Trimmed source:\n%s\n", *source_ptr);
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
        preprocess(&source_code);
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
