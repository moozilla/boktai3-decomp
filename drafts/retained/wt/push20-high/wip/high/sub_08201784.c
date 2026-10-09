#include "global.h"
struct H {u32 lo:16;u32 hi:16;};struct Z{struct H a,b;};
struct Q {u8 pad[6];u16 flags;};
void sub_0821FEB4(u8*,u16,u32,u32,u32,struct Z*,struct Z*);
void sub_0821FF84(u8*,void(*)(void),u8*);
void sub_0821FF58(u8*,s32,s32,u32,u32,s32);
void sub_0821FF24(u8*,u8*,u32);void sub_0821FF7C(void*,u32,u32,u32);void sub_0821FE40(u8*);
void sub_082015D0(void);void sub_082015D4(void);
void sub_08201784(u8 *p)
{
 struct Z a,b;
 u8 *q=p+0x6c;
 u16 *v,*r;
 a.a.lo=120;a.a.hi=120;a.b.lo=120;
 *(u32*)&b.a=0x780000;b.b.lo=0;
 sub_0821FEB4(q,0,0x2001,0,0x10,&a,&b);
 sub_0821FF84(q,sub_082015D0,p);
 sub_0821FF58(q,*(s16*)(p+0x174),*(s16*)(p+0x176),0,0,*(s16*)(p+0x178));
 q+=0x54;
 v=(u16*)&a;r=(u16*)&b;
 v[0]=100;v[1]=200;v[2]=100;
 r[0]=0;r[1]=100;r[2]=0;
 sub_0821FEB4(q,0,0x5009,0,0x10,&a,&b);
 sub_0821FF24(q,p+0x40,0);
 sub_0821FF84(q,sub_082015D4,p);
 sub_0821FF7C(q,0,*(u16*)(p+0x19a),*(u16*)(p+0x19c));
 sub_0821FE40(q);((struct Q*)q)->flags|=4;
}
