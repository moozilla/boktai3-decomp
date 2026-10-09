#include "global.h"
struct S { u8 p0[0x48]; u16 *src; u16 dest[16]; u8 b6c, alpha; };
static inline int green(u16 c) { return (c>>5)&31; }
static inline int blue(u16 c) { return (c>>10)&31; }
void sub_0823652C(struct S *s)
{
 u16 *p=s->src;
 int bias=(16-s->alpha)*15;
 int i=0;
 do {
  u32 c=*p;
  int r=c&31, g=green(c), b=blue(c);
  int a=s->alpha;
  int rr,gg,bb;
  rr=(bias+r*a)>>4;
  gg=(bias+g*a)>>4;
  bb=(bias+b*a)>>4;
  s->dest[i]=(bb<<10)|(gg<<5)|rr;
  p++; i++;
 } while(i<=15);
}
