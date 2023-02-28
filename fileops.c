#include "fileops.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int RemoveDirectory(const char *path) {
  DIR *dir = opendir(path);
  if (!dir) {
    return -1;
  }
  struct dirent *ent;
  char buf[PATH_MAX];
  char *ptr_buf = buf;
  errno = 0;
  while ((ent = readdir(dir))) {
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
      continue;
    }
    memcpy(buf, path, PATH_MAX);
    if (strcmp(path, "/") != 0) {
      ptr_buf = strcat(ptr_buf, "/");
    }
    ptr_buf = strcat(ptr_buf, ent->d_name);
    if (ent->d_type == DT_DIR) {
      int res = RemoveDirectory(buf);
      if (res == -1) {
        return -1;
      }
    } else {
      if (remove(buf) == -1) {
        return -1;
      }
    }
    errno = 0;
  }
  if (errno != 0) {
    return -1;
  }
  closedir(dir);
  if (rmdir(path) == -1) {
    return -1;
  }
  return 0;
}

int PasteFile(struct CBItem *cb, const char *new_dir) {
  DIR *dir = opendir(new_dir);
  if (!dir) {
    return -1;
  }

  struct dirent *ent;
  while ((ent = readdir(dir))) {
    if (strcmp(ent->d_name, cb->filename) == 0) {
      closedir(dir);
      return 1;
    }
  }
  closedir(dir);

  size_t cnt = 0;
  char dst[PATH_MAX], buf[BUF_SZ];

  memcpy(dst, new_dir, PATH_MAX);
  Concat(dst, cb->filename);

  FILE *input = fopen(cb->path, "rb");
  if (!input) {
    return -1;
  }
  FILE *output = fopen(dst, "wb");
  if (!output) {
    return -1;
  }
  while ((cnt = fread(buf, sizeof(char), sizeof(buf) / sizeof(char), input))) {
    fwrite(buf, sizeof(char), cnt, output);
    if (ferror(output) != 0) {
      return -1;
    }
  }
  if (ferror(input) != 0) {
    return -1;
  }

  fclose(input);
  fclose(output);
  return 0;
}

void EndLoop(struct dirent **arr, int cnt) {
  for (int i = 0; i < cnt; ++i) {
    free(arr[i]);
  }
  free(arr);
}

void Concat(char *path, const char *extra) {
  if (strcmp(path, "/") != 0) {
    path = strcat(path, "/");
  }
  path = strcat(path, extra);
}

int DiscardHidden(const struct dirent *ent) {
  if (ent->d_name[0] == '.') {
    return 0;
  }
  return 1;
}

void FillToCb(struct CBItem *cb, struct dirent *ent, char *buf, int type) {
  if (ent->d_type != DT_DIR) {
    memcpy(cb->path, buf, PATH_MAX);
    memcpy(cb->filename, ent->d_name, PATH_MAX);
    cb->id = ent->d_ino;
    cb->has_val = type; // 1 if cut op, 2 if copy op
  }
}