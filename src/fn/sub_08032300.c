#include "global.h"
struct Game {
  u8 pad0[0x5A8];
  s16 form;
  u8 pad5aa[0x2BE];
  u32 flags;
};
extern struct Game *gUnk_02000710;
// MATCHING_CONCESSION: register-pinning
int sub_08032300(void) {
  int v = gUnk_02000710->form;
  register int result asm("r0");
  if (v == 1)
    goto fallback;
  asm("" : "+r"(v));
  if (v > 1)
    goto fallback;
  asm("" : "+r"(v));
  if (v != 0)
    goto fallback;
  result = 31;
  asm("" : "+r"(result));
  goto done;
fallback:
  result = 32;
  asm("" : "+r"(result));
done:
  return result;
}
