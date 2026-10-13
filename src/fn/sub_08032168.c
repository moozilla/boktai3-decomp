#include "global.h"
int sub_08032168(const u8 *a, const u8 *b, int n) {
  int i = 0;
  for (; i < n; i++) {
    int c = a[i];
    if (!c) {
      if (!*b)
        return 0;
      return -1;
    }
    if (c != *b)
      return 1;
    b++;
  }
  return 0;
}
