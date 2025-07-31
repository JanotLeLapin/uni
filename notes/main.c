#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <cmarkdown.h>

static int
compile_file(const char *in, const char *out)
{
  pid_t pid = fork();
  int fd;

  if (0 == pid) {
    fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
      perror("open");
      exit(1);
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);

    execlp("uni", "uni", in, NULL);
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

int
main()
{
  compile_file("index.md", "index.html");
  return 0;
}
