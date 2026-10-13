// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Motion {
  u16 x,y;
}
;
extern const s16 gUnk_086149C4[];
void sub_08039998(struct Motion*arg,int angleArg,int scale) {
  register struct Motion*p asm("r4")=arg;
  register const s16*table asm("r3");
  register int angle asm("r1")=angleArg;
  register int product asm("r0");
  int delta;
  angle*=4;
  asm("" : "+r"(angle));
  table=gUnk_086149C4;
  asm("" : "+r"(table));
  angle&=255;
  asm("" : "+r"(angle));
  product=table[angle]*scale;
  asm("" : "+r"(product));
  if(product>=0)delta=product>>12;
  else delta=-((-product)>>12);
  p->x+=delta;
  p->y+=delta;
}
