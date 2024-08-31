#include <cmarkdown.h>
#include <string.h>
#include <tree_sitter/api.h>
#include <stdio.h>

#define COMPILE_CHILDREN(i, node) for (i = 0; i < node.children_count; i++) compile_node(node.children[i]);

const TSLanguage *tree_sitter_python(void);

void
compile_ts_node(TSNode node, const char *sources, unsigned int *last_end)
{
  TSNode child;
  unsigned int start, end, i;

  if (ts_node_is_null(node)) return;

  if (ts_node_child_count(node)) {
    for (i = 0; i < ts_node_child_count(node); i++) {
      child = ts_node_child(node, i);
      compile_ts_node(child, sources, last_end);
    }
  } else {
    start = ts_node_start_byte(node);
    end = ts_node_end_byte(node);

    for (i = *last_end; i < start; i++) {
      putchar(sources[i]);
    }

    printf("<span class=\"%s\">", ts_node_type(node));
    for (i = start; i < end; i++) {
      putchar(sources[i]);
    }
    printf("</span>");

    *last_end = end;
  }
}

void
compile_node(struct CMarkNode node)
{
  size_t i;
  unsigned int last_end;
  TSParser *ts_parser;
  TSTree *ts_tree;
  TSNode ts_node;

  switch (node.type) {
    case CMARK_ROOT:
      printf("<main>");
      COMPILE_CHILDREN(i, node);
      printf("</main>");
      break;
    case CMARK_NULL:
      break;
    case CMARK_HEADER:
      printf("<h%d>", node.data.header.level);
      COMPILE_CHILDREN(i, node);
      printf("</h%d>", node.data.header.level);
      break;
    case CMARK_PARAGRAPH:
      printf("<p>");
      COMPILE_CHILDREN(i, node);
      printf("</p>");
      break;
    case CMARK_ANCHOR:
      printf("<a href=\"%s\">", node.data.anchor.href);
      COMPILE_CHILDREN(i, node);
      printf("</a>");
      break;
    case CMARK_CODE:
      if (node.data.code.is_block) {
        if ('\0' == node.data.code.lang[0]) {
          printf("<pre><code>%s</code></pre>", node.data.code.content);
        } else {
          last_end = 0;

          ts_parser = ts_parser_new();
          ts_parser_set_language(ts_parser, tree_sitter_python());

          ts_tree = ts_parser_parse_string(ts_parser, NULL, node.data.code.content, strlen(node.data.code.content));
          ts_node = ts_tree_root_node(ts_tree);

          printf("<pre><code>");
          compile_ts_node(ts_node, node.data.code.content, &last_end);
          printf("</pre></code>");

          ts_tree_delete(ts_tree);
        }
      } else {
        printf("<code>%s</code>", node.data.code.content);
      }
      break;
    case CMARK_PLAIN:
      printf("%s", node.data.plain);
      break;
    case CMARK_WHITESPACE:
      printf(" ");
      break;
    case CMARK_BREAK:
      printf("</br>");
      break;
  }
}

int
main(void)
{
  struct CMarkContext *ctx = cmark_create_context(stdin);
  struct CMarkNode root = cmark_parse(ctx);

  compile_node(root);
  cmark_free_node(root);

  return 0;
}
