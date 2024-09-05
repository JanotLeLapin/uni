#include <cmarkdown.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define COMPILE_CHILDREN() for (i = 0; i < node.children_count; i++) compile_node(ctx, node.children[i], stream);
#define COMPILE_CHILDREN_STR() for (i = 0; i < node.children_count; i++) compile_node_str(node.children[i], buffer, buffer_length, buffer_end);

char *uni_highlight(const char *source, const char *lang);
void uni_free_buffer(char *ptr);

struct Header {
  char level;
  char *text;
  char *id;
};

struct Context {
  size_t header_count;
  size_t header_size;
  struct Header *headers;
};

void
kebab_case(char *dest, const char *src, size_t dest_length)
{
  size_t di = 0, si = 0;

  while (di < dest_length && '\0' != src[si]) {
    if (' ' == src[si] || '\t' == src[si]) {
      dest[di] = '-';
      di++;

      while (' ' == src[si] || '\t' == src[si]) {
        si++;
      }
    } else {
      dest[di] = src[si] >= 'A' && src[si] <= 'Z' ? src[si] + 32 : src[si];
      di++;
      si++;
    }
  }
  dest[di] = '\0';
}

void
compile_node_str(struct CMarkNode node, char *buffer, size_t buffer_length, size_t *buffer_end) {
  size_t i;

  switch (node.type) {
    case CMARK_HEADER:
    case CMARK_ANCHOR:
      COMPILE_CHILDREN_STR();
      break;
    case CMARK_CODE:
      strcat(buffer, node.data.code.content);
      break;
    case CMARK_PLAIN:
      strcat(buffer, node.data.plain);
      break;
    case CMARK_WHITESPACE:
      strcat(buffer, " ");
      break;
    default:
      break;
  }
}

void
compile_node(struct Context *ctx, struct CMarkNode node, FILE *stream)
{
  size_t i, end;
  struct Header header;

  switch (node.type) {
    case CMARK_ROOT:
      fprintf(stream, "<main>");
      COMPILE_CHILDREN();
      fprintf(stream, "</main>");
      break;
    case CMARK_NULL:
      break;
    case CMARK_HEADER:
      end = 0;
      header.level = node.data.header.level;
      header.text = malloc(128);
      header.text[0] = '\0';
      compile_node_str(node, header.text, 128, &end);
      header.id = malloc(128);
      header.id[0] = '\0';
      kebab_case(header.id, header.text, 128);
      ctx->headers[ctx->header_count] = header;
      ctx->header_count++;

      fprintf(stream, "<h%d id=\"%s\">", node.data.header.level, header.id);
      COMPILE_CHILDREN();
      fprintf(stream, "</h%d>", node.data.header.level);
      break;
    case CMARK_PARAGRAPH:
      fprintf(stream, "<p>");
      COMPILE_CHILDREN();
      fprintf(stream, "</p>");
      break;
    case CMARK_ANCHOR:
      fprintf(stream, "<a href=\"%s\">", node.data.anchor.href);
      COMPILE_CHILDREN();
      fprintf(stream, "</a>");
      break;
    case CMARK_CODE:
      if (node.data.code.is_block) {
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
  struct CMarkContext *cmark_context = cmark_create_context(stdin);
  struct CMarkNode root = cmark_parse(cmark_context);
  struct Context ctx;
  FILE *file = stdout;
  size_t i;

  ctx.header_count = 0;
  ctx.header_size = 8;
  ctx.headers = malloc(sizeof(struct Header) * 8);

  fprintf(file, "<!DOCTYPE html><head><link rel=\"stylesheet\" href=\"/uni/static/app.css\"/><meta charset=\"utf-8\"/></head><body>");
  compile_node(&ctx, root, file);

  cmark_free_node(root);
  free(cmark_context);

  fprintf(file, "<nav><ul>");
  for (i = 0; i < ctx.header_count; i++) {
    fprintf(file, "<li><a href=\"#%s\">%s</a></li>", ctx.headers[i].id, ctx.headers[i].text);
    free(ctx.headers[i].text);
    free(ctx.headers[i].id);
  }
  free(ctx.headers);
  fprintf(file, "</nav></ul>");
  fprintf(file, "</body>");

  return 0;
}
