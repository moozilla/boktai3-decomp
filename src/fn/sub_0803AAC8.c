#include "global.h"
struct Display {
  u8 pad0[0x1c];
  u8 state;
}
;
extern void*gUnk_086057FC[];
int sub_08036BEC(void);
void sub_0824923C(void*,void*);
int sub_0803AAC8(struct Display*p) {
  if(!sub_08036BEC())sub_0824923C(p,gUnk_086057FC[p->state]);
  return 0;
}
