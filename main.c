#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#include <cmarkdown.h>

typedef struct {
  char *p;
  size_t len;
  size_t capacity;
} dyn_str_t;

typedef struct {
  cmark_ctx_t cmark;
  dyn_str_t str;
  cmark_elem_t current;
} ctx_t;

static int compile_inline(ctx_t *ctx, dyn_str_t *dst);

static inline int
dyn_str_init(dyn_str_t *s, size_t initial_cap)
{
  s->p = malloc(initial_cap);
  if (0 == s->p) {
    return -1;
  }

  s->len = 0;
  s->capacity = initial_cap;

  return 0;
}

static inline int
dyn_str_append(dyn_str_t *s, const char *str, size_t len)
{
  char *new_p;
  size_t new_capacity = s->capacity;

  if (s->len + len >= s->capacity) {
    do {
      new_capacity *= 2;
    } while (s->len + len >= new_capacity);

    new_p = realloc(s->p, new_capacity);
    if (0 == new_p) {
      return -1;
    }

    s->p = new_p;
    s->capacity = new_capacity;
  }

  memcpy(s->p + s->len, str, len);
  s->len += len;

  return 0;
}

static inline void
dyn_str_free(dyn_str_t *s)
{
  free(s->p);
  s->len = 0;
  s->capacity = 0;
}

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

  dyn_str_append(dst, "<a href=\"", 9);
  dyn_str_append(dst, ctx->current.anchor_link.p, ctx->current.anchor_link.len);
  dyn_str_append(dst, "\">", 2);
  dyn_str_append(dst, content.p, content.len);
  dyn_str_append(dst, "</a>", 4);

  dyn_str_free(&content);

  return 0;
}

static inline int
compile_code_inline(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_append(dst, "<code>", 6);
  dyn_str_append(dst, ctx->current.code_inline.p, ctx->current.code_inline.len);
  dyn_str_append(dst, "</code>", 7);

  return 0;
}

static inline int
compile_code_multiline(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_append(dst, "<pre class=\"code-", 17);
  dyn_str_append(dst, ctx->current.code_multiline.lang.p, ctx->current.code_multiline.lang.len);
  dyn_str_append(dst, "\">", 2);
  dyn_str_append(dst, ctx->current.code_multiline.content.p, ctx->current.code_multiline.content.len);
  dyn_str_append(dst, "</pre>", 6);

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
  case CMARK_ELEM_CODE_MULTILINE:
    return compile_code_multiline(ctx, dst);
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

  dyn_str_append(dst, "<", 1);
  dyn_str_append(dst, tag, 3);

  while (CMARK_ELEM_BREAK != ctx->current.type && CMARK_ELEM_EOF != ctx->current.type) {
    ctx->current = cmark_next(&ctx->cmark);
    compile_inline(ctx, dst);
  }

  dyn_str_append(dst, "</", 2);
  dyn_str_append(dst, tag, 3);

  return 0;
}

static inline int
compile_list(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_append(dst, "<ul>", 4);

  cmark_next(&ctx->cmark);
  while (CMARK_ELEM_LIST_END != ctx->current.type) {
    dyn_str_append(dst, "<li>", 4);
    do {
      ctx->current = cmark_next(&ctx->cmark);
      if (CMARK_ELEM_LIST_END == ctx->current.type) {
        break;
      }
      compile_inline(ctx, dst);
    } while (CMARK_ELEM_LIST_ITEM != ctx->current.type);
    dyn_str_append(dst, "</li>", 5);
  }

  dyn_str_append(dst, "</ul>", 5);

  return 0;
}

static inline int
compile_paragraph(ctx_t *ctx, dyn_str_t *dst)
{
  dyn_str_append(dst, "<p>", 3);

  compile_inline(ctx, dst);
  while (CMARK_ELEM_BREAK != ctx->current.type && CMARK_ELEM_EOF != ctx->current.type) {
    ctx->current = cmark_next(&ctx->cmark);
    compile_inline(ctx, dst);
  }

  dyn_str_append(dst, "</p>", 4);

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
    default:
      compile_paragraph(&ctx, &ctx.str);
      break;
    }
  } while (CMARK_ELEM_EOF != ctx.current.type);
  end = clock();

  fprintf(stdout, "%.*s\n", (int) ctx.str.len, ctx.str.p);
  fprintf(stderr, "took %fs\n", ((double) (end - start)) / CLOCKS_PER_SEC);

  dyn_str_free(&ctx.str);

  munmap(src, len);

  close(fd);
}
