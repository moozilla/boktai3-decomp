#include "global.h"
struct Item {
  u8 pad0[0xb0];
}
;
struct Owner {
  u8 pad0[0x5c];
  struct Item items[16];
}
;
extern struct Owner*gUnk_02000100;
void sub_0803A350(void*,void*,int);
int sub_0803A408(struct Owner*p) {
  struct Item*q=p->items;
  int i=0;
  do {
    sub_0803A350(p,q,i);
    i++;
    q++;
  }
  while(i<=15);
  return (int)(gUnk_02000100=0);
}
