#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *exts[] = {"txt", "c", "cpp", "h", "S", "py", "sh", "gz", NULL};
char *progs[] = {"vim",     "vim", "vim",    "vim", "vim",
                 "python3", NULL,  "gunzip", NULL};

int can_run(const char *file_path) {
  const char *ext = strrchr(file_path, '.');
  if (!ext || *(ext + 1) == 0) {
    return 0;
  }
  ++ext;
  for (size_t i = 0; exts[i] != NULL; ++i) {
    if (strcmp(exts[i], ext) == 0) {
      return 1;
    }
  }
  return 0;
}

int run(const char *file_path, const char *name) {
  const char *ext = strrchr(file_path, '.');
  ++ext;
  for (size_t i = 0; exts[i] != NULL; ++i) {
    if (strcmp(exts[i], ext) == 0) {
      pid_t num = fork();
      if (num == 0) {
        execlp("clear", "clear", NULL);
        // если программа сюда дошла значит ошибка
        return -1;
      } else if (num > 0) {
        int res = wait(NULL);
        if (res == -1) {
          return -1;
        }
      } else {
        return -1;
      }

      num = fork();
      if (num == 0) {
        if (progs[i]) {
          execlp(progs[i], progs[i], file_path, NULL);
        } else {
          execlp(file_path, name, NULL);
        }
        return -1;
      } else if (num > 0) {
        int res = wait(NULL);
        if (res == -1) {
          return -1;
        }
      } else {
        return -1;
      }

      if (!progs[i] || strcmp(progs[i], "vim") != 0 && strcmp(ext, "gz") != 0) {
        num = fork();
        if (num == 0) {
          execlp("echo", "echo", "Type any key to exit...", NULL);
          return -1;
        } else if (num > 0) {
          int res = wait(NULL);
          if (res == -1) {
            return -1;
          }
        } else {
          return -1;
        }
        return 1;
      } else {
        return 0;
      }
    }
  }
  return -1;
}