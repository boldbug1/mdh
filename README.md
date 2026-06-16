# Markdown-to-HTML Parser (`mdh`)

A lightweight Markdown-to-HTML compiler written in C. It parses Markdown documents and generates clean HTML output with support for headings, lists, paragraphs, and fenced code blocks.

## Features

* Written in pure C
* Dynamic token storage using heap allocation
* Fenced code block support
* Automatic HTML generation
* Lightweight with no external dependencies

## Supported Syntax

| Markdown         | HTML          |
| ---------------- | ------------- |
| `# Heading`      | `<h1>`        |
| `## Heading`     | `<h2>`        |
| `### Heading`    | `<h3>`        |
| `- Item`         | `<ul><li>`    |
| Triple backticks | `<pre><code>` |
| Plain text       | `<p>`         |

For complete Markdown syntax documentation, see the official Markdown guide:
https://www.markdownguide.org/basic-syntax/

## Installation

```bash
gcc -O2 mdh.c -o mdh
```

## Usage

Run the parser and provide a Markdown file:

```bash
./mdh input.md
```

The generated HTML file will be written to:

```text
index.html
```

## Example

### Input

```md
# Hello

This is a paragraph.

- Item 1
- Item 2
```

### Output

```html
<h1>Hello</h1>

<p>This is a paragraph.</p>

<ul>
    <li>Item 1</li>
    <li>Item 2</li>
</ul>
```

## Implementation Notes

* Line-based Markdown parsing
* Dynamic token array with automatic resizing
* Context-aware code block handling
* HTML generation through token traversal
