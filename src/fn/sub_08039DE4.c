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
void sub_08039968(void*,int,int);
void sub_08039DE4(struct Owner *owner,struct Item *p,int index)  {
  register struct Sprite *sprite asm("r9")=&p->sprite;
  register struct Drawing *drawing asm("r8");
  register int choice asm("r0");
  {
    register int offset asm("r2")=0x50;
    register struct Drawing*initial asm("r2");
    asm("" : "+r"(offset));
    initial=(struct Drawing*)(offset+(u32)p);
    drawing=initial;
  }
  if(p->entered)  {
    register int zero asm("r3")=0;
    register struct Vec*vector asm("r2");
    register int y asm("r6");
    register const u16*table asm("r0");
    register const u16*entry asm("r1");
    asm("" : "+r"(zero));
    p->entered=zero;
    vector=&p->start;
    y=128;
    asm("" : "+r"(vector),"+r"(y));
    if(index>7)y=144;
    table=gUnk_0824DE8C;
    asm("" : "+r"(table));
    entry=(const u16*)((index&3)*2+(u32)table);
    asm("" : "+r"(entry));
    p->start.x=*entry+80;
    vector->y=y;
    vector->z=zero;
    vector=&p->end;
    y=272;
    asm("" : "+r"(vector),"+r"(y));
    {
      register int x asm("r0")=y;
      register int value asm("r1");
      asm("" : "+r"(x));
      value=*entry;
      asm("" : "+r"(value));
      p->end.x=x+value;
    }
    vector->y=32;
    vector->z=zero;
    sub_08039968(p,32,15);
  }
  p->sprite.value=p->count;
  p->sprite.flags&=~1;
  {
    register void*draw asm("r3")=&p->drawing;
    register void*resource asm("r6")=owner->resource;
    asm("" : "+r"(draw),"+r"(resource));
    if(index>7)choice=(index&3)+4;
    else choice=index&3;
    asm("" : "+r"(choice));
    sub_0821980C(draw,resource,gUnk_0824DE7C[choice],0);
  }
  p->drawing.flags&=~1;
  {
    register void*position asm("r4")=&drawing->x;
    asm("" : "+r"(position));
    sub_082216F8(position,&p->start,&p->end,p->timer,5);
    sub_08039998(position,p->timer,p->velocity);
  }
  {
    register struct Drawing*source asm("r2")=drawing;
    register u32 x asm("r0")=source->x;
    register u32 y asm("r1")=source->y;
    register struct Sprite*dest asm("r6")=sprite;
    asm("" : "+r"(x),"+r"(y));
    dest->x=x;
    dest->y=y;
  }
  if(p->timer>31)  {
    register int one asm("r1")=1;
    int zero=0;
    asm("" : "+r"(one),"+r"(zero));
    p->state=one;
    p->entered=one;
    p->timer=zero;
    p->count=zero;
    p->unused=zero;
  }
  p->timer++;
}
