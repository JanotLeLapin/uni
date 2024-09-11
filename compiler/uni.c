#include <cmarkdown.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
  size_t buf_end = 0;
  char buf[1024];
  size_t inline_buf_end = 0;
  char inline_buf[1024];
  char code_lang[16];
  size_t i;

  char *ptr;
  size_t *end;

  printf("<!DOCTYPE html><head><link rel=\"stylesheet\" href=\"/static/app.css\"/><meta charset=\"utf-8\"/></head><body>");

  while (1) {
    e = cmark_next(&p);

    ptr = (flags & FLAG_ANCHOR) ? (inline_buf + inline_buf_end) : (buf + buf_end);
    end = (flags & FLAG_ANCHOR) ? &inline_buf_end : &buf_end;

    switch (e.type) {
      case CMARK_HEADER:
        flags |= (e.data.header_level & 0b111) << 8;
        header.level = e.data.header_level;
        header.text_end = 0;
        header.text[0] = '\0';
        header.id[0] = '\0';
        printf("<h%d>", e.data.header_level);
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

        snprintf(ptr, 1023 - *end, "%.*s", (int) e.data.plain.length, e.data.plain.ptr);
        *end += e.data.plain.length;
        continue;
      case CMARK_ANCHOR_START:
        flags |= FLAG_ANCHOR;
        continue;
      case CMARK_ANCHOR_END:
        flags &= ~FLAG_ANCHOR;
        snprintf(buf + buf_end, 1023 - buf_end, "<a href=\"%.*s\">%.*s</a>", (int) e.data.anchor_end_href.length, e.data.anchor_end_href.ptr, (int) inline_buf_end, inline_buf);
        buf_end += 15 + e.data.anchor_end_href.length + inline_buf_end;
        inline_buf_end = 0;
        continue;
      case CMARK_CODE_START:
        if (e.data.code.is_multi_line) {
          flags |= FLAG_MULTILINE_CODE;

          size_t len = strlen(e.data.code.lang);
          memcpy(code_lang, e.data.code.lang, len);
          code_lang[len] = '\0';

          printf("<pre><code>");
          buf_end = 0;
        } else {
          flags |= FLAG_INLINE_CODE;

          snprintf(ptr, 1023 - *end, "<code>");
          *end += 6;
        }
        continue;
      case CMARK_CODE_END:
        if (flags & FLAG_MULTILINE_CODE) {
          flags &= ~FLAG_MULTILINE_CODE;
          buf[buf_end] = '\0';
          char *highlighted = uni_highlight(buf, code_lang);
          printf("%s", highlighted);
          uni_free_buffer(highlighted);
          buf_end = 0;

          printf("</code></pre>");
        } else {
          flags &= ~FLAG_INLINE_CODE;
          snprintf(ptr, 1023 - *end, "</code>");
          *end += 7;
        }
        continue;
      case CMARK_BREAK:
        if (flags & FLAG_MULTILINE_CODE) {
          snprintf(buf + buf_end, 1023 - buf_end, "\n");
          buf_end += 1;
          continue;
        }
        
        printf("%.*s", (int) buf_end, buf);
        buf_end = 0;
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

          printf("</h%d>", (flags >> 8) & 0b111);
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
  
  printf("<nav id=\"contents\"><ul>");
  for (i = 0; i < header_count; i++) {
    printf("<li><a href=\"%s\">%s</a></li>", headers[i].id, headers[i].text);
  }
  free(headers);

  printf("</ul></nav>");
  printf("</body>");

  return 0;
}
