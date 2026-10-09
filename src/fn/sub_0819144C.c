#include "global.h"
void sub_08219728(void *, void *, int);
struct Flags
{
  u8 pad[0x71dc];
  u8 flags[52];
  u8 done[52];
};
struct Entry
{
  u32 flags;
  u8 pad[0x5c];
};
struct Entries
{
  u8 pad[0x5444];
  struct Entry items[52];
};
struct Visual {u8 pad[6];u16 flags;};
struct Sprite {u32 pad;u32 flags;};
void sub_08220D78(void *,void *,s32,s32,s32);s32 sub_08220F70(void *,void *);void sub_0822B3BC(s32);
struct Pair {u32 a,b;};extern const struct Pair gUnk_0824F35C;
void sub_0818FA54(void *,void *,void *,void *);
void sub_0819144C(u8 *p,int i) {
 u8 *base=p+0x71dc;u8 **base_pointer=&base;u8 *q=*base_pointer+i;u32 old=*q;
 if(!old) {
 struct Pair v;u32 off=i*0x60;u8 *a;
 ((struct Entries *)p)->items[i].flags&=~1;
 a=p+(off+0x543c);
 sub_08220D78(a,p+0x541c,9,2,4);
 (*q)++;((struct Flags *)p)->done[i]=old;
 v=gUnk_0824F35C;
 sub_0818FA54(p+0x54,a+0x20,&v,p);
 }
 if(sub_08220F70(p+(i*0x60+0x543c),p+0x541c)) {*q=0xff;((struct Flags *)p)->done[i]=1;}
}
