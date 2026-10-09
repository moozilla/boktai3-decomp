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
void sub_0819132C(u8 *p,int i) {
 u8 *q;u32 old;
 u8 *base=p+0x71dc;u8 **base_pointer=&base;q=*base_pointer+i;old=*q;
 if(!old) {
 ((struct Entries *)p)->items[i].flags&=~1;
 sub_08220D78(p+(i*0x60+0x543c),p+0x541c,8,2,6);
 (*q)++;((struct Flags *)p)->done[i]=old;
 sub_0822B3BC(0x3ff);
 }
 if(sub_08220F70(p+(i*0x60+0x543c),p+0x541c)) {
 *q=0xff;((struct Flags *)p)->done[i]=1;
 ((struct Visual *)(p+i*0x54+0x66d8))->flags |=4;
 ((struct Sprite *)((i<<4)+(u32)p+0x6f60))->flags |=1;
 }
}
