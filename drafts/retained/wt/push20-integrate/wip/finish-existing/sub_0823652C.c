#include "global.h"
struct S { u8 p0[0x48]; u16 *src; u16 dest[16]; u8 b6c, alpha; };
void sub_0823652C(struct S *s)
{
 u16 *p=s->src;
 int bias=(16-s->alpha)*15;
 int i=0;
 do {
  u16 c=*p;
  u16 shifted=c>>5;
  int r=c&31, g=shifted&31, b=(c>>10)&31;
  int a=s->alpha;
  r=(r*a+bias)>>4;
  g=(g*a+bias)>>4;
  b=(b*a+bias)>>4;
  s->dest[i]=(b<<10)|(g<<5)|r;
  p++; i++;
 } while(i<=15);
}
