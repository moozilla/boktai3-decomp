// MATCHING_CONCESSION: register-pinning
#include "global.h"
u16 *sub_08036CA4(void);
u16 sub_08036CAC(u8);
int sub_08036CC4(u16);
int sub_08036CC4(u16 code) {
  u16 *base = sub_08036CA4();
  u16 *table;
  register int i asm("r2") = 0;
  int result;
  asm("" : "+r"(i));
  table = base;
  do {
    if (*table == code) {
      {
        register u32 narrowed asm("r0") = i << 24;
        asm("" : "+r"(narrowed));
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