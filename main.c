#include "fileops.h"
#include "gui.h"
#include <dirent.h>
#include <dlfcn.h>
#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum {
  PUSH_ENTER = 10,
  PUSH_DELETE = 68,
  LEN_SZ = 21,
  PUSH_CUT = 88,
  PUSH_PASTE = 86,
  PUSH_COPY = 67,
  PUSH_HIDE = 72,
  PUSH_EXIT = 81
};

int main(void) {
  InitScreen();

  char prev_path[PATH_MAX], cur_path[PATH_MAX], cur_item[PATH_MAX];
  char *ptr_path = cur_path, *ptr_item = cur_item;

  struct CBItem clipboard;
  clipboard.has_val = 0;

  ptr_path = getcwd(cur_path, PATH_MAX);
  if (!ptr_path) {
    perror("FATAL ERROR");
    return 1;
  }

  int choice = 0, start = 0, rows = 0, cols = 0, cnt = 0;
  char show_hidden = 1;

  struct dirent **dir;

  while (true) {
    if (show_hidden) {
      cnt = scandir(cur_path, &dir, NULL, alphasort);
    } else {
      cnt = scandir(cur_path, &dir, DiscardHidden, alphasort);
    }
    if (cnt < 0) {
      DrawStatus(rows, cols,
                 "ERROR: can't open directory. Press any key to continue...");
      getch();
      memcpy(cur_path, prev_path, PATH_MAX);
      continue;
    }

    clear();
    getmaxyx(stdscr, rows, cols);

    if (choice - start >= rows - 2) {
      choice = start + rows - 3;
    }

    char size_str[LEN_SZ];
    int mx_num_size = 0, mx_name_size = 0;
    struct stat stat_buf;
    for (int i = 0; i < cnt; ++i) {
      memcpy(cur_item, cur_path, PATH_MAX);
      Concat(cur_item, dir[i]->d_name);
      lstat(cur_item, &stat_buf);
      snprintf(size_str, LEN_SZ, "%ld", stat_buf.st_size);
      if (mx_num_size < strlen(size_str)) {
        mx_num_size = strlen(size_str);
      }
      if (mx_name_size < strlen(dir[i]->d_name)) {
        mx_name_size = strlen(dir[i]->d_name);
      }
    }

    DrawHeader(mx_num_size, mx_name_size, cols);

    for (int i = start;
         i < start + ((cnt - start < rows - 2) ? cnt - start : rows - 2); ++i) {
      DrawName(dir[i], i + 1 - start, i == choice, &clipboard);
      memcpy(cur_item, cur_path, PATH_MAX);
      Concat(cur_item, dir[i]->d_name);
      lstat(cur_item, &stat_buf);
      DrawMisc(i + 1 - start, i == choice, mx_num_size, mx_name_size, cols,
               &stat_buf);
    }

    DrawStatus(rows, cols, cur_path);
    refresh();

    if (cnt > 0) {
      memcpy(cur_item, cur_path, sizeof(cur_path));
      Concat(cur_item, dir[choice]->d_name);
    }

    switch (getch()) {
    case KEY_UP:
      if (cnt <= 0) {
        break;
      }
      if (choice) {
        --choice;
        if (choice < start) {
          --start;
        }
      }
      break;
    case KEY_DOWN:
      if (choice != cnt - 1) {
        ++choice;
        if (choice - start == rows - 2) {
          ++start;
        }
      }
      break;
    case PUSH_ENTER:
      if (cnt <= 0) {
        break;
      }
      if (dir[choice]->d_type == DT_DIR) {
        if (strcmp(dir[choice]->d_name, ".") == 0) {
          break;
        } else if (strcmp(dir[choice]->d_name, "..") == 0) {
          if (strcmp(dir[choice]->d_name, "/") == 0) {
            break;
          }
          memcpy(prev_path, cur_path, PATH_MAX);
          size_t len = strlen(cur_path);
          while (cur_path[len - 1] != '/') {
            cur_path[len - 1] = 0;
            --len;
          }
          if (len > 1) {
            cur_path[len - 1] = 0;
            --len;
          }
          choice = 0;
          start = 0;
        } else {
          memcpy(prev_path, cur_path, PATH_MAX);
          memcpy(cur_path, cur_item, PATH_MAX);
          choice = 0;
          start = 0;
        }
      } else {
        DIR *stdir = opendir(".");
        if (!stdir) {
          DrawStatus(rows, cols,
                     "ERROR: Can't load library. Press any key to continue...");
          getch();
          break;
        }

        struct dirent *stdir_ent;
        void *handle = NULL;
        int (*can_run)(const char *) = NULL;
        int (*run)(const char *, const char *) = NULL;
        char found = 0;
        while ((stdir_ent = readdir(stdir))) {
          const char *ext = strrchr(stdir_ent->d_name, '.');
          if (!ext || *(ext + 1) == 0) {
            continue;
          }
          ++ext;
          if (strcmp(ext, "so") == 0) {
            char lib_path[PATH_MAX] = "./";
            strcat(lib_path, stdir_ent->d_name);

            handle = dlopen(lib_path, RTLD_LAZY);
            if (!handle) {
              continue;
            }
            can_run = dlsym(handle, "can_run");
            if (!can_run) {
              continue;
            }
            run = dlsym(handle, "run");
            if (!run) {
              continue;
            }
            found = 1;
            break;
          }
        }
        closedir(stdir);

        if (!found) {
          DrawStatus(rows, cols,
                     "ERROR: Can't load library. Press any key to continue...");
          getch();
          break;
        }

        int res = can_run(cur_item);
        if (res == 1) {
          endwin();
          res = run(cur_item, dir[choice]->d_name);
          if (res == -1) {
            DrawStatus(rows, cols,
                       "ERROR: Can't run another program. Press any key to "
                       "continue...");
            getch();
          } else if (res == 1) {
            getch();
          }
          InitScreen();
        }
        dlclose(handle);
      }
      break;
    case PUSH_DELETE:
      if (cnt <= 0) {
        break;
      }
      if (strcmp(dir[choice]->d_name, ".") != 0 &&
          strcmp(dir[choice]->d_name, "..") != 0) {
        if (clipboard.has_val && dir[choice]->d_ino == clipboard.id) {
          break;
        }
        if (dir[choice]->d_type == DT_DIR) {
          if (RemoveDirectory(cur_item) == -1) {
            DrawStatus(rows, cols,
                       "ERROR: Can't delete a directory. Press any key to "
                       "continue...");
            getch();
            break;
          }
        } else {
          if (remove(cur_item) == -1) {
            DrawStatus(
                rows, cols,
                "ERROR: Can't delete a file. Press any key to continue...");
            getch();
            break;
          }
        }
        if (choice == cnt - 1) {
          --choice;
          if (choice < start) {
            start -= (rows - 2);
            if (start < 0) {
              start = 0;
            }
          }
        }
      }
      break;
    case PUSH_CUT:
      if (cnt <= 0) {
        break;
      }
      if (strcmp(dir[choice]->d_name, ".") != 0 &&
          strcmp(dir[choice]->d_name, "..") != 0) {
        FillToCb(&clipboard, dir[choice], cur_item, 1);
      }
      break;
    case PUSH_COPY:
      if (cnt <= 0) {
        break;
      }
      if (strcmp(dir[choice]->d_name, ".") != 0 &&
          strcmp(dir[choice]->d_name, "..") != 0) {
        FillToCb(&clipboard, dir[choice], cur_item, 2);
      }
      break;
    case PUSH_PASTE:
      if (clipboard.has_val) {
        int res = PasteFile(&clipboard, cur_path);
        if (res == 0 && clipboard.has_val == 1) {
          // может быть == 2 а не 1 тогда это copy и нельзя удалять
          if (remove(clipboard.path) == -1) {
            DrawStatus(rows, cols,
                       "ERROR: Can't delete the old file. Press any key to "
                       "continue...");
            getch();
          }
        } else if (res == 1) {
          DrawStatus(rows, cols,
                     "ERROR: There is a file with the same name in directory. "
                     "Press any key to continue...");
          getch();
        } else if (res == -1) {
          DrawStatus(
              rows, cols,
              "ERROR: Can't insert a file. Press any key to continue...");
          getch();
        }
        clipboard.has_val = 0;
      }
      break;
    case PUSH_HIDE:
      if (show_hidden) {
        show_hidden = 0;
      } else {
        show_hidden = 1;
      }
      choice = 0;
      start = 0;
      break;
    case PUSH_EXIT:
      EndLoop(dir, cnt);
      endwin();
      return 0;
    }
    EndLoop(dir, cnt);
  }
  endwin();
  return 0;
}