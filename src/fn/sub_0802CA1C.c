#include "global.h"
void sub_0821FEB4(u8 *,u32,u32,u32,u32,void *,void *);
void sub_0821FF24(u8 *,void *,u32);
void sub_0821FF84(u8 *,void *,u8 *);
void sub_0821FF7C(u8 *,u32,u32,u32);
void sub_0821FE40(u8 *);
void sub_0802CA1C(u8 *p,void *a,void *b,void *fn)
{
 u8 *q=p+0xc0;
 u32 flags=0x4001;
 sub_0821FEB4(q,0,flags,0,16,a,b);
 sub_0821FF24(q,p+0xc,0);
 sub_0821FF84(q,fn,p);
 sub_0821FF7C(q,*(u32 *)(p+0x14c),*(u32 *)(p+0x164),*(u32 *)(p+0x168));
 sub_0821FE40(q);
 {u32 mask=8; *(u32 *)(p+0x50)|=mask;}
}
