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
int sub_080397AC(void);
int Div(int,int);
int Mod(int,int);
void sub_0803A844(struct Display*p) {
  struct Drawing *q;
  void*volatile resource;
  u16*volatile table;
  register int mask asm("r8");
  if(p->entered) {
    register int count asm("r4");
    p->entered=0;
    count=sub_080397AC();
    if(count==1)sub_0822B2F8(706);
    q=p->drawings;
    resource=p->resource;
    sub_0821980C(q,resource,11,0);
     {
      register u32 flags asm("r0")=q->flags;
      register int value asm("r1")=~1;
      asm("" : "+r"(flags),"+r"(value));
      mask=value;
      q->flags=flags&value;
    }
    q++;
    sub_0821980C(q,resource,(u16)(Div(count,10)+12),0);
     {
      register u32 flags asm("r0")=q->flags;
      register int value asm("r3");
      asm("" : "+r"(flags));
      value=mask;
      asm("" : "+r"(value));
      q->flags=flags&value;
    }
    q=&p->drawings[2];
    sub_0821980C(q,resource,(u16)(Mod(count,10)+12),0);
    q->flags&=mask;
    q->x=16;
     {
      register int offset asm("r3")=0x160;
      asm("" : "+r"(offset));
      q=(struct Drawing*)((u8*)p+offset);
    }
    sub_0821980C(q,resource,29,0);
    q->flags|=1;
  }
  if(p->timer==90) {
    q=p->drawings;
    sub_0821980C(q,p->resource,9,0);
    q->flags&=~1;
    q++;
    q->flags|=1;
    q=&p->drawings[2];
    q->flags|=1;
     {
      register int offset asm("r3")=0x160;
      asm("" : "+r"(offset));
      q=(struct Drawing*)((u8*)p+offset);
    }
    q->flags|=1;
    sub_0822B2F8(1452);
  }
  if(p->timer-90<=7) {
    register u32*state asm("r2");
    register u32 limit asm("r5");
    register u32 index asm("r0");
    register int value asm("r1");
    register int bits asm("r3");
    q=p->drawings;
    asm("" : "+r"(q));
    table=gUnk_0203B400;
    state=&gUnk_03005308;
    asm("" : "+r"(state));
    index=*state+1;
    limit=1023;
    index&=limit;
    asm("" : "+r"(index));
    *state=index;
    value=index*2;
    asm("" : "+r"(value));
    value=*(u16*)(value+(u32)table);
    bits=3;
    asm("" : "+r"(value),"+r"(bits));
    value&=bits;
    value-=2;
    q->x=value;
    index=(index+1)&limit;
    *state=index;
    index*=2;
    asm("" : "+r"(index));
    index=*(u16*)(index+(u32)table)&bits;
    asm("" : "+r"(index));
    index-=2;
    q->y=index;
  }
  asm("" ::: "memory");
  if(p->timer==98) {
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
