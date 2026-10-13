// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Vm {
  u8 pad[0x1C];
  const u8 *pc;
};
u16 sub_08032070(const u8 *);
int sub_080321CC(struct Vm *p) {
  const u8 *s = p->pc;
  int n = 0;
  u32 c;
  goto test;
loop:
  c <<= 24;
  asm("" : "+r"(c));
  if ((s32)c < 0)
    goto wide;
  c >>= 24;
  if (c == '<') {
    do {
      s++;
      c = *s;
    } while (c != '>' && c && c != '\n');
    goto next;
  }
  if (c == 0x1F) {
    register u32 code asm("r2") = (u16)sub_08032070(s);
    asm("" : "+r"(code));
    if (code != 0xFFFF) {
      {
        register u32 mask asm("r0") = 0xFF;
        register u32 bits asm("r1");
        asm("" : "+r"(mask));
        mask <<= 8;
        asm("" : "+r"(mask));
        bits = mask;
        asm("" : "+r"(bits));
        mask = code;
        asm("" : "+r"(mask));
        mask &= bits;
        asm("" : "+r"(mask));
        if (mask)
          goto wide;
      }
      n++;
      goto nextWide;
    }
  }
  n++;
next:
  s++;
  goto limit;
wide:
  n += 2;
nextWide:
  s += 2;
limit:
  if (n > 254) {
    n = 255;
    goto done;
  }
test:
  c = *s;
  if (c)
    goto loop;
done:
  return n;
}
