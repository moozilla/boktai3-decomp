// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Item  {
  u8 state,entered,unused,count;
  u8 pad4[4];
  u16 timer;
  u8 pada[0x1a];
  u32 flags;
  u8 pad28[12];
  u16 value;
  u8 pad36[0x22];
  u32 flags2;
}
;
extern const u16 gUnk_0824DE7C[];
void sub_0821980C(void*,void*,int,int);
void sub_08039C24(void*p,struct Item*q,int index) {
  if(q->entered)q->entered=0;
  if(q->timer&1) {
    register int choice asm("r0");
    register void*draw asm("r3");
    q->value=q->count;
    q->flags&=~1;
    draw=(u8*)q+0x50;
    p=(u8*)p+0x3c;
    asm("" : "+r"(draw),"+r"(p));
    if(index>7)choice=(index&3)+4;
    else choice=index&3;
    asm("" : "+r"(choice));
    sub_0821980C(draw,p,gUnk_0824DE7C[choice],0);
    q->flags2&=~1;
  }
  else {
    q->flags|=1;
    q->flags2|=1;
  }
  if(q->timer>9) {
    register int one asm("r0")=1;
    register int zero asm("r1")=0;
    asm("" : "+r"(one),"+r"(zero));
    q->state=one;
    q->entered=one;
    one=0;
    asm("" : "+r"(one));
    q->timer=zero;
    q->count=one;
  }
  q->timer++;
}
