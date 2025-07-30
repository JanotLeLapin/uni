#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#include "uni.h"

#include "style/app.h"

#ifdef ENABLE_TREE_SITTER
#include "highlight.h"
#include "style/code.h"
#else
int
highlight(dyn_str_t *dst, cmark_str_t lang, cmark_str_t code)
{
  dyn_str_append(dst, code.p, code.len);
  return 0;
}
#endif

#ifdef ENABLE_TOC
#include "style/toc.h"
#endif

static inline void
embed_stylesheet(dyn_str_t *dst, const char *stylesheet, size_t stylesheet_len)
{
  DYN_STR_APPEND_PLAIN(dst, "<style>");
  dyn_str_append(dst, stylesheet, stylesheet_len);
  DYN_STR_APPEND_PLAIN(dst, "</style>");
}

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
compile_heading_toc(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_t plain;
  char tag[2], *id;
  size_t i;
  cmark_elem_t e;

  if (-1 == dyn_str_init(&plain, 32)) {
    return 0;
  }

  tag[0] = 'h';
  tag[1] = ctx->current.heading + '0';

  DYN_STR_APPEND_PLAIN(dst, "<");
  dyn_str_append(dst, tag, 2);

  i = ctx->cmark.i;
  e = ctx->current;
  while (CMARK_ELEM_BREAK != ctx->current.type && CMARK_ELEM_EOF != ctx->current.type) {
    ctx->current = cmark_next(&ctx->cmark);
    switch (ctx->current.type) {
    case CMARK_ELEM_PLAIN:
      dyn_str_append(&plain, ctx->current.plain.p, ctx->current.plain.len);
      break;
    case CMARK_ELEM_CODE_INLINE:
      dyn_str_append(&plain, ctx->current.code_inline.p, ctx->current.code_inline.len);
      break;
    default:
      break;
    }
  }

  ctx->cmark.i = i;
  ctx->current = e;

  id = malloc(plain.len);
  for (i = 0; i < plain.len; i++) {
    if ('a' <= plain.p[i] && 'z' >= plain.p[i]) {
      id[i] = plain.p[i];
    } else if ('A' <= plain.p[i] && 'Z' >= plain.p[i]) {
      id[i] = plain.p[i] + ('a' - 'A');
    } else {
      id[i] = '-';
    }
  }

  DYN_STR_APPEND_PLAIN(dst, " id=\"");
  dyn_str_append(dst, id, plain.len);
  DYN_STR_APPEND_PLAIN(dst, "\">");

  while (CMARK_ELEM_BREAK != ctx->current.type && CMARK_ELEM_EOF != ctx->current.type) {
    ctx->current = cmark_next(&ctx->cmark);
    compile_inline(ctx, dst);
  }

  DYN_STR_APPEND_PLAIN(dst, "</");
  dyn_str_append(dst, tag, 2);
  DYN_STR_APPEND_PLAIN(dst, ">");

  DYN_STR_APPEND_PLAIN(&ctx->toc, "<li><a href=\"#");
  dyn_str_append(&ctx->toc, id, plain.len);
  DYN_STR_APPEND_PLAIN(&ctx->toc, "\">");
  dyn_str_append(&ctx->toc, plain.p, plain.len);
  DYN_STR_APPEND_PLAIN(&ctx->toc, "</a></li>");

  dyn_str_free(&plain);
  free(id);

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
  DYN_STR_APPEND_PLAIN(dst, "<pre class=\"code-");
  dyn_str_append(dst, ctx->current.code_multiline.lang.p, ctx->current.code_multiline.lang.len);
  DYN_STR_APPEND_PLAIN(dst, "\">");
  DYN_STR_APPEND_PLAIN(dst, "<code>");
  highlight(dst, ctx->current.code_multiline.lang, ctx->current.code_multiline.content);
  DYN_STR_APPEND_PLAIN(dst, "</code></pre>");

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
  int i = 1;
  const char *title = "uni";
  ctx_t ctx;
  int fd;
  char *src;
  size_t len;
  clock_t start, end;

  if (argc < 2) {
    fprintf(stderr, "missing input file\n");
    return -1;
  }

  while (1) {
    if (!strcmp("--title", argv[i]) || !strcmp("-t", argv[i])) {
      title = argv[++i];
      i++;
    } else {
      break;
    }
  }

  fd = open(argv[i], O_RDONLY);

  len = lseek(fd, 0, SEEK_END);
  src = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

  cmark_init_ctx(&ctx.cmark, src, len);
  if (-1 == dyn_str_init(&ctx.str, 512)) {
    perror("dyn_str_init");
    return -1;
  };

  #ifdef ENABLE_TOC
  if (-1 == dyn_str_init(&ctx.toc, 64)) {
    perror("dyn_str_init");
    dyn_str_free(&ctx.str);
    return -1;
  }
  #endif

  DYN_STR_APPEND_PLAIN(&ctx.str, "<!DOCTYPE html><html><head><title>");
  dyn_str_append(&ctx.str, title, strlen(title));
  DYN_STR_APPEND_PLAIN(&ctx.str, "</title>");

  embed_stylesheet(&ctx.str, (char *) style_app, style_app_len);
  #ifdef ENABLE_TREE_SITTER
  embed_stylesheet(&ctx.str, (char *) style_code, style_code_len);
  #endif
  #ifdef ENABLE_TOC
  embed_stylesheet(&ctx.str, (char *) style_toc, style_toc_len);
  #endif

  DYN_STR_APPEND_PLAIN(&ctx.str, "</head><body>");

  start = clock();
  do {
    ctx.current = cmark_next(&ctx.cmark);
    switch (ctx.current.type) {
    case CMARK_ELEM_HEADING:
      #ifdef ENABLE_TOC
      compile_heading_toc(&ctx, &ctx.str);
      #else
      compile_heading(&ctx, &ctx.str);
      #endif
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

  #ifdef ENABLE_TOC
  DYN_STR_APPEND_PLAIN(&ctx.str, "<nav><ul>");
  dyn_str_append(&ctx.str, ctx.toc.p, ctx.toc.len);
  DYN_STR_APPEND_PLAIN(&ctx.str, "</ul></nav>");
  #endif
  DYN_STR_APPEND_PLAIN(&ctx.str, "</body></html>");

  fprintf(stdout, "%.*s\n", (int) ctx.str.len, ctx.str.p);

  dyn_str_free(&ctx.str);

  #ifdef ENABLE_TOC
  dyn_str_free(&ctx.toc);
  #endif

  munmap(src, len);

  close(fd);
}
