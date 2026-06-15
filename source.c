#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 1000

typedef enum
{
    HASH,
    BULLET_LIST,
    CODE_BLOCK,
    HASH2,
    HASH3,
    PARAGRAPH
} TokenType;

typedef struct
{
    TokenType type;
    char *text;
} Token;

Token *tokens = NULL;
int tokenCount = 0;
int tokenCapacity = 8;

void scanTokens(char md[])
{
    char *line_start = md;
    char *p = md;

    while (*p != '\0')
    {
        if (*p == '\n')
        {
            char *content_start = line_start;
            int len;

            if (strncmp(line_start, "### ", 4) == 0)
            {
                content_start = line_start + 4;
            }
            else if (strncmp(line_start, "## ", 3) == 0)
            {
                content_start = line_start + 3;
            }
            else if (strncmp(line_start, "# ", 2) == 0)
            {
                content_start = line_start + 2;
            }
            else if (strncmp(line_start, "- ", 2) == 0)
            {
                content_start = line_start + 2;
            }
            else if (strncmp(line_start, "```", 3) == 0)
            {
                
            }

           
            Token t; 
            if (tokenCount == tokenCapacity)
            {
                tokenCapacity *= 2;
                Token *temp = realloc(tokens, tokenCapacity * sizeof(Token));
                if (!temp)
                {
                    printf("realloc failed\n");
                    return;
                }
                tokens = temp;
            }

            tokens[tokenCount++] = t;
            if (strncmp(line_start, "### ", 4) == 0)
            {
                t.type = HASH3;
            }
            else if (strncmp(line_start, "## ", 3) == 0)
            {
                t.type = HASH2;
            }
            else if (strncmp(line_start, "# ", 2) == 0)
            {
                t.type = HASH;
            }
            else if (strncmp(line_start, "- ", 2) == 0)
            {
                t.type = BULLET_LIST;
            }
            else if (strncmp(line_start, "```", 3) == 0)
            {
                t.type = CODE_BLOCK;
            }
            else
            {
                t.type = PARAGRAPH;
            }

            len = p - content_start;
            t.text = malloc(len + 1);
            if (len > 0)
            {
                strncpy(t.text, content_start, len);
                t.text[len] = '\0';
            }
            else
            {
                t.text[0] = '\0';
            }

            tokens[tokenCount++] = t;
            line_start = p + 1;
        }
        p++;
    }

    // handle last line if no trailing newline
    if (p != line_start)
    {
        char *content_start = line_start;
        Token t;

        if (strncmp(line_start, "### ", 4) == 0)
        {
            t.type = HASH3;
            content_start = line_start + 4;
        }
        else if (strncmp(line_start, "## ", 3) == 0)
        {
            t.type = HASH2;
            content_start = line_start + 3;
        }
        else if (strncmp(line_start, "# ", 2) == 0)
        {
            t.type = HASH;
            content_start = line_start + 2;
        }
        else if (strncmp(line_start, "- ", 2) == 0)
        {
            t.type = BULLET_LIST;
            content_start = line_start + 2;
        }
        else if (strncmp(line_start, "```", 3) == 0)
        {
            t.type = CODE_BLOCK;
        }
        else
        {
            t.type = PARAGRAPH;
        }

        int len = p - content_start;
        if (len > 0)
        {
            strncpy(t.text, content_start, len < 99 ? len : 99);
            t.text[len < 99 ? len : 99] = '\0';
        }
        else
        {
            t.text[0] = '\0';
        }

        tokens[tokenCount++] = t;
    }
}

void printTokens(Token tokens[])
{
    for (int i = 0; i < tokenCount; i++)
    {
        printf("Tokentype : %d , text : %s\n", tokens[i].type, tokens[i].text);
    }
}

void writeHTML()
{
    int inList = 0;
    FILE *fp;
    fp = fopen("Index.html", "w");
    if (!fp)
    {
        printf("Error while creating file");
        return;
    }
    for (int i = 0; i < tokenCount; i++)
    {
        if (tokens[i].type != BULLET_LIST && inList)
        {
            fprintf(fp, "</ul>\n");
            inList = 0;
        }
        if (tokens[i].type == HASH)
        {
            fprintf(fp, "<h1>%s</h1>\n", tokens[i].text);
        }
        else if (tokens[i].type == HASH2)
        {
            fprintf(fp, "<h2>%s</h2>\n", tokens[i].text);
        }
        else if (tokens[i].type == HASH3)
        {
            fprintf(fp, "<h3>%s</h3>\n", tokens[i].text);
        }
        else if (tokens[i].type == PARAGRAPH)
        {
            fprintf(fp, "<p>%s</p>\n", tokens[i].text);
        }
        else if (tokens[i].type == BULLET_LIST)
        {
            if (!inList)
            {
                inList = 1;
                fprintf(fp, "<ul>\n");
            }
            fprintf(fp, "<li>%s</li>\n", tokens[i].text);
        }
        else if (tokens[i].type == CODE_BLOCK)
        {
            fprintf(fp, "<code>%s</code>\n", tokens[i].text);
        }
    }

    if (inList)
    {
        fprintf(fp, "</ul>");
        inList = 0;
    }
}

int main()
{
    printf("Welcome to mdrender : \n");
    char md[1024];

    printf("Enter markdown (Press Ctrl + Z to finish) : ");
    int c;
    int i = 0;
    while ((c = getchar()) != EOF)
    {
        md[i++] = c;
    }
    md[i] = '\0';

    int size = strlen(md);

    scanTokens(md);

    printf("Would you like to download html? (y/n) : ");
    char chs;
    scanf("%c", &chs);
    if (chs == 'y')
    {
        writeHTML();
        return 0;
    }
    else if (chs == 'n')
    {
        return 0;
    }
    else
    {
        printf("Invalid input\n");
        printf("Returning...");
        return 1;
    }

    return 0;
}