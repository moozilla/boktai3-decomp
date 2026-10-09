#include "global.h"
void sub_0809775C(u8 *,s16 *);
void sub_08097798(u8 *);
u32 sub_082215E4(s32,s32);
void CpuSet(const void *, void *, u32);
void sub_08097BC0(u8 *s,s16 *v)
{
 u8 *p=*(u8 **)(s+0x3d0);
 u32 stack[3];
 s16 *a;
 s16 *b;
 sub_0809775C(s,v);
 sub_08097798(s);
 {
  u32 one=1;
  u8 *q=s+0x2a8;
  u32 zero=0;
  *q=zero;
  {s32 off=0x2a9; s[off]=zero;
  s[0x2aa]=one;
  *(u32 *)(s+off+3)=zero; }
  s[0x2b1]=one;
  b=(s16 *)(stack+1);
  a=(s16 *)(s+0x5c);
  b[0]=v[0]-a[0];
  b[1]=v[1]-a[1];
  b[2]=v[2]-a[2];
  s[0x3cd]=sub_082215E4(b[0],b[2]);
  p+=0x5fc;
  stack[0]=zero;
 }
 { u32 *fill; CpuSet(fill=stack,p,0x05000002); }
}
