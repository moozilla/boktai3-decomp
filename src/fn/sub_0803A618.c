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
extern struct Owner *gUnk_02000100;
int sub_0803A618(int indexArg,int amountArg,int pendingArg) {
  register int index asm("r12")=indexArg;
  register int amount asm("r8")=amountArg;
  register int pendingFlag asm("r9")=pendingArg;
  register struct Owner *owner asm("r3")=gUnk_02000100;
  struct Item *target;
  if(!owner)return -1;
  if(amount<=0) {
    register int result asm("r0")=0;
    asm("" : "+r"(result));
    return result;
  }
   {
    register int size asm("r0")=0xb0;
    register int scaled asm("r1");
    register int extra asm("r2");
    register int offset asm("r0");
    asm("" : "+r"(size));
    scaled=index*size;
    extra=0x89c;
    asm("" : "+r"(scaled),"+r"(extra));
    offset=scaled+extra;
    target=(struct Item*)((u8*)owner+offset);
    if(target->count) {
      struct Item *current;
      register u8 *first asm("r1");
      register u8 *second asm("r6");
      register u8 *dest asm("r5");
      register int total asm("r2");
      extra=0x5dc;
      asm("" : "+r"(extra));
      offset=scaled+extra;
      current=(struct Item*)((u8*)owner+offset);
      if(target->unused) {
        first=owner->pending;
         {
          register int slot asm("r0")=index;
          asm("" : "+r"(slot));
          dest=first+slot;
        }
        total=*dest;
        asm("" : "+r"(total));
        total+=target->count;
        second=first;
        asm("" : "+r"(second));
        if(total>255)total=255;
        *dest=total;
        first=owner->current;
      }
      else {
        first=owner->current;
         {
          register int slot asm("r2")=index;
          asm("" : "+r"(slot));
          dest=first+slot;
        }
        total=*dest;
        asm("" : "+r"(total));
        total+=target->count;
        if(total>255)total=255;
        *dest=total;
        second=owner->pending;
      }
       {
        register int slot asm("r2")=index;
        register u8*address asm("r0");
        register int a asm("r3");
        register int b asm("r2");
        register int one asm("r0");
        register int zero asm("r1");
        asm("" : "+r"(slot));
        address=first+slot;
        a=*address;
        address=second+slot;
        b=*address;
        one=1;
        zero=0;
        asm("" : "+r"(a),"+r"(b),"+r"(one),"+r"(zero));
        current->state=one;
        current->entered=one;
        current->timer=zero;
         {
          register int value asm("r0")=a+b;
          int clear;
          asm("" : "+r"(value));
          if(value>10)value=10;
          clear=0;
          asm("" : "+r"(clear));
          current->count=value;
          value=0;
          asm("" : "+r"(value));
          current->unused=value;
          target->count=clear;
        }
      }
    }
  }
   {
    register int value asm("r1")=amount;
    register int zero asm("r0");
    asm("" : "+r"(value));
    if(value>10)value=10;
    else if(value<0)value=0;
    zero=0;
    asm("" : "+r"(zero));
    target->count=value;
    value=pendingFlag;
    asm("" : "+r"(value));
    target->unused=value;
    target->state=5;
    target->entered=1;
    target->timer=zero;
    return zero;
  }
}
