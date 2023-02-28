#include "gui.h"
#include "fileops.h"
#include "perms.h"
#include <dirent.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void InitScreen() {
  initscr();
  curs_set(0);
  keypad(stdscr, true);
  start_color();
  init_pair(REG_FONT, COLOR_WHITE, COLOR_BLACK);
  init_pair(CURS_FONT, COLOR_GREEN, COLOR_BLACK);
  init_pair(DIR_FONT, COLOR_CYAN, COLOR_BLACK);
  init_pair(STAT_FONT, COLOR_BLACK, COLOR_WHITE);
  init_pair(LNK_FONT, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(FIFO_FONT, COLOR_YELLOW, COLOR_BLACK);
}

void DrawHeader(int mx_num_size, int mx_name_size, int cols) {
  attron(COLOR_PAIR(STAT_FONT));
  mvhline(0, 0, ' ', cols);
  mvprintw(0, NAME_OFF, "Name");
  if (mx_name_size + NAME_OFF < cols - mx_num_size - SZ_OFF - 1) {
    mvprintw(0, cols - mx_num_size - SZ_OFF, "Size");
  }
  if (mx_name_size + NAME_OFF < cols - mx_num_size - PERMS_OFF - 1) {
    mvprintw(0, cols - mx_num_size - PERMS_OFF, "Perms");
  }
  attroff(COLOR_PAIR(STAT_FONT));
}

void AttrOn(struct dirent *ent, struct CBItem *cb) {
  if (ent->d_type == DT_DIR) {
    attron(A_BOLD | COLOR_PAIR(DIR_FONT));
  }
  if (cb->has_val && ent->d_ino == cb->id) {
    if (cb->has_val == 1) {
      attron(A_DIM);
    } else {
      attron(A_ITALIC);
    }
  }
  if (ent->d_type == DT_LNK) {
    attron(COLOR_PAIR(LNK_FONT));
  }
  if (ent->d_type == DT_FIFO) {
    attron(COLOR_PAIR(FIFO_FONT));
  }
}

void AttrOff(struct dirent *ent, struct CBItem *cb) {
  if (ent->d_type == DT_DIR) {
    attroff(A_BOLD | COLOR_PAIR(DIR_FONT));
  }
  if (cb->has_val && ent->d_ino == cb->id) {
    if (cb->has_val == 1) {
      attroff(A_DIM);
    } else {
      attroff(A_ITALIC);
    }
  }
  if (ent->d_type == DT_LNK) {
    attroff(COLOR_PAIR(LNK_FONT));
  }
  if (ent->d_type == DT_FIFO) {
    attroff(COLOR_PAIR(FIFO_FONT));
  }
}

void DrawName(struct dirent *ent, int pos, int is_chosen, struct CBItem *cb) {
  // рисую курсор
  if (!is_chosen) {
    attron(COLOR_PAIR(REG_FONT));
    mvprintw(pos, 0, "    ");
    attroff(COLOR_PAIR(REG_FONT));
  } else {
    attron(COLOR_PAIR(CURS_FONT));
    mvprintw(pos, 0, ">> ");
    attroff(COLOR_PAIR(CURS_FONT));
  }
  // рисую название сущности
  AttrOn(ent, cb);
  if (!is_chosen) {
    mvprintw(pos, NAME_OFF, "%s", ent->d_name);
  } else {
    mvprintw(pos, NAME_OFF - 1, "%s", ent->d_name);
  }
  AttrOff(ent, cb);
}

void DrawMisc(int pos, int is_chosen, int mx_num_size, int mx_name_size,
              int cols, struct stat *stat_buf) {
  char perms[PERMS_LEN + 1];
  const char *perms_str = perms_to_str(perms, PERMS_LEN + 1, stat_buf->st_mode);
  if (!is_chosen) {
    if (mx_name_size + NAME_OFF < cols - mx_num_size - SZ_OFF - 1) {
      mvprintw(pos, cols - mx_num_size - SZ_OFF, "%ld", stat_buf->st_size);
    }
    if (mx_name_size + NAME_OFF < cols - mx_num_size - PERMS_OFF - 1) {
      mvprintw(pos, cols - mx_num_size - PERMS_OFF, "%s", perms_str);
    }
  } else {
    if (mx_name_size + NAME_OFF < cols - mx_num_size - SZ_OFF - 1) {
      mvprintw(pos, cols - mx_num_size - SZ_OFF - 1, "%ld", stat_buf->st_size);
    }
    if (mx_name_size + NAME_OFF < cols - mx_num_size - PERMS_OFF - 1) {
      mvprintw(pos, cols - mx_num_size - PERMS_OFF - 1, "%s", perms_str);
    }
  }
}

void DrawStatus(int rows, int cols, const char *str) {
  attron(COLOR_PAIR(STAT_FONT));

  mvhline(rows - 1, 0, ' ', cols);
  mvprintw(rows - 1, 0, "%s", str);

  attroff(COLOR_PAIR(STAT_FONT));
}