#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    buffer[read] = '\0'; // Null-terminate

    if (out_length) {
        *out_length = length;
    }

    return buffer;
}

// Stubbed interpreter stages
void preprocess(const char *source_code) {
    printf("Preprocessing source...\n");
    // TODO: Implement preprocessing logic
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
        preprocess(source_code);
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
