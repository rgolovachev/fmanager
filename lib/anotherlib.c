#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int can_run(const char *file_path) {
  const char *ext = strrchr(file_path, '.');
  if (!ext || *(ext + 1) == 0) {
    return 0;
  }
  ++ext;
  if (strcmp(ext, "py") == 0) {
    return 1;
  } else {
    return 0;
  }
}

int run(const char *file_path, const char *name) {
  const char *ext = strrchr(file_path, '.');
  if (!ext || *(ext + 1) == 0) {
    return 0;
  }
  ++ext;
  if (strcmp(ext, "py") != 0) {
    return -1;
  }
  pid_t num = fork();
  if (num == 0) {
    execlp("rm", "rm", file_path, NULL);
    return -1;
  } else if (num > 0) {
    pid_t res = wait(NULL);
    if (res == -1) {
      return -1;
    }
  } else {
    return -1;
  }
  return 0;
}
