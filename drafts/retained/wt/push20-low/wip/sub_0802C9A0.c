#include "global.h"
void sub_0821FEB4(u8 *,u32,u32,u32,u32,void *,void *);
void sub_0821FF24(u8 *,void *,u32);
void sub_0821FF84(u8 *,void *,u8 *);
void sub_0821FF58(u8 *,u32,u32,u32,u32,u32);
void sub_0802C9A0(u8 *p,void *a,void *b,void *fn)
{
 u8 *q=p+0x6c;
 u32 flags=0x2001;
 sub_0821FEB4(q,0,flags,0,16,a,b);
 sub_0821FF24(q,p+0xc,0);
 sub_0821FF84(q,fn,p);
 sub_0821FF58(q,*(u32 *)(p+0x150),*(u32 *)(p+0x154),*(u32 *)(p+0x15c),*(u32 *)(p+0x160),*(u32 *)(p+0x158));
 {u32 mask=4; *(u32 *)(p+0x50)|=mask;}
}
