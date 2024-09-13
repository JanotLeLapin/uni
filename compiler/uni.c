#include <cmarkdown.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define RES_BUF_SIZE 512
#define HEADER_BUF_SIZE 128
#define ANCHOR_BUF_SIZE 256
#define CODE_BUF_SIZE 2048

enum Flag {
  FLAG_PARAGRAPH = 1 << 1,
  FLAG_ANCHOR = 1 << 2,
  FLAG_INLINE_CODE = 1 << 3,
  FLAG_MULTILINE_CODE = 1 << 4,
};

char *uni_highlight(const char *source, const char *lang);
void uni_free_buffer(char *ptr);

struct Header {
  char level;
  size_t text_end;
  char text[128];
  char id[128];
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

int
main(void)
{
  struct CMarkParser p = cmark_new_parser(stdin);
  struct CMarkElem e;
  size_t header_count = 0;
  size_t header_size = 8;
  struct Header *headers = malloc(sizeof(struct Header) * header_size);
  struct Header header;
  unsigned short flags = 0;
  char is_newline = 1;
  char is_list = 0;
  size_t res_buf_end = 0, header_buf_end = 0, anchor_buf_end = 0, code_buf_end = 0;
  char res_buf[RES_BUF_SIZE], header_buf[HEADER_BUF_SIZE], anchor_buf[ANCHOR_BUF_SIZE], code_buf[CODE_BUF_SIZE];
  char code_lang[16];
  size_t i;

  char *ptr;
  size_t *end, size;

  printf("<!DOCTYPE html><html><head><link rel=\"stylesheet\" href=\"/uni/static/app.css\"/><meta charset=\"utf-8\"/></head><body><main>");

  while (1) {
    e = cmark_next(&p);

    if (is_newline && is_list && CMARK_LIST_ITEM != e.type) {
      printf("</ul>");
      is_list = 0;
    }

    is_newline = e.type == CMARK_BREAK;

    if (flags & FLAG_ANCHOR) {
      ptr = anchor_buf + anchor_buf_end;
      end = &anchor_buf_end;
      size = ANCHOR_BUF_SIZE;
    } else if (flags & (0b111 << 8)) {
      ptr = header_buf + header_buf_end;
      end = &header_buf_end;
      size = HEADER_BUF_SIZE;
    } else if (flags & (FLAG_MULTILINE_CODE)) {
      ptr = code_buf + code_buf_end;
      end = &code_buf_end;
      size = CODE_BUF_SIZE;
    } else {
      ptr = res_buf + res_buf_end;
      end = &res_buf_end;
      size = RES_BUF_SIZE;
    }

    switch (e.type) {
      case CMARK_HEADER:
        flags |= (e.data.header_level & 0b111) << 8;
        header.level = e.data.header_level;
        header.text_end = 0;
        header.text[0] = '\0';
        header.id[0] = '\0';
        continue;
      case CMARK_LIST_ITEM:
        if (!is_list) {
          printf("<ul>");
        }
        printf("<li>");
        is_list = 1;
        continue;
      case CMARK_BLOCKQUOTE_START:
        printf("<blockquote>");
        continue;
      case CMARK_BLOCKQUOTE_END:
        printf("</blockquote>");
        continue;
      case CMARK_PLAIN:
        if (!flags) {
          flags |= FLAG_PARAGRAPH;
          printf("<p>");
        }

        if (flags & (0b111 << 8)) {
          memcpy(header.text + header.text_end, e.data.plain.ptr, e.data.plain.length);
          header.text_end += e.data.plain.length;
        }

        snprintf(ptr, size - *end, "%.*s", (int) e.data.plain.length, e.data.plain.ptr);
        *end += e.data.plain.length;
        continue;
      case CMARK_ANCHOR_START:
        flags |= FLAG_ANCHOR;
        continue;
      case CMARK_ANCHOR_END:
        flags &= ~FLAG_ANCHOR;
        if (flags & (0b111 << 8)) {
          ptr = header_buf + header_buf_end;
          end = &header_buf_end;
          size = HEADER_BUF_SIZE;
        } else {
          ptr = res_buf + res_buf_end;
          end = &res_buf_end;
          size = RES_BUF_SIZE;
        }
        snprintf(ptr, size - *end, "<a href=\"%.*s\">%.*s</a>", (int) e.data.anchor_end_href.length, e.data.anchor_end_href.ptr, (int) anchor_buf_end, anchor_buf);
        *end += 15 + e.data.anchor_end_href.length + anchor_buf_end;
        anchor_buf_end = 0;
        continue;
      case CMARK_CODE_START:
        if (e.data.code.is_multi_line) {
          flags |= FLAG_MULTILINE_CODE;

          size_t len = strlen(e.data.code.lang);
          memcpy(code_lang, e.data.code.lang, len);
          code_lang[len] = '\0';

          printf("<pre><code>");
        } else {
          flags |= FLAG_INLINE_CODE;

          snprintf(ptr, size - *end, "<code>");
          *end += 6;
        }
        continue;
      case CMARK_CODE_END:
        if (flags & FLAG_MULTILINE_CODE) {
          flags &= ~FLAG_MULTILINE_CODE;
          code_buf[code_buf_end] = '\0';
          char *highlighted = uni_highlight(code_buf, code_lang);
          printf("%s", highlighted);
          uni_free_buffer(highlighted);
          code_buf_end = 0;

          printf("</code></pre>");
        } else {
          flags &= ~FLAG_INLINE_CODE;
          snprintf(ptr, size - *end, "</code>");
          *end += 7;
        }
        continue;
      case CMARK_BREAK:
        if (flags & FLAG_MULTILINE_CODE) {
          snprintf(code_buf + code_buf_end, size - code_buf_end, "\n");
          code_buf_end += 1;
          continue;
        }
        
        printf("%.*s", (int) res_buf_end, res_buf);
        res_buf_end = 0;
        if (flags & FLAG_PARAGRAPH) {
          printf("</p>");
        } else if (flags & (0b111 << 8)) {
          header.text[header.text_end] = '\0';
          kebab_case(header.id, header.text, 128);
          if (header_count >= header_size) {
            header_size *= 2;
            headers = realloc(headers, sizeof(struct Header) * header_size);
          }
          headers[header_count] = header;
          header_count++;

          printf("<h%d id=\"%s\">%.*s</h%d>", header.level, header.id, (int) header_buf_end, header_buf, header.level);
          header_buf_end = 0;
        }

        if (is_list) {
          printf("</li>");
        }

        flags = 0;
        continue;
      case CMARK_EOF:
        break;
      default:
        continue;
    }

    break;
  }
  
  printf("</main><nav id=\"contents\"><h3>Contenu</h3><ul>");
  for (i = 0; i < header_count; i++) {
    printf("<li><a href=\"#%s\">%s</a></li>", headers[i].id, headers[i].text);
  }
  free(headers);

  printf("</ul></nav></body></html>");

  return 0;
}
