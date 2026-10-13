// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Item  {
  u8 state,entered,unused,count;
  u8 pad4[4];
  u16 timer;
  u8 pada[0xa6];
}
;
struct Owner  {
  u8 pad0[0x1c];
  u8 counts[4];
  u8 pad20[0x3c];
  struct Item items[16];
}
;
extern struct Owner *gUnk_02000100;
void sub_0803A6F8(void)  {
  register struct Owner *p asm("r5") = gUnk_02000100;
  if (p)  {
    register int i asm("r4")=0;
    int state=4;
    register int zero asm("r6")=0;
    register u8*count asm("r2")=p->counts;
    asm("" : "+r"(i),"+r"(state),"+r"(zero),"+r"(count));
    do  {
      if (*count)  {
        int value;
        register int offset asm("r0")=i*0xb0+0x5c;
        register struct Item*q asm("r1");
        asm("" : "+r"(offset));
        q=(struct Item*)((u8*)p+offset);
        q->state = state;
        q->entered = 1;
        q->timer = zero;
        value = *count;
        {
          register int clear asm("r3")=0;
          asm("" : "+r"(clear));
          if (value > 10) value = 10;
          q->count = value;
          q->unused = clear;
        }
        *count = zero;
      }
      count++;
      i++;
    }
    while (i <= 3);
  }
}
