

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DEPTH 10

typedef struct
{
    char name[64];
    char value[512];
    int is_function;
    char argument[64];
} Macro;

void remove_comments(char *str);
void register_macro(char *definition);
void expand_macros(char *line);
void process_file(const char *filename, FILE *output, int depth);

#endif

