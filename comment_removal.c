
#include "header.h"

void remove_comments(char *str)
{
    int i = 0, j = 0;
    int state = 0;

    while (str[i] != '\0')
    {
        /* Normal code */
        if (state == 0)
        {
            if (str[i] == '/' && str[i + 1] == '/')
            {
                state = 1;
                i += 2;
            }
            else if (str[i] == '/' && str[i + 1] == '*')
            {
                state = 2;
                i += 2;
            }
            else if (str[i] == '"')
            {
                str[j++] = str[i++];
                state = 3;
            }
            else if (str[i] == '\'')
            {
                str[j++] = str[i++];
                state = 4;
            }
            else
            {
                str[j++] = str[i++];
            }
        }

        /* Single-line comment */
        else if (state == 1)
        {
            if (str[i] == '\n')
            {
                str[j++] = '\n';
                state = 0;
            }
            i++;
        }

        /* Multi-line comment */
        else if (state == 2)
        {
            if (str[i] == '*' && str[i + 1] == '/')
            {
                state = 0;
                i += 2;
            }
            else
            {
                if (str[i] == '\n')
                    str[j++] = '\n';

                i++;
            }
        }

        /* Double-quoted string */
        else if (state == 3)
        {
            str[j++] = str[i];

            if (str[i] == '\\' && str[i + 1] != '\0')
                str[j++] = str[++i];
            else if (str[i] == '"')
                state = 0;

            i++;
        }

        /* Single-quoted character */
        else if (state == 4)
        {
            str[j++] = str[i];

            if (str[i] == '\\' && str[i + 1] != '\0')
                str[j++] = str[++i];
            else if (str[i] == '\'')
                state = 0;

            i++;
        }
    }

    str[j] = '\0';
}


