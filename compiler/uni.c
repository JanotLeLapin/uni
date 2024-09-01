#include <cmarkdown.h>
#include <string.h>
#include <tree_sitter/api.h>
#include <stdio.h>

#define COMPILE_CHILDREN(i, node, stream) for (i = 0; i < node.children_count; i++) compile_node(node.children[i], stream);

const TSLanguage *tree_sitter_python(void);
const TSLanguage *tree_sitter_c(void);

void
putchar_escaped(char c, FILE *stream)
{
  switch (c) {
    case '<':
      fprintf(stream, "&lt;");
      break;
    case '>':
      fprintf(stream, "&gt;");
      break;
    case '&':
      fprintf(stream, "&ampq;");
      break;
    default:
      putc(c, stream);
      break;
  }
}

void
compile_ts_node(TSNode node, const char *sources, unsigned int *last_end, FILE *stream)
{
  TSNode child;
  unsigned int start, end, i;

  if (ts_node_is_null(node)) return;

  if (ts_node_child_count(node)) {
    for (i = 0; i < ts_node_child_count(node); i++) {
      child = ts_node_child(node, i);
      compile_ts_node(child, sources, last_end, stream);
    }
  } else {
    start = ts_node_start_byte(node);
    end = ts_node_end_byte(node);

    for (i = *last_end; i < start; i++) {
      putchar_escaped(sources[i], stream);
    }

    fprintf(stream, "<span class=\"%s\">", ts_node_type(node));
    for (i = start; i < end; i++) {
      putchar_escaped(sources[i], stream);
    }
    fprintf(stream, "</span>");

    *last_end = end;
  }
}

void
compile_node(struct CMarkNode node, FILE *stream)
{
  size_t i;
  unsigned int last_end;
  TSParser *ts_parser;
  const TSLanguage *ts_parser_lang = NULL;
  TSTree *ts_tree;
  TSNode ts_node;

  switch (node.type) {
    case CMARK_ROOT:
      fprintf(stream, "<main>");
      COMPILE_CHILDREN(i, node, stream);
      fprintf(stream, "</main>");
      break;
    case CMARK_NULL:
      break;
    case CMARK_HEADER:
      fprintf(stream, "<h%d>", node.data.header.level);
      COMPILE_CHILDREN(i, node, stream);
      fprintf(stream, "</h%d>", node.data.header.level);
      break;
    case CMARK_PARAGRAPH:
      fprintf(stream, "<p>");
      COMPILE_CHILDREN(i, node, stream);
      fprintf(stream, "</p>");
      break;
    case CMARK_ANCHOR:
      fprintf(stream, "<a href=\"%s\">", node.data.anchor.href);
      COMPILE_CHILDREN(i, node, stream);
      fprintf(stream, "</a>");
      break;
    case CMARK_CODE:
      if (node.data.code.is_block) {
        if (!strcmp("python", node.data.code.lang) || !strcmp("py", node.data.code.lang))
          ts_parser_lang = tree_sitter_python();
        else if (!strcmp("c", node.data.code.lang))
          ts_parser_lang = tree_sitter_c();

        if (ts_parser_lang) {
          last_end = 0;

          ts_parser = ts_parser_new();
          ts_parser_set_language(ts_parser, ts_parser_lang);

          ts_tree = ts_parser_parse_string(ts_parser, NULL, node.data.code.content, strlen(node.data.code.content));
          ts_node = ts_tree_root_node(ts_tree);

          fprintf(stream, "<pre><code>");
          compile_ts_node(ts_node, node.data.code.content, &last_end, stream);
          fprintf(stream, "</pre></code>");

          ts_tree_delete(ts_tree);
        } else {
          fprintf(stream, "<pre><code>%s</code></pre>", node.data.code.content);
        }
      } else {
        fprintf(stream, "<code>%s</code>", node.data.code.content);
      }
      break;
    case CMARK_PLAIN:
      fprintf(stream, "%s", node.data.plain);
      break;
    case CMARK_WHITESPACE:
      fprintf(stream, " ");
      break;
    case CMARK_BREAK:
      fprintf(stream, "</br>");
      break;
  }
}

int
main(void)
{
  struct CMarkContext *ctx = cmark_create_context(stdin);
  struct CMarkNode root = cmark_parse(ctx);
  FILE *file = stdout;

  fprintf(file, "<!DOCTYPE html><head><link rel=\"stylesheet\" href=\"/static/app.css\"/><meta charset=\"utf-8\"/></head><body>");
  compile_node(root, file);
  fprintf(file, "</body>");

  cmark_free_node(root);

  fclose(file);

  return 0;
}
