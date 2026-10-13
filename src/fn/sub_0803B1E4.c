// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Blob {
  u32 words[8];
};
struct Drawing {
  u8 bytes[0x60];
};
struct ResultMenu {
  u8 task[0x18];
  u32 timer;
  u8 state, entered;
  u8 pad1e[6];
  u8 result, step;
  u8 pad26[2];
  s16 remaining, shown;
  u8 pad2c[4];
  struct Blob primary, secondary;
  u8 pad70[0x48];
  struct Drawing draws[22];
};
extern u16 gUnk_03005260[];
int Mod(int, int);
void sub_0822B2F8(int);
void sub_082213B8(void *, void *, int, int, int, int, int, int, int);
static inline int clamp(int value) {
  if (value > 9999)
    value = 9999;
  else if (value < 0)
    value = 0;
  return value;
}
void sub_0803B1E4(struct ResultMenu *p) {
  register struct Blob *desc asm("r4");
  if (p->entered) {
    p->entered = 0;
    p->step = 2;
  }
  if (p->remaining > 8 && (gUnk_03005260[1] & 0x30F))
    p->step = 8;
  if (p->remaining >= p->step) {
    p->remaining = (u16)p->remaining - p->step;
    p->shown = (u16)p->shown + p->step;
    if (!Mod(p->timer, 2))
      sub_0822B2F8(677);
  } else {
    p->shown = (u16)p->shown + (u16)p->remaining;
    p->remaining = 0;
  }
  if (p->shown > 9999)
    p->shown = 9999;
  if (p->remaining < 0)
    p->remaining = 0;
  {
    register struct Drawing *draw asm("r3") = &p->draws[14];
    register int value asm("r2");
    asm("" : "+r"(draw));
    value = clamp(p->remaining);
    desc = &p->primary;
    sub_082213B8(draw, desc, value, 4, 51, 104, 96, 8, 0);
  }
  {
    register struct Drawing *draw asm("r3") = &p->draws[18];
    register int value asm("r2");
    asm("" : "+r"(draw));
    {
      register int maximum asm("r0");
      register struct Blob *descriptor asm("r1");
      value = p->shown;
      maximum = 9999;
      asm("" : "+r"(maximum));
      descriptor = desc;
      asm("" : "+r"(descriptor));
      if (value > maximum)
        value = maximum;
      else if (value < 0)
        value = 0;
      sub_082213B8(draw, descriptor, value, 4, 51, 104, 136, 8, 0);
    }
  }
  if (!p->remaining) {
    p->state = 5;
    p->entered = 1;
    p->timer = 0;
  } else
    p->timer++;
}
