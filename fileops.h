#ifndef FILEOPS_H
#define FILEOPS_H
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>

enum { BUF_SZ = 4096 };

struct CBItem {
  char path[PATH_MAX];
  char filename[PATH_MAX];
  ino_t id;
  char has_val;
};

int RemoveDirectory(const char *path);

int PasteFile(struct CBItem *cb, const char *new_dir);

void EndLoop(struct dirent **arr, int cnt);

void Concat(char *path, const char *extra);

int DiscardHidden(const struct dirent *ent);

void FillToCb(struct CBItem *cb, struct dirent *ent, char *buf, int type);

#endif