#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *p;
  size_t len;
  size_t capacity;
} dyn_str_t;

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

int
main()
{
  dyn_str_t str;

  dyn_str_init(&str, 2);
  dyn_str_append(&str, "hello", 5);
  dyn_str_append(&str, " ", 1);
  dyn_str_append(&str, "world", 5);

  fprintf(stderr, "%.*s\n", (int) str.len, str.p);

  dyn_str_free(&str);
}
