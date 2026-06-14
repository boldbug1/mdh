#include <stdio.h>
#include <string.h>
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
    char text[100];
} Token;

Token tokens[MAX_SIZE];
int tokenCount = 0;

void scanTokens(char md[])
{
    int i = 0;
    while (md[i] != '\0')
    {
        if (md[i] == '\n')
        {
            i++;
            continue;
        }
        if (md[i] == '#')
        {
            tokens[tokenCount].type = HASH;
            i++;
            if (md[i] == ' ')
                i++;
            int c = 0;
            while (md[i] != '\n' && md[i] != '\0')
            {
                tokens[tokenCount].text[c++] = md[i++];
            }
            tokens[tokenCount].text[c] = '\0';
            tokenCount++;
        }
        else if (md[i] == '-' && md[i + 1] == ' ')
        {
            tokens[tokenCount].type = BULLET_LIST;
            int c = 0;
            i += 2;
            while (md[i] != '\n' && md[i] != '\0')
            {
                tokens[tokenCount].text[c++] = md[i++];
            }
            tokens[tokenCount].text[c] = '\0';
            tokenCount++;
        }
        else
        {
            tokens[tokenCount].type = PARAGRAPH;
            int c = 0;
            while (md[i] != '\n' && md[i] != '\0')
            {
                tokens[tokenCount].text[c++] = md[i++];
            }
            tokens[tokenCount].text[c] = '\0';
            tokenCount++;
        }
    }
}

void printTokens(Token tokens[])
{
    for (int i = 0; i < tokenCount; i++)
    {
        printf("Tokentype : %d , text : %s\n", tokens[i].type, tokens[i].text);
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
    printTokens(tokens);

    return 0;
}