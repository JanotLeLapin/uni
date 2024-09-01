#include <cmarkdown.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define COMPILE_CHILDREN(i, node, stream) for (i = 0; i < node.children_count; i++) compile_node(node.children[i], stream);

char *uni_highlight(const char *source, const char *lang);
void uni_free_buffer(char *ptr);

void
compile_node(struct CMarkNode node, FILE *stream)
{
  size_t i;
  unsigned int last_end;

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
        last_end = 0;

        char *buffer = uni_highlight(node.data.code.content, node.data.code.lang);
        fprintf(stream, "<pre><code>%s</pre></code>", buffer);
        uni_free_buffer(buffer);
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

  fprintf(file, "<!DOCTYPE html><head><link rel=\"stylesheet\" href=\"/uni/static/app.css\"/><meta charset=\"utf-8\"/></head><body>");
  compile_node(root, file);
  fprintf(file, "</body>");

  cmark_free_node(root);
  free(ctx);

  return 0;
}
