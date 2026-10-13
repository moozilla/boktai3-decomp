#include "global.h"
u16 *sub_08036CA4(void);
u16 sub_08036CAC(u8);
int sub_08036CC4(u16);
int sub_08036CC4(u16 code) {
  u16 *base = sub_08036CA4();
  u16 *table;
  int i = 0;
  int result;
  table = base;
  do {
    if (*table == code) {
      {
        u32 narrowed = i << 24;
        result = narrowed >> 24;
      }
      goto done;
    }
    table++;
  } while (++i <= 255);
  result = 0;
done:
  return result;
}