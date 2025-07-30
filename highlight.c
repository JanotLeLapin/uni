#include "highlight.h"

#include <stdio.h>

#include <tree_sitter/api.h>

#include "highlights/json.h"
#include "highlights/python.h"

const TSLanguage *tree_sitter_json(void);
const TSLanguage *tree_sitter_python(void);

int
highlight(cmark_str_t code)
{
  unsigned int err_offset, start, end, name_length;
  TSParser *parser = ts_parser_new();
  TSQuery *query;
  TSQueryCursor *query_cursor;
  TSTree *tree;
  TSNode root;
  TSQueryMatch match;
  TSQueryError err;
  unsigned short i;
  const char *name;

  ts_parser_set_language(parser, tree_sitter_python());

  query = ts_query_new(tree_sitter_python(), (char *) highlights_python, highlights_python_len, &err_offset, &err);
  if (!query) {
    return -1;
  }
  query_cursor = ts_query_cursor_new();
  tree = ts_parser_parse_string(parser, 0, code.p, code.len);
  root = ts_tree_root_node(tree);

  ts_query_cursor_exec(query_cursor, query, root);

  while (ts_query_cursor_next_match(query_cursor, &match)) {
    for (i = 0; i < match.capture_count; i++) {
      start = ts_node_start_byte(match.captures[i].node);
      end = ts_node_end_byte(match.captures[i].node);
      name = ts_query_capture_name_for_id(query, match.captures[i].index, &name_length);
      fprintf(stderr, "%.*s: %.*s\n", name_length, name, end - start, code.p + start);
    }
  }

  ts_tree_delete(tree);
  ts_query_cursor_delete(query_cursor);
  ts_query_delete(query);
  ts_parser_delete(parser);

  return 0;
}
