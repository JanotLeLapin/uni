#include <cmarkdown.h>
#include <stdio.h>

#define COMPILE_CHILDREN(i, node) for (i = 0; i < node.children_count; i++) compile_node(node.children[i]);

void
compile_node(struct CMarkNode node)
{
  size_t i;

  switch (node.type) {
    case CMARK_ROOT:
      printf("<main>");
      COMPILE_CHILDREN(i, node);
      printf("</main>");
      break;
    case CMARK_NULL:
      break;
    case CMARK_HEADER:
      printf("<h%d>", node.data.header.level);
      COMPILE_CHILDREN(i, node);
      printf("</h%d>", node.data.header.level);
      break;
    case CMARK_PARAGRAPH:
      printf("<p>");
      COMPILE_CHILDREN(i, node);
      printf("</p>");
      break;
    case CMARK_ANCHOR:
      printf("<a href=\"%s\">", node.data.anchor.href);
      COMPILE_CHILDREN(i, node);
      printf("</a>");
      break;
    case CMARK_CODE:
      if (node.data.code.is_block) {
        printf("<pre><code>%s</code></pre>", node.data.code.content);
      } else {
        printf("<code>%s</code>", node.data.code.content);
      }
      break;
    case CMARK_PLAIN:
      printf("%s", node.data.plain);
      break;
    case CMARK_WHITESPACE:
      printf(" ");
      break;
    case CMARK_BREAK:
      printf("</br>");
      break;
  }
}

int
main(void)
{
  struct CMarkContext *ctx = cmark_create_context(stdin);
  struct CMarkNode root = cmark_parse(ctx);

  compile_node(root);
  cmark_free_node(root);

  return 0;
}
