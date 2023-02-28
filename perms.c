#include <stdio.h>

#include "sys/stat.h"

enum { LEN = 9, CNT_BITS = 12 };

const char *perms_to_str(char *buf, size_t size, int perms) {
  if (size == 0) {
    return buf;
  }
  size_t cnt = 0;
  unsigned mask = perms & ((1 << CNT_BITS) - 1);
  if (size > LEN) {
    cnt = LEN;
  } else {
    cnt = size - 1;
  }
  for (size_t i = 0; i < cnt; ++i) {
    if (i == 0) {
      if (mask & S_IRUSR) {
        buf[i] = 'r';
      } else {
        buf[i] = '-';
      }
    } else if (i == 1) {
      if (mask & S_IWUSR) {
        buf[i] = 'w';
      } else {
        buf[i] = '-';
      }
    } else if (i == 2) {
      if ((mask & S_ISUID) && ((mask & S_IXGRP) || (mask & S_IXOTH))) {
        buf[i] = 's';
      } else if (mask & S_IXUSR) {
        buf[i] = 'x';
      } else {
        buf[i] = '-';
      }
    } else if (i == 3) {
      if (mask & S_IRGRP) {
        buf[i] = 'r';
      } else {
        buf[i] = '-';
      }
    } else if (i == 4) {
      if (mask & S_IWGRP) {
        buf[i] = 'w';
      } else {
        buf[i] = '-';
      }
    } else if (i == 5) {
      if ((mask & S_ISGID) && (mask & S_IXOTH)) {
        buf[i] = 's';
      } else if (mask & S_IXGRP) {
        buf[i] = 'x';
      } else {
        buf[i] = '-';
      }
    } else if (i == 6) {
      if (mask & S_IROTH) {
        buf[i] = 'r';
      } else {
        buf[i] = '-';
      }
    } else if (i == 7) {
      if (mask & S_IWOTH) {
        buf[i] = 'w';
      } else {
        buf[i] = '-';
      }
    } else if (i == 8) {
      if ((mask & S_ISVTX) && (mask & S_IWOTH) && (mask & S_IXOTH)) {
        buf[i] = 't';
      } else if (mask & S_IXOTH) {
        buf[i] = 'x';
      } else {
        buf[i] = '-';
      }
    }
  }

  buf[cnt] = 0;
  return buf;
}