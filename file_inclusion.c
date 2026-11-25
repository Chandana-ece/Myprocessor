
#include "header.h"

static int find_header(const char *current_file,
                       const char *header,
                       char *path,
                       size_t path_size)
{
    FILE *fp;
    const char *slash;
    size_t dir_len;
    size_t header_len;

    slash = strrchr(current_file, '/');
    dir_len = (slash != NULL) ? (size_t)(slash - current_file + 1) : 0;
    header_len = strlen(header);

    /* Search relative to current file */
    if (dir_len + header_len + 1 <= path_size)
    {
        memcpy(path, current_file, dir_len);
        memcpy(path + dir_len, header, header_len + 1);

        fp = fopen(path, "r");

        if (fp != NULL)
        {
            fclose(fp);
            return 1;
        }
    }

    /* Search current working directory */
    snprintf(path, path_size, "%s", header);

    fp = fopen(path, "r");

    if (fp != NULL)
    {
        fclose(fp);
        return 1;
    }

    return 0;
}

void process_file(const char *filename, FILE *output, int depth)
{
    FILE *input;
    char line[1024];
    char header[256];
    char path[512];
    char *start;
    char *end;
    char closing;
    size_t len;

    if (depth >= MAX_DEPTH)
    {
        fprintf(stderr, "Error: Maximum include depth reached\n");
        return;
    }

    input = fopen(filename, "r");

    if (input == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return;
    }

    while (fgets(line, sizeof(line), input) != NULL)
    {
        start = line;

        while (isspace((unsigned char)*start))
            start++;

        /* Check include directive */
        if (strncmp(start, "#include", 8) == 0 &&
            (isspace((unsigned char)start[8]) ||
             start[8] == '"' || start[8] == '<'))
        {
            start += 8;

            while (isspace((unsigned char)*start))
                start++;

            if (*start == '"' || *start == '<')
            {
                closing = (*start == '"') ? '"' : '>';
                start++;

                end = strchr(start, closing);

                if (end != NULL)
                {
                    len = (size_t)(end - start);

                    if (len >= sizeof(header))
                    {
                        fprintf(stderr, "Error: Header name too long\n");
                        continue;
                    }

                    memcpy(header, start, len);
                    header[len] = '\0';

                    /* System headers are left to the system compiler */
                    if (closing == '>')
                        continue;

                    if (find_header(filename, header,
                                    path, sizeof(path)))
                    {
                        process_file(path, output, depth + 1);
                    }
                    else
                    {
                        fprintf(stderr,
                                "Error: Header file %s not found\n",
                                header);
                    }

                    continue;
                }
            }
        }

        /* Remove comments */
        remove_comments(line);

        start = line;

        while (isspace((unsigned char)*start))
            start++;

        /* Register macro definitions */
        if (strncmp(start, "#define", 7) == 0 &&
            (start[7] == ' ' || start[7] == '\t'))
        {
            register_macro(start + 7);
            continue;
        }

        /* Expand macros */
        expand_macros(line);

        fputs(line, output);
    }

    fclose(input);
}

