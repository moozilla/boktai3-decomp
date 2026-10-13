// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Pair {
  u32 x,y;
}
;
struct Vec  {
  s16 x,y,z;
}
;
struct Sprite  {
  u32 flags;
  u8 pad4[12];
  u16 value;
  u8 pad12[10];
  u32 x,y;
  u8 pad24[8];
}
;
struct Drawing  {
  u8 pad0[8];
  u32 flags;
  u8 padc[0x14];
  u32 x,y;
  u8 pad28[0x38];
}
;
struct Item  {
  u8 state,entered,unused,count,type,pad5;
  s16 velocity;
  u16 timer;
  u8 pada[2];
  struct Vec start;
  struct Vec end;
  u8 pad1c[8];
  struct Sprite sprite;
  struct Drawing drawing;
}
;
struct Owner  {
  u8 pad0[0x3c];
  u8 resource[32];
  struct Item items[16];
}
;
extern const u16 gUnk_0824DE7C[],gUnk_0824DE8C[];
void sub_0821980C(void*,void*,int,int);
void sub_08039920(void*,int,int);
void sub_082216F8(void*,void*,void*,int,int);
void sub_08039998(void*,int,int);
void sub_08039CB0(struct Owner *owner,struct Item *p,int indexArg)  {
  register int index asm("r6")=indexArg;
  struct Sprite *sprite=&p->sprite;
  struct Drawing *drawing=&p->drawing;
  if(p->entered)  {
    register int choice asm("r0");
    register int y asm("r3");
    register void*draw asm("r3");
    register void*resource asm("r4");
    p->entered=0;
    sprite->value=p->count;
    sprite->flags&=~1;
    draw=drawing;
    resource=owner->resource;
    asm("" : "+r"(draw),"+r"(resource));
    if(index>7)choice=(index&3)+4;
    else choice=index&3;
    asm("" : "+r"(choice));
    sub_0821980C(draw,resource,gUnk_0824DE7C[choice],0);
    *(u32*)((u8*)p+0x58)&=~1;
    {
      register struct Vec*end asm("r2")=&p->end;
      asm("" : "+r"(end));
      y=128;
      if(index>7)y=144;
      p->end.x=gUnk_0824DE8C[index&3]+80;
      end->y=y;
      end->z=0;
    }
    sub_08039920(p,32,15);
  }
  sub_082216F8(&drawing->x,&p->start,&p->end,p->timer,5);
  sub_08039998(&drawing->x,p->timer,p->velocity);
  *(struct Pair*)&sprite->x=*(struct Pair*)&drawing->x;
  if(p->timer>31)  {
    register int offset asm("r0")=(index&~4)*0xb0+0x5c;
    register struct Item*target asm("r2");
    register int count asm("r3");
    asm("" : "+r"(offset));
    target=(struct Item*)((u8*)owner+offset);
    count=p->count;
    if(target->state==1)count+=target->count;
    {
      register int one asm("r0")=1;
      register int zero asm("r1")=0;
      asm("" : "+r"(one),"+r"(zero));
      target->state=one;
      target->entered=one;
      target->timer=zero;
    }
    {
      register int value asm("r0")=count;
      asm("" : "+r"(value));
      if(value>10)value=10;
      else if(value<0)value=0;
      {
        int zero=0;
        asm("" : "+r"(zero));
        target->count=value;
        value=0;
        asm("" : "+r"(value));
        target->unused=value;
        target->type=8;
        p->state=zero;
        p->entered=1;
        p->timer=zero;
        p->count=zero;
        p->unused=zero;
      }
    }
  }
  p->timer++;
}
