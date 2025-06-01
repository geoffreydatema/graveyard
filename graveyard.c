#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    // enforce correct usage
    if (argc != 2) {
        fprintf(stderr, "Usage: <mode> <graveyard source>");
        return 1;
    }

    // get mode
    if (strcmp(argv[1], "--preprocess") == 0) {
        printf("preprocess only\n");
    } else if (strcmp(argv[1], "--tokenize") == 0) {
        printf("tokenize only\n");
    } else if (strcmp(argv[1], "--parse") == 0) {
        printf("parse only\n");
    } else if (strcmp(argv[1], "--monolith") == 0) {
        printf("debugging monolith\n");
    } else if (strcmp(argv[1], "--execute") == 0) {
        printf("execute\n");
    } else if (strcmp(argv[1], "--compile") == 0) {
        printf("compile only\n");
    } else {
        fprintf(stderr, "Unknown mode");
    }

    return 0;
}