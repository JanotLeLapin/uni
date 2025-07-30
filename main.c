#include "uni.h"
#include "highlight.h"

#ifdef ENABLE_TREE_SITTER
#include "style/code.h"
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#include <cmarkdown.h>

typedef struct {
  cmark_ctx_t cmark;
  dyn_str_t str;
  cmark_elem_t current;
} ctx_t;

static inline void
embed_stylesheet(dyn_str_t *dst, const char *stylesheet, size_t stylesheet_len)
{
  DYN_STR_APPEND_PLAIN(dst, "<style>");
  dyn_str_append(dst, stylesheet, stylesheet_len);
  DYN_STR_APPEND_PLAIN(dst, "</style>");
}

#ifdef ENABLE_TREE_SITTER

static inline void
embed_code_stylesheet(dyn_str_t *dst)
{
  embed_stylesheet(dst, style_code, style_code_len);
}

#else

static inline void
embed_code_stylesheet(dyn_str_t *dst)
{}

#endif

static int compile_inline(ctx_t *ctx, dyn_str_t *dst);

static inline int
compile_plain(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_append(dst, ctx->current.plain.p, ctx->current.plain.len);
  return 0;
}

static inline int
compile_anchor(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_t content;

  if (-1 == dyn_str_init(&content, 16)) {
    return -1;
  }

  while (CMARK_ELEM_ANCHOR_LINK != ctx->current.type) {
    if (CMARK_ELEM_EOF == ctx->current.type) {
      return 0;
    }
    ctx->current = cmark_next(&ctx->cmark);
    compile_inline(ctx, &content);
  }

  DYN_STR_APPEND_PLAIN(dst, "<a href=\"");
  dyn_str_append(dst, ctx->current.anchor_link.p, ctx->current.anchor_link.len);
  DYN_STR_APPEND_PLAIN(dst, "\">");
  dyn_str_append(dst, content.p, content.len);
  DYN_STR_APPEND_PLAIN(dst, "</a>");

  dyn_str_free(&content);

  return 0;
}

static inline int
compile_code_inline(ctx_t *ctx, dyn_str_t *dst)
{
  DYN_STR_APPEND_PLAIN(dst, "<code>");
  dyn_str_append(dst, ctx->current.code_inline.p, ctx->current.code_inline.len);
  DYN_STR_APPEND_PLAIN(dst, "</code>");

  return 0;
}

static int
compile_inline(ctx_t *ctx, dyn_str_t *dst)
{
  switch (ctx->current.type) {
  case CMARK_ELEM_PLAIN:
    return compile_plain(ctx, dst);
  case CMARK_ELEM_ANCHOR_TEXT:
    return compile_anchor(ctx, dst);
  case CMARK_ELEM_CODE_INLINE:
    return compile_code_inline(ctx, dst);
  default:
    return -1;
  }
}

static inline int
compile_heading(ctx_t *ctx, dyn_str_t *dst)
{
  char tag[3];
  tag[0] = 'h';
  tag[1] = ctx->current.heading + '0';
  tag[2] = '>';

  DYN_STR_APPEND_PLAIN(dst, "<");
  dyn_str_append(dst, tag, 3);

  while (CMARK_ELEM_BREAK != ctx->current.type && CMARK_ELEM_EOF != ctx->current.type) {
    ctx->current = cmark_next(&ctx->cmark);
    compile_inline(ctx, dst);
  }

  DYN_STR_APPEND_PLAIN(dst, "</");
  dyn_str_append(dst, tag, 3);

  return 0;
}

static inline int
compile_list(ctx_t *ctx, dyn_str_t *dst)
{
  DYN_STR_APPEND_PLAIN(dst, "<ul>");

  cmark_next(&ctx->cmark);
  while (CMARK_ELEM_LIST_END != ctx->current.type) {
    DYN_STR_APPEND_PLAIN(dst, "<li>");
    do {
      ctx->current = cmark_next(&ctx->cmark);
      if (CMARK_ELEM_LIST_END == ctx->current.type) {
        break;
      }
      compile_inline(ctx, dst);
    } while (CMARK_ELEM_LIST_ITEM != ctx->current.type);
    DYN_STR_APPEND_PLAIN(dst, "</li>");
  }

  DYN_STR_APPEND_PLAIN(dst, "</ul>");

  return 0;
}

static inline int
compile_code_multiline(ctx_t *ctx, dyn_str_t *dst)
{
  DYN_STR_APPEND_PLAIN(dst, "<code><pre class=\"code-");
  dyn_str_append(dst, ctx->current.code_multiline.lang.p, ctx->current.code_multiline.lang.len);
  DYN_STR_APPEND_PLAIN(dst, "\">");
  highlight(dst, ctx->current.code_multiline.lang, ctx->current.code_multiline.content);
  DYN_STR_APPEND_PLAIN(dst, "</pre></code>");

  return 0;
}

static inline int
compile_paragraph(ctx_t *ctx, dyn_str_t *dst)
{
  DYN_STR_APPEND_PLAIN(dst, "<p>");

  compile_inline(ctx, dst);
  while (CMARK_ELEM_BREAK != ctx->current.type && CMARK_ELEM_EOF != ctx->current.type) {
    ctx->current = cmark_next(&ctx->cmark);
    compile_inline(ctx, dst);
  }

  DYN_STR_APPEND_PLAIN(dst, "</p>");

  return 0;
}

int
main(int argc, char **argv)
{
  ctx_t ctx;
  int fd;
  char *src;
  size_t len;
  clock_t start, end;

  if (argc < 2) {
    fprintf(stderr, "missing input file\n");
    return -1;
  }

  fd = open(argv[1], O_RDONLY);

  len = lseek(fd, 0, SEEK_END);
  src = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

  cmark_init_ctx(&ctx.cmark, src, len);
  if (-1 == dyn_str_init(&ctx.str, 512)) {
    perror("dyn_str_init");
    return -1;
  };

  DYN_STR_APPEND_PLAIN(&ctx.str, "<!DOCTYPE html><html><head><title>Hello, World!</title>");
  embed_code_stylesheet(&ctx.str);
  DYN_STR_APPEND_PLAIN(&ctx.str, "</head><body>");

  start = clock();
  do {
    ctx.current = cmark_next(&ctx.cmark);
    fprintf(stderr, "got %d\n", ctx.current.type);
    switch (ctx.current.type) {
    case CMARK_ELEM_HEADING:
      compile_heading(&ctx, &ctx.str);
      break;
    case CMARK_ELEM_LIST_START:
      compile_list(&ctx, &ctx.str);
      break;
    case CMARK_ELEM_CODE_MULTILINE:
      compile_code_multiline(&ctx, &ctx.str);
      break;
    case CMARK_ELEM_EOF:
      break;
    default:
      compile_paragraph(&ctx, &ctx.str);
      break;
    }
  } while (CMARK_ELEM_EOF != ctx.current.type);
  end = clock();

  fprintf(stderr, "took %fs\n", ((double) (end - start)) / CLOCKS_PER_SEC);

  DYN_STR_APPEND_PLAIN(&ctx.str, "</body></html>");

  fprintf(stdout, "%.*s\n", (int) ctx.str.len, ctx.str.p);

  dyn_str_free(&ctx.str);

  munmap(src, len);

  close(fd);
}
