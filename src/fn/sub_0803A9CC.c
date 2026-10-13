// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Drawing {
  u8 pad0[8];
  u32 flags;
  u8 padc[0x14];
  u16 x,y;
  u8 pad24[0x3c];
}
;
struct Display {
  u8 pad0[0x18];
  u32 timer;
  u8 state,entered,finished,pad1f;
  u8 resource[32];
  struct Drawing drawings[4];
}
;
void sub_0821980C(void*,void*,int,int);
void sub_0822B2F8(int);
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0803A9CC(struct Display*arg) {
  register struct Display*p asm("r6")=arg;
  struct Drawing*q;
  if(p->entered) {
    p->entered=0;
    q=p->drawings;
    asm("" : "+r"(q));
    sub_0821980C(q,p->resource,9,0);
    q->flags&=~1;
    sub_0822B2F8(1452);
  }
  if(p->timer<=7) {
    register u16*table asm("r5");
    register u32*state asm("r2");
    register u32 limit asm("r4");
    register u32 index asm("r0");
    register int value asm("r1");
    register int mask asm("r3");
    q=p->drawings;
    asm("" : "+r"(q));
    table=gUnk_0203B400;
    state=&gUnk_03005308;
    asm("" : "+r"(table),"+r"(state));
    index=*state+1;
    limit=1023;
    index&=limit;
    asm("" : "+r"(index));
    *state=index;
    value=table[index];
    mask=3;
    asm("" : "+r"(value),"+r"(mask));
    value&=mask;
    value-=2;
    q->x=value;
    index=(index+1)&limit;
    *state=index;
    index=table[index]&mask;
    asm("" : "+r"(index));
    index-=2;
    q->y=index;
  }
  asm("" ::: "memory");
  if(p->timer==8) {
    q=p->drawings;
    asm("" : "+r"(q));
    q->x=0;
    q->y=0;
  }
  if(p->finished) {
    p->finished=0;
    p->state=0;
    p->entered=1;
    p->timer=0;
  }
  else p->timer++;
}
