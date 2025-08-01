#include <tree_sitter/api.h>

#include "uni.h"

#ifdef ENABLE_JSON_GRAMMAR
#include "highlights/json.h"
const TSLanguage *tree_sitter_json(void);
#endif

#ifdef ENABLE_PYTHON_GRAMMAR
#include "highlights/python.h"
const TSLanguage *tree_sitter_python(void);
#endif

#ifdef ENABLE_JAVA_GRAMMAR
#include "highlights/java.h"
const TSLanguage *tree_sitter_java(void);
#endif

#ifdef ENABLE_C_GRAMMAR
#include "highlights/c.h"
const TSLanguage *tree_sitter_c(void);
#endif

#ifdef ENABLE_BASH_GRAMMAR
#include "highlights/bash.h"
const TSLanguage *tree_sitter_bash(void);
#endif

#ifdef ENABLE_NIX_GRAMMAR
#include "highlights/nix.h"
const TSLanguage *tree_sitter_nix(void);
#endif

#define CMP_LANG(expected, actual) (sizeof(expected) == actual.len + 1 && !strncmp(expected, actual.p, actual.len))


typedef struct {
  const TSLanguage *ts;
  const char *highlights;
  size_t highlights_len;
} lang_t;

static inline char
find_lang(lang_t *dst, cmark_str_t lang)
{
  if (0) {}
  #ifdef ENABLE_PYTHON_GRAMMAR
  else if (
    CMP_LANG("py", lang)
    || CMP_LANG("python", lang)
  ) {
    dst->ts = tree_sitter_python();
    dst->highlights = highlights_python;
    dst->highlights_len = highlights_python_len;
    return 1;
  }
  #endif
  #ifdef ENABLE_JAVA_GRAMMAR
  else if (CMP_LANG("java", lang)) {
    dst->ts = tree_sitter_java();
    dst->highlights = highlights_java;
    dst->highlights_len = highlights_java_len;
    return 1;
  }
  #endif
  #ifdef ENABLE_C_GRAMMAR
  else if (CMP_LANG("c", lang)) {
    dst->ts = tree_sitter_c();
    dst->highlights = highlights_c;
    dst->highlights_len = highlights_c_len;
    return 1;
  }
  #endif
  #ifdef ENABLE_JSON_GRAMMAR
  else if (
    CMP_LANG("json", lang)
  ) {
    dst->ts = tree_sitter_json();
    dst->highlights = highlights_json;
    dst->highlights_len = highlights_json_len;
    return 1;
  }
  #endif
  #ifdef ENABLE_BASH_GRAMMAR
  else if (
    CMP_LANG("bash", lang)
    || CMP_LANG("sh", lang)
    || CMP_LANG("shell", lang)
  ) {
    dst->ts = tree_sitter_bash();
    dst->highlights = highlights_bash;
    dst->highlights_len = highlights_bash_len;
    return 1;
  }
  #endif
  #ifdef ENABLE_NIX_GRAMMAR
  else if (CMP_LANG("nix", lang)) {
    dst->ts = tree_sitter_nix();
    dst->highlights = highlights_nix;
    dst->highlights_len = highlights_nix_len;
    return 1;
  }
  #endif
  else {
    return 0;
  }
}

