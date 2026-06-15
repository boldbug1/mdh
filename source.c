#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SIZE 4096

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

void processLine(char *line_start, char *end);

void cleanup(){
    if (tokens != NULL) {
        for(int i = 0; i < tokenCount; i++){
            free(tokens[i].text);
        }
        free(tokens);
        tokens = NULL;
    }
}

void scanTokens(char md[])
{
   
    tokens = malloc(tokenCapacity * sizeof(Token));
    if (!tokens) {
        printf("Initial memory allocation failed\n");
        return;
    }

    char *line_start = md;
    char *p = md;

    while (*p != '\0')
    {
        if (*p == '\n')
        {
            processLine(line_start, p);
            line_start = p + 1;
        }
        p++;
    }

    if (p != line_start)
    {
        processLine(line_start, p);
    }
}

int parsingCodeBlock = 0;

void processLine(char *line_start, char *end)
{
    char *content_start = line_start;
    Token t;
    int line_len = end - line_start;

  
    if (line_len >= 3 && strncmp(line_start, "```", 3) == 0) {
        t.type = CODE_BLOCK;
        content_start = line_start + 3;
        parsingCodeBlock = !parsingCodeBlock; // Toggle the state
    } 
  
    else if (parsingCodeBlock) {
        t.type = PARAGRAPH; 
    } 
    
    else if (line_len >= 4 && strncmp(line_start, "### ", 4) == 0) {
        t.type = HASH3;
        content_start = line_start + 4;
    } else if (line_len >= 3 && strncmp(line_start, "## ", 3) == 0) {
        t.type = HASH2;
        content_start = line_start + 3;
    } else if (line_len >= 2 && strncmp(line_start, "# ", 2) == 0) {
        t.type = HASH;
        content_start = line_start + 2;
    } else if (line_len >= 2 && strncmp(line_start, "- ", 2) == 0) {
        t.type = BULLET_LIST;
        content_start = line_start + 2;
    } else {
        t.type = PARAGRAPH;
    }

    int len = end - content_start;
    if (len < 0) len = 0;

    t.text = malloc(len + 1);
    if (!t.text) {
        printf("malloc failed\n");
        return;
    }

    if (len > 0) {
        strncpy(t.text, content_start, len);
        t.text[len] = '\0';
    } else {
        t.text[0] = '\0';
    }

   
    if (tokenCount == tokenCapacity) {
        tokenCapacity *= 2;
        Token *temp = realloc(tokens, tokenCapacity * sizeof(Token));
        if (!temp) {
            printf("realloc failed\n");
            free(t.text);
            return;
        }
        tokens = temp;
    }

    tokens[tokenCount++] = t;
}

void writeHTML()
{
    int inList = 0;
    int inCode = 0;
    FILE *fp = fopen("Index.html", "w");
    if (!fp)
    {
        printf("Error while creating file\n");
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
            
            if (inCode) {
                fprintf(fp, "%s\n", tokens[i].text);
            } else {
                fprintf(fp, "<p>%s</p>\n", tokens[i].text);
            }
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
            if (!inCode) {
                fprintf(fp, "<pre><code>\n");
                inCode = 1;
            } else {
                fprintf(fp, "</code></pre>\n");
                inCode = 0;
            }
        }
    }

    if (inList) fprintf(fp, "</ul>\n");
    if (inCode) fprintf(fp, "</code></pre>\n");

    fclose(fp); 
    printf("HTML generated successfully as 'Index.html'.\n");
}

int main()
{
    printf("Welcome to mdrender : \n");
    char md[MAX_SIZE];

    printf("Enter markdown (Press Ctrl + D / Ctrl + Z to finish) :\n");
    int c;
    int i = 0;
    
   
    while ((c = getchar()) != EOF && i < MAX_SIZE - 1)
    {
        md[i++] = c;
    }
    md[i] = '\0';

    scanTokens(md);

    printf("\nWould you like to download html? (y/n) : ");
    char chs;
    
    
    scanf(" %c", &chs); 
    
    if (chs == 'y' || chs == 'Y')
    {
        writeHTML();
    }
    else if (chs == 'n' || chs == 'N')
    {
        printf("Exiting without saving.\n");
    }
    else
    {
        printf("Invalid input. Returning...\n");
    }

    cleanup();
    return 0;
}