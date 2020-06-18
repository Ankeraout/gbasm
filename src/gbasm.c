#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *inputFileName;
    const char *outputFileName;
    size_t romSize;
    bool computeChecksums;
} gbasm_settings_t;

static inline int parseArgs(const int argc, const char *argv[], gbasm_settings_t *gbasm_settings);

int main(const int argc, const char *argv[]) {
    gbasm_settings_t gbasm_settings;
    FILE *fd_in = NULL;
    FILE *fd_out = NULL;
    uint8_t *romBuffer = NULL;

    memset(&gbasm_settings, 0, sizeof(gbasm_settings_t));

    if(parseArgs(argc, argv, &gbasm_settings)) {
        fprintf(stderr, "An error occurred while reading arguments. Exiting...\n");
        return EXIT_FAILURE;
    }

    fd_in = fopen(gbasm_settings.inputFileName, "r");

    if(!fd_in) {
        fprintf(stderr, "Failed to open input file.\n");
        return EXIT_FAILURE;
    }

    romBuffer = malloc(gbasm_settings.romSize);

    if(!romBuffer) {
        fprintf(stderr, "Failed to allocate a ROM buffer.\n");
        return EXIT_FAILURE;
    }

    // Parse

    fclose(fd_in);

    fd_out = fopen(gbasm_settings.outputFileName, "w");

    if(!fd_out) {
        fprintf(stderr, "Failed to open output file.\n");
        return EXIT_FAILURE;
    }

    fwrite(romBuffer, 1, gbasm_settings.romSize, fd_out);
    fclose(fd_out);

    return EXIT_SUCCESS;
}

static inline int parseArgs(const int argc, const char *argv[], gbasm_settings_t *gbasm_settings) {
    bool readingInputFileName = false;
    bool readingOutputFileName = false;
    bool readingRomSize = false;

    for(int i = 1; i < argc; i++) {
        if(readingInputFileName) {
            gbasm_settings->inputFileName = argv[i];
            readingInputFileName = false;
        } else if(readingOutputFileName) {
            gbasm_settings->outputFileName = argv[i];
            readingOutputFileName = false;
        } else if(readingRomSize) {
            if(strcmp(argv[i], "32k") == 0) {
                gbasm_settings->romSize = 32768;
            } else if(strcmp(argv[i], "64k") == 0) {
                gbasm_settings->romSize = 65536;
            }
        } else {
            if(strcmp(argv[i], "-i") == 0) {
                readingInputFileName = true;
            } else if(strcmp(argv[i], "-o") == 0) {
                readingOutputFileName = true;
            } else if(strcmp(argv[i], "-r") == 0) {
                readingRomSize = true;
            } else if(strcmp(argv[i], "-c") == 0) {
                gbasm_settings->computeChecksums = true;
            } else {
                fprintf(stderr, "Unexpected argument \"%s\".\n", argv[i]);
                return 1;
            }
        }
    }

    if(readingInputFileName) {
        fprintf(stderr, "Expected a file name after \"-i\".\n");
        return 1;
    } else if(readingOutputFileName) {
        fprintf(stderr, "Expected a file name after \"-o\".\n");
        return 1;
    }

    if(gbasm_settings->inputFileName == NULL) {
        fprintf(stderr, "No input file provided.\n");
        return 1;
    } else if(gbasm_settings->outputFileName == NULL) {
        fprintf(stderr, "No output file name provided.\n");
        return 1;
    } else if(gbasm_settings->romSize == 0) {
        fprintf(stderr, "No ROM file size provided.\n");
        return 1;
    }

    return 0;
}
