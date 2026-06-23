#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    HASH,
    BULLET_LIST,
    CODE_BLOCK,
    HASH2,
    HASH3,
    PARAGRAPH,
    BLOCK_QUOTE,
    INLINE_BOLD,
    INLINE_ITALIC
} TokenType;

typedef struct {
    TokenType type;
    char *text;
} Token;

Token *tokens = NULL;
int tokenCount = 0;
int tokenCapacity = 8;

void processLine(char *line_start, char *end);
void writeHTML(char *filename);
void cleanup();
void scanTokens(char md[]);
void renderInline(FILE *fp, const char *text);
int parsingCodeBlock = 0;

int main(int argc, char *argv[])
{
    FILE *file;

    if (argc < 2 || argc > 3) {
        printf("Usage : %s <filename> <output.html>\nLeave output.html for default index.html\n", argv[0]);
        return 1;
    }
    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }
    char *output_filename = "index.html";

    if(argc == 3){
        output_filename = argv[2];
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char *md = malloc(size + 1);
    if (!md) {
        return 1;
    }
    size_t bytes_read = fread(md, 1, size, file);
    md[bytes_read] = '\0';
    scanTokens(md);
    writeHTML(output_filename);
    cleanup();
    fclose(file);
    free(md);
    return 0;
}

void renderInline(FILE *fp, const char *text)
{
    const char *p = text;

    while (*p != '\0') {

        /* ── code span: `...` – pass through verbatim ── */
        if (*p == '`') {
            fputc('`', fp);
            p++;
            while (*p != '\0' && *p != '`') {
                fputc(*p, fp);
                p++;
            }
            if (*p == '`') { fputc('`', fp); p++; }
            continue;
        }

        /* ── bold: **...** ── */
        if (p[0] == '*' && p[1] == '*') {
            const char *close = strstr(p + 2, "**");
            if (close) {
                fprintf(fp, "<strong>");
                /* recurse so bold content can itself be italic */
                int inner_len = close - (p + 2);
                char *inner = malloc(inner_len + 1);
                if (inner) {
                    strncpy(inner, p + 2, inner_len);
                    inner[inner_len] = '\0';
                    renderInline(fp, inner);
                    free(inner);
                }
                fprintf(fp, "</strong>");
                p = close + 2;
                continue;
            }
        }

        /* ── italic: *...* ── */
        if (p[0] == '*') {
            const char *close = strchr(p + 1, '*');
            if (close) {
                fprintf(fp, "<em>");
                int inner_len = close - (p + 1);
                char *inner = malloc(inner_len + 1);
                if (inner) {
                    strncpy(inner, p + 1, inner_len);
                    inner[inner_len] = '\0';
                    renderInline(fp, inner);
                    free(inner);
                }
                fprintf(fp, "</em>");
                p = close + 1;
                continue;
            }
        }

        /* ── italic: _..._ ── */
        if (p[0] == '_') {
            const char *close = strchr(p + 1, '_');
            if (close) {
                fprintf(fp, "<em>");
                int inner_len = close - (p + 1);
                char *inner = malloc(inner_len + 1);
                if (inner) {
                    strncpy(inner, p + 1, inner_len);
                    inner[inner_len] = '\0';
                    renderInline(fp, inner);
                    free(inner);
                }
                fprintf(fp, "</em>");
                p = close + 1;
                continue;
            }
        }

        /* plain character */
        fputc(*p, fp);
        p++;
    }
}

void writeHTML(char *filename)
{
    int inList = 0;
    int inCode = 0;
    
    char output[256];

    size_t len = strlen(filename);

    if(len >= 5 && strcmp(filename + len - 5 , ".html") == 0){
        strcpy(output , filename);
    }
    else{
        snprintf(output,sizeof(output),"%s.html",filename);
    }

    FILE *fp;
    fp = fopen(output,"w");

    if (!fp) {
        printf("Error while creating file\n");
        return;
    }

    fprintf(
        fp,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<title>Markdown Output</title>\n"
        "<style>\n"
        "body{font-family:Arial,sans-serif;max-width:800px;margin:auto;padding:"
        "2rem;line-height:1.6;}\n"
        "pre{background:#f4f4f4;padding:1rem;overflow-x:auto;}\n"
        "pre "
        "{background:#f6f8fa;padding:1rem;border-radius:8px;overflow-x:auto;}\n"
        "code {font-family: Consolas, Monaco, monospace;}\n"
        "</style>\n"
        "</head>\n"
        "<body>\n");

    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type != BULLET_LIST && inList) {
            fprintf(fp, "</ul>\n");
            inList = 0;
        }

        if (tokens[i].type == HASH) {
            fprintf(fp, "<h1>"); renderInline(fp, tokens[i].text); fprintf(fp, "</h1>\n");
        } else if (tokens[i].type == HASH2) {
            fprintf(fp, "<h2>"); renderInline(fp, tokens[i].text); fprintf(fp, "</h2>\n");
        } else if (tokens[i].type == HASH3) {
            fprintf(fp, "<h3>"); renderInline(fp, tokens[i].text); fprintf(fp, "</h3>\n");
        } else if (tokens[i].type == BLOCK_QUOTE) {
            fprintf(fp, "<blockquote>"); renderInline(fp, tokens[i].text); fprintf(fp, "</blockquote>\n");
        } else if (tokens[i].type == PARAGRAPH) {
            if (inCode) {
                /* inside a code block: emit raw, no inline processing */
                fprintf(fp, "%s\n", tokens[i].text);
            } else {
                fprintf(fp, "<p>"); renderInline(fp, tokens[i].text); fprintf(fp, "</p>\n");
            }
        } else if (tokens[i].type == BULLET_LIST) {
            if (!inList) {
                inList = 1;
                fprintf(fp, "<ul>\n");
            }
            fprintf(fp, "<li>"); renderInline(fp, tokens[i].text); fprintf(fp, "</li>\n");
        } else if (tokens[i].type == CODE_BLOCK) {
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

    fprintf(fp, "</body></html>");
    fclose(fp);
    printf("HTML generated successfully as '%s'.\n", output);
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

    while (*p != '\0') {
        if (*p == '\n') {
            processLine(line_start, p);
            line_start = p + 1;
        }
        p++;
    }

    if (p != line_start) {
        processLine(line_start, p);
    }
}

void cleanup()
{
    if (tokens != NULL) {
        for (int i = 0; i < tokenCount; i++) {
            free(tokens[i].text);
        }
        free(tokens);
        tokens = NULL;
    }
}

void processLine(char *line_start, char *end)
{
    char *content_start = line_start;
    Token t;
    int line_len = end - line_start;

    if (line_len == 0) {
        return;
    }

    if (line_len >= 3 && strncmp(line_start, "```", 3) == 0) {
        t.type = CODE_BLOCK;
        content_start = line_start + 3;
        parsingCodeBlock = !parsingCodeBlock;
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
    } else if (line_len >= 2 && strncmp(line_start, "* ", 2) == 0) {
        t.type = BULLET_LIST;
        content_start = line_start + 2;
    } else if (line_len >= 2 && strncmp(line_start, "> ", 2) == 0) {
        t.type = BLOCK_QUOTE;
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