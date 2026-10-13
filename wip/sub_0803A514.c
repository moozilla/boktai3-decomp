// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Pair{u32 x,y;};
struct Vec {s16 x,y,z;};
struct Sprite {u32 flags;u8 pad4[12];u16 value;u8 pad12[10];u32 x,y;u8 pad24[8];};
struct Drawing {u8 pad0[8];u32 flags;u8 padc[0x14];u32 x,y;u8 pad28[0x38];};
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
};
struct Owner {u8 pad0[0x18];u8 current[4],pending[4];u8 pad20[0x1c];u8 resource[32];struct Item items[16];};
extern const u16 gUnk_0824DE7C[],gUnk_0824DE8C[];
void sub_0821980C(void*,void*,int,int);
void sub_08039968(void*,int,int);
void sub_082216F8(void*,void*,void*,int,int);
void sub_08039998(void*,int,int);
extern struct Owner *gUnk_02000100;
extern u16 gUnk_03005418[];
static inline int project(int input) {
  register int value asm("r0")=input*3;
  asm("" : "+r"(value));
  if(value>=0)value>>=5;else value=-((-value)>>5);
  asm("" : "+r"(value));return value;
}
int sub_0803A514(const s16 *position,int index,int amountArg) {
  register int amount asm("r2")=amountArg;
  register struct Owner *owner asm("r3")=gUnk_02000100;
  register struct Item *target asm("r4");
  if(!owner)return -1;
  if(amount<=0)goto done;
  {
    {register int offset asm("r0");register int extra asm("r4");index*=0xb0;asm("" : "+r"(index));extra=0x31c;asm("" : "+r"(extra));offset=index+extra;target=(struct Item*)((u8*)owner+offset);}
    if(target->count) {
      register int offset asm("r0")=index+0x5c;register struct Item *current asm("r3");register int count asm("r5");asm("" : "+r"(offset));current=(struct Item*)((u8*)owner+offset);

      if(current->state==4)count=target->count;
      else {register int first asm("r1")=current->count;register int second asm("r0")=target->count;asm("" : "+r"(first),"+r"(second));count=first+second;}
      {register int one asm("r0")=1;register int zero asm("r1")=0;asm("" : "+r"(one),"+r"(zero));current->state=one;current->entered=one;current->timer=zero;}
      {register int value asm("r0")=count;int zero;asm("" : "+r"(value));if(value>10)value=10;else if(value<0)value=0;zero=0;asm("" : "+r"(zero));current->count=value;value=0;asm("" : "+r"(value));current->unused=value;target->count=zero;}
    }
    if(amount>10)amount=10;else if(amount<0)amount=0;
    {int zero=0;register int clear asm("r0");asm("" : "+r"(zero));target->count=amount;clear=0;asm("" : "+r"(clear));target->unused=clear;target->state=3;target->entered=1;target->timer=zero;}
    {
      struct Vec *start=&target->start;
      register int x asm("r3")=position[0];register int z asm("r2");register int sum asm("r4");int y;{z=position[2];}
      start->x=project(x-z);
      {register int input asm("r1")=x+z;asm("" : "+r"(input));sum=project(input);}
      y=project(position[1]);
      {register u16*camera asm("r3");register int resultY asm("r2")=sum-y;asm("" : "+r"(resultY));start->y=resultY;sum+=y;start->z=sum;camera=gUnk_03005418;asm("" : "+r"(camera));start->x=start->x-camera[0]+120;resultY=resultY-camera[1]+80;start->y=resultY;sum-=camera[2];start->z=sum;}
    }
  }
done:
  return 0;
}
