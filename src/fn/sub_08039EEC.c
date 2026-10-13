// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Pair {
  u32 x,y;
}
;
struct Vec {
  s16 x,y,z;
}
;
struct Sprite {
  u32 flags;
  u8 pad4[12];
  u16 value;
  u8 pad12[10];
  u32 x,y;
  u8 pad24[8];
}
;
struct Drawing {
  u8 pad0[8];
  u32 flags;
  u8 padc[0x14];
  u32 x,y;
  u8 pad28[0x38];
}
;
struct Item {
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
struct Owner {
  u8 pad0[0x18];
  u8 current[4],pending[4];
  u8 pad20[0x1c];
  u8 resource[32];
  struct Item items[16];
}
;
extern const u16 gUnk_0824DE7C[],gUnk_0824DE8C[];
void sub_0821980C(void*,void*,int,int);
void sub_08039968(void*,int,int);
void sub_082216F8(void*,void*,void*,int,int);
void sub_08039998(void*,int,int);
void sub_08039EEC(struct Owner *owner,struct Item *p,int indexArg) {
  register int index asm("r6")=indexArg;
  register struct Sprite *sprite asm("r9");
  struct Drawing *drawing=&p->drawing;
  asm("" : "+r"(drawing));
  sprite=&p->sprite;
  if(p->entered) {
    register int choice asm("r0");
    register int y asm("r3");
    register void*draw asm("r3");
    register void*resource asm("r4");
    p->entered=0;
     {
      register int value asm("r0")=p->count;
      register struct Sprite*dest asm("r1");
      asm("" : "+r"(value));
      dest=sprite;
      dest->value=value;
    }
    p->sprite.flags&=~1;
    draw=drawing;
    resource=owner->resource;
    asm("" : "+r"(draw),"+r"(resource));
    if(index>7)choice=(index&3)+4;
    else choice=index&3;
    asm("" : "+r"(choice));
    sub_0821980C(draw,resource,gUnk_0824DE7C[choice],0);
    *(u32*)((u8*)p+0x58)&=~1;
     {
      register struct Vec*start asm("r2")=&p->start;
      register const u16*entry asm("r3");
      register int x asm("r0");
      register int zero asm("r4");
      register struct Vec*end asm("r1");
      register int y asm("r2");
      asm("" : "+r"(start));
       {
        register const u16*table asm("r1")=gUnk_0824DE8C;
        register int offset asm("r0");
        asm("" : "+r"(table));
        offset=(index&3)*2;
        entry=(const u16*)(offset+(u32)table);
      }
       {
        register int initial asm("r1")=272;
        asm("" : "+r"(initial));
        x=initial;
      }
      asm("" : "+r"(x),"+r"(entry));
       {
        register int value asm("r1")=*entry;
        asm("" : "+r"(value));
        x+=value;
      }
      zero=0;
      asm("" : "+r"(zero));
      p->start.x=x;
      start->y=32;
      start->z=zero;
      end=&p->end;
      y=128;
      asm("" : "+r"(end),"+r"(y));
      if(index>7)y=144;
      p->end.x=*entry+80;
      end->y=y;
      end->z=zero;
    }
    sub_08039968(p,32,15);
  }
  sub_082216F8(&drawing->x,&p->start,&p->end,p->timer,5);
  sub_08039998(&drawing->x,p->timer,p->velocity);
   {
    register u32 x asm("r0")=drawing->x;
    register u32 y asm("r1")=drawing->y;
    register struct Sprite*dest asm("r2");
    asm("" : "+r"(x),"+r"(y));
    dest=sprite;
    dest->x=x;
    dest->y=y;
  }
  if(p->timer>31) {
    int slot=index&3;
    register u8 *current asm("r2");
    register u8 *pending asm("r12");
    register struct Item *target asm("r4");
    int count;
    if(p->unused) {
      register u8*array asm("r2")=owner->pending;
      register u8*dest asm("r3")=array+slot;
      register int total asm("r1")=*dest;
      asm("" : "+r"(total));
      total+=p->count;
      asm("" : "+r"(total),"+r"(array));
      pending=array;
      if(total>255)total=255;
      *dest=total;
      current=owner->current;
    }
    else {
      register u8*dest asm("r3");
      register int total asm("r1");
      current=owner->current;
      dest=current+slot;
      total=*dest;
      asm("" : "+r"(total));
      total+=p->count;
      asm("" : "+r"(total));
      if(total>255)total=255;
      *dest=total;
       {
        register int offset asm("r0")=0x1c;
        register u8*array asm("r0");
        asm("" : "+r"(offset));
        array=(u8*)(offset+(u32)owner);
        asm("" : "+r"(array));
        pending=array;
      }
    }
     {
      register int offset asm("r0")=(index&~4)*0xb0+0x5c;
      register int first asm("r3");
      register int second asm("r2");
      register int one asm("r0");
      register int zero asm("r1");
      asm("" : "+r"(offset));
      target=(struct Item*)((u8*)owner+offset);
       {
        register u8*address asm("r0")=current+slot;
        asm("" : "+r"(address));
        first=*address;
        current=pending;
        asm("" : "+r"(current));
        address=current+slot;
        second=*address;
      }
      asm("" : "+r"(first),"+r"(second));
      one=1;
      zero=0;
      asm("" : "+r"(one),"+r"(zero));
      target->state=one;
      target->entered=one;
      target->timer=zero;
       {
        register int value asm("r0")=first+second;
        int clear;
        asm("" : "+r"(value));
        if(value>10)value=10;
        clear=0;
        asm("" : "+r"(clear));
        target->count=value;
        value=0;
        asm("" : "+r"(value));
        target->unused=value;
        target->type=8;
        p->count=clear;
        p->state=clear;
        p->entered=1;
        p->timer=clear;
      }
    }
  }
  p->timer++;
}
