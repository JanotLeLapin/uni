#include "highlight.h"

#include <string.h>

#include <tree_sitter/api.h>

#include "highlights/json.h"
#include "highlights/python.h"
#include "uni.h"

const TSLanguage *tree_sitter_json(void);
const TSLanguage *tree_sitter_python(void);

int
highlight(dyn_str_t *dst, cmark_str_t lang, cmark_str_t code)
{
  unsigned int err_offset, start, end, name_length;
  TSParser *parser;
  const TSLanguage *language;
  TSQuery *query;
  TSQueryCursor *query_cursor;
  TSTree *tree;
  TSNode root;
  TSQueryMatch match;
  TSQueryError err;
  size_t i = 0;
  unsigned short ci;
  const char *name;

  if (!strncmp("py", lang.p, lang.len)) {
    language = tree_sitter_python();
  } else if (!strncmp("json", lang.p, lang.len)) {
    language = tree_sitter_json();
  } else {
    return -1;
  }

  parser = ts_parser_new();
  ts_parser_set_language(parser, language);

  query = ts_query_new(tree_sitter_python(), (char *) highlights_python, highlights_python_len, &err_offset, &err);
  if (!query) {
    return -1;
  }
  query_cursor = ts_query_cursor_new();
  tree = ts_parser_parse_string(parser, 0, code.p, code.len);
  root = ts_tree_root_node(tree);

  ts_query_cursor_exec(query_cursor, query, root);

  while (ts_query_cursor_next_match(query_cursor, &match)) {
    for (ci = 0; ci < match.capture_count; ci++) {
      start = ts_node_start_byte(match.captures[ci].node);
      end = ts_node_end_byte(match.captures[ci].node);
      name = ts_query_capture_name_for_id(query, match.captures[ci].index, &name_length);

      if (i < start) {
        dyn_str_append(dst, code.p + i, start - i);
        i = end;
      } else {
        continue;
      }

      dyn_str_append(dst, "<span class=\"", 13);
      dyn_str_append(dst, name, name_length);
      dyn_str_append(dst, "\">", 2);
      dyn_str_append(dst, code.p + start, end - start);
      dyn_str_append(dst, "</span>", 7);
    }
  }

  dyn_str_append(dst, code.p + i, code.len - i);

  ts_tree_delete(tree);
  ts_query_cursor_delete(query_cursor);
  ts_query_delete(query);
  ts_parser_delete(parser);

  return 0;
}
