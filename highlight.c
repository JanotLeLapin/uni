#include "highlight.h"

#include <stdio.h>

#include <tree_sitter/api.h>

const TSLanguage *tree_sitter_json(void);
const TSLanguage *tree_sitter_python(void);

void
walk(cmark_str_t code, TSNode node)
{
  const char *type = ts_node_type(node);
  size_t child_count = ts_node_child_count(node), i;
  unsigned int start;
  unsigned int end;

  if (0 == child_count) {
    start = ts_node_start_byte(node);
    end = ts_node_end_byte(node);
    fprintf(stderr, "%s: %.*s\n", ts_node_type(node), end - start, code.p + start);
  } else {
    for (i = 0; i < child_count; i++) {
      walk(code, ts_node_child(node, i));
    }
  }
}

int
highlight(cmark_str_t code)
{
  TSParser *parser = ts_parser_new();

  ts_parser_set_language(parser, tree_sitter_python());

  TSTree *tree = ts_parser_parse_string(parser, 0, code.p, code.len);

  TSNode root = ts_tree_root_node(tree);

  walk(code, root);

  ts_tree_delete(tree);
  ts_parser_delete(parser);

  return 0;
}
