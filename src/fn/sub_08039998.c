#include "global.h"
struct Motion {
  u16 x,y;
}
;
extern const s16 gUnk_086149C4[];
void sub_08039998(struct Motion*arg,int angleArg,int scale) {
  struct Motion*p=arg;
  const s16*table;
  int angle=angleArg;
  int product;
  int delta;
  angle*=4;
  table=gUnk_086149C4;
  angle&=255;
  product=table[angle]*scale;
  if(product>=0)delta=product>>12;
  else delta=-((-product)>>12);
  p->x+=delta;
  p->y+=delta;
}
