#ifndef _UNI_HIGHLIGHT_H
#define _UNI_HIGHLIGHT_H

#include "uni.h"

#include <cmarkdown.h>

int highlight(dyn_str_t *dst, cmark_str_t lang, cmark_str_t code);

#endif
