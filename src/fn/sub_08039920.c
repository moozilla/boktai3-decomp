// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Motion {
  u8 pad0[6];
  u16 velocity;
}
;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08039920(struct Motion*arg,int baseArg,int mask) {
  struct Motion*p=arg;
  register int base asm("r1")=baseArg;
  register u16*table asm("r6")=gUnk_0203B400;
  register u32*state asm("r5")=&gUnk_03005308;
  register u32 limit asm("r4");
  register u32 index asm("r0");
  register int random asm("r3");
  asm("" : "+r"(table),"+r"(state));
  index=*state+1;
  limit=1023;
  index&=limit;
  asm("" : "+r"(index),"+r"(limit));
  random=index*2;
  asm("" : "+r"(random));
  random=*(u16*)(random+(u32)table);
  random&=mask;
  asm("" : "+r"(random));
  base+=random;
  asm("" : "+r"(base),"+r"(p));
  index=(index+1)&limit;
  *state=index;
  {
    int sign=*(u8*)&table[index];
    if(sign<=127)p->velocity=base;
    else  {
      register int negative asm("r0")=-base;
      asm("" : "+r"(negative));
      p->velocity=negative;
    }
  }
}
