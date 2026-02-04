//
// Created by Omeed on 2/3/26.
//

#include "errorhandler.h"

static char* exception_to_string(Exception exception) {
    switch (exception) {
        case NULL_POINTER: return "NullPointerException";
        case FILE_ERROR: return "FileException";
        case MEMORY_ALLOC_ERROR: return "MemoryAllocationException";
        case INDEX_OUT_OF_BOUNDS: return "IndexOutOfBoundsException";
        default: return "UnknownException";
    }
}

static void print_stack_trace() {
    void* array[10];
    int size = backtrace(array, 10);                  // get void* addresses
    char** symbols = backtrace_symbols(array + 1, size - 1); // don't include throw exception

    if (symbols == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Stack trace (most recent call first):\n");
    for (int i = 0; i < size; i++) {
        fprintf(stderr, "%s\n", symbols[i]);
    }

    free(symbols);
}

void throw_exception(const Exception exception, const char* message) {
    fprintf(stderr, "%s: %s\nStack Trace:\n", exception_to_string(exception), message);
    print_stack_trace();
    fflush(stderr);
    exit(EXIT_FAILURE);
}

