#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
void sub_0821FEB4(u8 *, u16, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);
void sub_0820AD84(void);
void sub_0821FF7C(void *,u32,u32,u32);
struct Q {u8 pad[6];u16 flags;};
void sub_0820AF30(u8 *p) {
 struct Z a,b;u8 *q=p+0x6c;
 a.a.lo=100;a.a.hi=120;a.b.lo=100;
 *(u32 *)&b.a=0x800000;*(u32 *)&b.b&=0xffff0000;
 sub_0821FEB4(q,0,0x5009,0,0x10,&a,&b);
 sub_0821FF24(q,p+0x40,0);
 sub_0821FF84(q,sub_0820AD84,p);
 sub_0821FF7C(q,0,*(u16 *)(p+0x13c),*(u16 *)(p+0x13e));
 sub_0821FE40(q);((struct Q *)q)->flags |= 4;
}
