//
// Created by Omeed on 2/3/26.
//

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
typedef enum Exception {
    NULL_POINTER,
    FILE_ERROR,
    MEMORY_ALLOC_ERROR,
    INDEX_OUT_OF_BOUNDS
} Exception;

//void print_stack_trace();

void throw_exception(const Exception exception, const char* message);

#endif //ERRORHANDLER_H
