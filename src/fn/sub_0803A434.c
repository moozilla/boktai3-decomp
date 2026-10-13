// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Resource  {
  u32 words[8];
}
;
struct Item  {
  u8 pad0[0xb0];
}
;
struct Owner  {
  u8 pad0[0x18];
  u8 current[4], pending[4];
  u8 sprite[0x1c];
  struct Resource resource;
  struct Item items[16];
}
;
extern struct Owner *gUnk_02000100;
struct Resource *sub_0821A520(int,int);
void sub_082196C4(void*,void*);
int sub_082151E4(void*,int);
void sub_0803A2A8(void*,void*,int);
int sub_0803A434(struct Owner *p)  {
  struct Resource *resource;
  struct Item *item;
  u8*current;
  register u8*pending asm("r8");
  int i;
  gUnk_02000100 = p;
  resource = sub_0821A520(0xcb05,0x5d04);
  if (!resource) return -1;
  p->resource = *resource;
  sub_082196C4(&p->resource,resource);
  if (!sub_082151E4(p->sprite,0x18a9)) return -1;
  item = p->items;
  i = 0;
  current = p->current;
  asm("" : "+r"(current));
  pending = p->pending;
  do  {
    sub_0803A2A8(p,item,i);
    i++;
    item++;
  }
  while (i <= 15);
  i = 0;
  {
    register u8*first asm("r3")=current;
    register int zero asm("r1")=0;
    register u8*second asm("r2")=pending;
    asm("" : "+r"(first),"+r"(zero),"+r"(second));
    do  {
      first[i]=zero;
      second[i]=zero;
    }
    while(++i<=3);
  }
  return 0;
}
