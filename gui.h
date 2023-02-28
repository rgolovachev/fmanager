#ifndef FMANAGER_GUI_H
#define FMANAGER_GUI_H
#include "fileops.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum {
  REG_FONT = 0,
  CURS_FONT = 1,
  DIR_FONT = 2,
  STAT_FONT = 3,
  LNK_FONT = 4,
  FIFO_FONT = 5,
  SZ_OFF = 4,
  PERMS_OFF = 16,
  NAME_OFF = 4,
  PERMS_LEN = 9
};

void InitScreen();

void DrawHeader(int mx_num_size, int mx_name_size, int cols);

void DrawName(struct dirent *ent, int pos, int is_chosen, struct CBItem *cb);

void DrawMisc(int pos, int is_chosen, int mx_num_size, int mx_name_size,
              int cols, struct stat *stat_buf);

void DrawStatus(int rows, int cols, const char *str);

#endif