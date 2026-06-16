# Markdown-to-HTML Parser (`mdh`)

A lightweight, high-performance static Markdown-to-HTML compiler written in pure C. This tool processes structural Markdown files line-by-line using a state-machine architecture to accurately translate headings, lists, paragraphs, and code blocks into clean, valid HTML documents.

---

## Architecture Overview

The program follows a traditional three-phase compiler pipeline:

1. **Lexical Scanning (`scanTokens`):** Reads the raw input character stream, maps line boundaries by locating newline (`\n`) characters, and handles dynamic memory initialization.
2. **Context-Aware Parsing (`processLine`):** Evaluates the syntax tags at the beginning of each line slice. It utilizes an internal state variable to isolate block scopes (like distinguishing code syntax from live Markdown markers).
3. **Code Generation (`writeHTML`):** Iterates over the tracked tokens, monitors container layouts (opening and closing tag balances like `<ul>` and `</ul>`), and flushes the formatted markup out to the physical file layer.

---

## Key Technical Systems & Fixes

### 1. Robust Dynamic Memory Architecture

The parser stores extracted content structures inside an expandable heap array buffer (`Token *tokens`).

To prevent memory access violations (`SIGSEGV`) or corruption, the program initializes a structured capacity buffer up-front before text processing begins. If the processed lines surpass the tracking threshold, the program triggers an automated block reallocation strategy:

```c
if (tokenCount == tokenCapacity) {
    tokenCapacity *= 2;
    Token *temp = realloc(tokens, tokenCapacity * sizeof(Token));
    // ... error verification ...
}

```

This guarantees an efficient memory footprint that scales gracefully with large documents.

### 2. Context-Aware Code Block State Machine

Standard line-by-line Markdown tokenizers often fail inside code blocks because language features (such as `#include` or `- comments`) conflict with Markdown syntax definitions (Headers, Lists).

To solve this, this parser implements an exclusive state tracking lock via `parsingCodeBlock`. Encountering an opening triple-backtick delimiter (`````) toggles this condition state. As long as this state remains locked active, normal syntax parsing is bypassed, and all underlying lines are preserved raw inside the target code block until the state is closed.

### 3. Memory Safety & Stability Guardrails

* **Buffer Overflow Prevention:** Global standard inputs limit character streams to safe boundaries (`MAX_SIZE - 1`), securing the memory stacks from intentional or accidental overflows.
* **Resource Leak Remediation:** The HTML generator explicitly maintains file system streams, matching every runtime opening handle with a clean `fclose()` termination to release operating system descriptors and guarantee data flush integrity.

---

## Supported Markdown Syntax

The parser processes standard block elements using specific delimiter matching rules:

| Element | Markdown Prefix | Generated HTML |
| --- | --- | --- |
| **Heading 1** | `# ` | `<h1>...</h1>` |
| **Heading 2** | `## ` | `<h2>...</h2>` |
| **Heading 3** | `### ` | `<h3>...</h3>` |
| **Bullet List Item** | `- ` | `<ul><li>...</li></ul>` |
| **Code Block** | ````` | `<pre><code>...</code></pre>` |
| **Paragraph** | *Default* | `<p>...</p>` |

---

## Installation & Compilation

Since the utility uses only standard POSIX/ISO C libraries, it can be compiled out-of-the-box using any standard C compiler (such as GCC or Clang).

```bash
# Compile the compiler tool
gcc -O2 mdh.c -o mdh

# Execute the binary
./mdh

```

---

## Usage Guide

1. Run the application binaries inside your working terminal console environment.
2. Paste or type your target Markdown payload block directly into the terminal stream.
3. Once completed, issue an **End-Of-File (EOF)** control command interrupt to conclude reading:
* **Linux/macOS:** Press `Ctrl + D`
* **Windows:** Press `Ctrl + Z` and then hit `Enter`


4. Answer the dynamic user optimization prompts (`y/n`) to compile and download your output data file. The compiled production code will automatically export to your active directory path under the filename `Index.html`.
