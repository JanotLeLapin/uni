#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <cmarkdown.h>

static int
compile_file(const char *in, const char *out)
{
  int fd;
  size_t len;
  char *src, title[256], *p;
  cmark_ctx_t ctx;
  cmark_elem_t e;
  pid_t pid;

  fprintf(stderr, "compiling: %s\n", in);

  fd = open(in, O_RDONLY);
  len = lseek(fd, 0, SEEK_END);
  src = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

  cmark_init_ctx(&ctx, src, len);
  e = cmark_next(&ctx);
  if (CMARK_ELEM_HEADING == e.type && 1 == e.heading) {
    p = title;
    while (CMARK_ELEM_BREAK != e.type) {
      e = cmark_next(&ctx);
      switch (e.type) {
      case CMARK_ELEM_PLAIN:
        memcpy(p, e.plain.p, e.plain.len);
        p += e.plain.len;
        break;
      default:
        break;
      }
    }
    *++p = '\0';
  } else {
    strcpy(title, "uni");
  }

  munmap(src, len);
  close(fd);

  pid = fork();
  if (0 == pid) {
    fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
      perror("open");
      exit(1);
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);

    execlp("uni", "uni", "--title", title, in, NULL);
    perror("execlp");
    exit(1);
  } else if (0 < pid) {
    wait(NULL);
  } else {
    perror("fork");
    return 1;
  }

  return 0;
}

static int
walk(const char *path)
{
  DIR *dir;
  struct dirent *entry;
  char fullpath[4096], target[4096];
  struct stat st;
  size_t i;

  dir = opendir(path);
  if (!dir) {
    perror("opendir");
    return 1;
  }

  while (NULL != (entry = readdir(dir))) {
    if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) {
      continue;
    }

    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    if (-1 == lstat(fullpath, &st)) {
      perror("lstat");
      continue;
    }

    if (S_ISDIR(st.st_mode)) {
      walk(fullpath);
    } else {
      i = 0;
      while ('\0' != fullpath[i]) {
        if ('/' == fullpath[i]) {
          snprintf(target, sizeof(target), "out/%.*s", (int) i, fullpath);
          mkdir(target, 0755);
        } else if ('.' == fullpath[i] && !strncmp(".md", fullpath + i, 3)) {
          snprintf(target, sizeof(target), "out/%.*s.html", (int) i, fullpath);
          compile_file(fullpath, target);
          break;
        }
        i++;
      }
    }
  }

  return 0;
}

int
main()
{
  if (walk(".")) {
    fprintf(stderr, "something went wrong\n");
  }
  return 0;
}
