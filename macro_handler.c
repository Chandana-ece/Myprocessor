
#include "header.h"

#define MAX_MACROS 100
#define NAME_SIZE 64
#define VALUE_SIZE 512
#define LINE_SIZE 1024

static Macro macros[MAX_MACROS];
static int macro_count = 0;

void register_macro(char *definition)
{
    int i = 0, j = 0;
    int k;

    while (definition[i] == ' ' || definition[i] == '\t')
        i++;

    /* Read macro name */
    while ((isalnum((unsigned char)definition[i]) ||
            definition[i] == '_') &&
           j < NAME_SIZE - 1)
    {
        macros[macro_count].name[j++] = definition[i++];
    }

    macros[macro_count].name[j] = '\0';

    if (j == 0 || macro_count >= MAX_MACROS)
        return;

    macros[macro_count].is_function = 0;
    macros[macro_count].argument[0] = '\0';

    /* Check for function-like macro */
    if (definition[i] == '(')
    {
        macros[macro_count].is_function = 1;
        i++;

        while (definition[i] == ' ' || definition[i] == '\t')
            i++;

        j = 0;

        while ((isalnum((unsigned char)definition[i]) ||
                definition[i] == '_') &&
               j < NAME_SIZE - 1)
        {
            macros[macro_count].argument[j++] = definition[i++];
        }

        macros[macro_count].argument[j] = '\0';

        while (definition[i] == ' ' || definition[i] == '\t')
            i++;

        if (definition[i] != ')')
            return;

        i++;
    }

    while (definition[i] == ' ' || definition[i] == '\t')
        i++;

    /* Store replacement value */
    j = 0;

    while (definition[i] != '\0' &&
           definition[i] != '\n' &&
           j < VALUE_SIZE - 1)
    {
        macros[macro_count].value[j++] = definition[i++];
    }

    macros[macro_count].value[j] = '\0';

    /* Remove extra spaces at the end */
    for (k = j - 1; k >= 0; k--)
    {
        if (macros[macro_count].value[k] == ' ' ||
            macros[macro_count].value[k] == '\t')
            macros[macro_count].value[k] = '\0';
        else
            break;
    }

    macro_count++;
}

/* Replace one occurrence of a function-like macro */
static void replace_argument(char *replacement,
                             const char *argument,
                             const char *actual)
{
    char result[VALUE_SIZE];
    int i = 0, j = 0;
    int len = strlen(argument);

    while (replacement[i] != '\0' && j < VALUE_SIZE - 1)
    {
        if (len > 0 &&
            strncmp(&replacement[i], argument, len) == 0 &&
            (i == 0 ||
             (!isalnum((unsigned char)replacement[i - 1]) &&
              replacement[i - 1] != '_')) &&
            (!isalnum((unsigned char)replacement[i + len]) &&
             replacement[i + len] != '_'))
        {
            int k = 0;

            while (actual[k] != '\0' && j < VALUE_SIZE - 1)
                result[j++] = actual[k++];

            i += len;
        }
        else
        {
            result[j++] = replacement[i++];
        }
    }

    result[j] = '\0';
    strcpy(replacement, result);
}

void expand_macros(char *line)
{
    char result[LINE_SIZE];
    char replacement[VALUE_SIZE];
    int pass, i, j, k;
    int found;

    /* Multiple passes allow simple nested object macros */
    for (pass = 0; pass < 5; pass++)
    {
        i = 0;
        j = 0;
        found = 0;

        while (line[i] != '\0' && j < LINE_SIZE - 1)
        {
            /* Do not expand inside strings */
            if (line[i] == '"')
            {
                result[j++] = line[i++];

                while (line[i] != '\0' && j < LINE_SIZE - 1)
                {
                    result[j++] = line[i];

                    if (line[i] == '\\' && line[i + 1] != '\0')
                    {
                        i++;
                        result[j++] = line[i++];
                        continue;
                    }

                    if (line[i++] == '"')
                        break;
                }
                continue;
            }

            /* Do not expand inside character literals */
            if (line[i] == '\'')
            {
                result[j++] = line[i++];

                while (line[i] != '\0' && j < LINE_SIZE - 1)
                {
                    result[j++] = line[i];

                    if (line[i] == '\\' && line[i + 1] != '\0')
                    {
                        i++;
                        result[j++] = line[i++];
                        continue;
                    }

                    if (line[i++] == '\'')
                        break;
                }
                continue;
            }

            /* Read an identifier */
            if (isalpha((unsigned char)line[i]) || line[i] == '_')
            {
                char word[NAME_SIZE];
                int start = i;
                int length = 0;

                while ((isalnum((unsigned char)line[i]) ||
                        line[i] == '_') &&
                       length < NAME_SIZE - 1)
                {
                    word[length++] = line[i++];
                }

                word[length] = '\0';

                for (k = 0; k < macro_count; k++)
                {
                    if (strcmp(word, macros[k].name) == 0)
                    {
                        if (macros[k].is_function)
                        {
                            int p = i;
                            int a = 0;
                            char actual[VALUE_SIZE];

                            while (line[p] == ' ' || line[p] == '\t')
                                p++;

                            if (line[p] != '(')
                                break;

                            p++;

                            while (line[p] == ' ' || line[p] == '\t')
                                p++;

                            while (line[p] != '\0' &&
                                   line[p] != ')' &&
                                   a < VALUE_SIZE - 1)
                            {
                                actual[a++] = line[p++];
                            }

                            if (line[p] != ')')
                                break;

                            actual[a] = '\0';

                            strcpy(replacement, macros[k].value);

                            replace_argument(replacement,
                                             macros[k].argument,
                                             actual);

                            i = p + 1;
                        }
                        else
                        {
                            strcpy(replacement, macros[k].value);
                        }

                        for (int r = 0;
                             replacement[r] != '\0' &&
                             j < LINE_SIZE - 1;
                             r++)
                        {
                            result[j++] = replacement[r];
                        }

                        found = 1;
                        break;
                    }
                }

                if (k == macro_count)
                {
                    for (int r = start; r < i && j < LINE_SIZE - 1; r++)
                        result[j++] = line[r];
                }
            }
            else
            {
                result[j++] = line[i++];
            }
        }

        result[j] = '\0';
        strcpy(line, result);

        if (!found)
            break;
    }
}

