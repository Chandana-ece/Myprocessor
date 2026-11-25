
#include "header.h"

int main(int argc, char *argv[])
{
    FILE *output;
    char output_filename[256];
    char *dot;

    if (argc != 2)
    {
        printf("Usage: %s inputfile.c\n", argv[0]);
        return 1;
    }

    snprintf(output_filename, sizeof(output_filename),
             "%s", argv[1]);

    dot = strrchr(output_filename, '.');

    if (dot != NULL)
        strcpy(dot, ".i");
    else
        strcat(output_filename, ".i");

    output = fopen(output_filename, "w");

    if (output == NULL)
    {
        perror("Output file error");
        return 1;
    }

    process_file(argv[1], output, 0);

    fclose(output);

    printf("Preprocessing completed.\n");
    printf("Output file created: %s\n", output_filename);

    return 0;
}

