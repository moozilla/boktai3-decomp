#include "global.h"
struct Game {
  u8 pad0[0x5A8];
  s16 form;
  u8 pad5aa[0x2BE];
  u32 flags;
};
extern struct Game *gUnk_02000710;
static inline u8 flag(u32 *ptr, u32 mask) {
  if (*ptr & mask)
    return 1;
  return 0;
}
// MATCHING_CONCESSION: register-pinning
int sub_080322B4(void) {
  int v;
  register u32 mask asm("r2") = 128;
  register struct Game **global asm("r1") = &gUnk_02000710;
  register int result asm("r0");
  asm("" : "+r"(mask), "+r"(global));
  if (flag(&(*global)->flags, mask)) {
    result = 28;
    asm("" : "+r"(result));
    goto done;
  }
  v = (*global)->form;
  if (v == 1)
    goto fallback;
  asm("" : "+r"(v));
  if (v <= 1)
    goto fallback;
  asm("" : "+r"(v));
  if (v != 6)
    goto fallback;
  result = 4;
  asm("" : "+r"(result));
  goto done;
fallback:
  result = 0;
  asm("" : "+r"(result));
done:
  return result;
}
