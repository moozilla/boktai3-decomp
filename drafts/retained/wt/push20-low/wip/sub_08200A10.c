#include "global.h"
static inline void fill00A(const void *source, void *dest, u32 control)
{ CpuSet(source, dest, control); }
static inline u32 flag00A(u32 value)
{ u32 mask=4; mask|=value; return mask; }
struct Vec00A {u32 x:16;u32 y:16;u32 z:16;};
void sub_08201274(u8*);
void *sub_0821A520(u32,u32);
void sub_082196C4(u8*,void*);
void sub_0821983C(u8*,u8*,u32,u32,u32,u32,u32,u32);
u32 sub_081605DC(u8*,u8*,u8*);
void sub_0816065C(u32,u32,u32);
void sub_080139F4(u8*,u8*);void sub_08013728(u8*);
void sub_0821FEB4(u8*,u32,u32,u32,u32,struct Vec00A*,struct Vec00A*);
void sub_0821FF84(u8*,void*,u8*);void sub_0821FF7C(u8*,s32,u32,u32);
void sub_0821FE40(u8*);void sub_0821FF58(u8*,s32,s32,u32,u32,s32);
void sub_08200324(void);void sub_082003B8(void);
void sub_08200A10(u32 unused,u8 *s,u32 arg)
{
 u8 *context=s+0x198;
 u32 zero,fill;
 u8 *res,*p;
 u32 value;
 struct Vec00A v,w;
 sub_08201274(context);
 zero=0;
 *(u32*)s=zero;
 {u8 *dest=s+0xc;
 u32 *source;
 fill=zero;
 source=&fill;
 fill00A(source,dest,0x05000002);}
 res=sub_0821A520(0xcb05,0xde23);
 {u8 *obj=s+0x278;
 sub_082196C4(obj,res);
 {u8 *p=s+0x1b8;
 sub_0821983C(p,obj,0,0,2,1,zero,zero);
 *(u32*)(s+0x1c0)|=1;
 value=sub_081605DC(p,s+0x320,s+0x322);
 *(u32*)(s+0x33c)=value;
 sub_0816065C(value,0xe001,0);
 }}
 {u8 *obj=s+0x298;
 sub_080139F4(obj,s+0x1d8);
 sub_08013728(obj);}
 *(u32*)(s+0x328)=arg;
 p=s+0x80;
 v.x=40;v.y=128;v.z=40;
 w.x=zero;w.y=128;w.z=zero;
 sub_0821FEB4(p,zero,0x4001,zero,16,&v,&w);
 sub_0821FF84(p,sub_08200324,s);
 sub_0821FF7C(p,*(s16*)(context+0x16),0x402,1);
 sub_0821FE40(p);
 *(u16*)(p+6)=flag00A(*(u16*)(p+6));
 p-=0x54;
 v.x=64;v.y=128;v.z=64;
 w.x=zero;w.y=128;w.z=zero;
 sub_0821FEB4(p,zero,0x2001,zero,16,&v,&w);
 sub_0821FF84(p,sub_082003B8,s);
 sub_0821FF58(p,*(s16*)(context+0x10),*(s16*)(context+0x12),0,zero,*(s16*)(context+0x14));
 p+=0xa8;
 v.x=200;v.y=128;v.z=200;
 w.x=zero;w.y=128;w.z=zero;
 sub_0821FEB4(p,zero,0x2001,zero,16,&v,&w);
 sub_0821FF84(p,sub_082003B8,s);
 sub_0821FF58(p,*(s16*)(context+0x18),*(s16*)(context+0x1a),0,zero,*(s16*)(context+0x1c));
 p=s+0x128;
 v.x=32;v.y=32;v.z=32;
 w.x=zero;w.y=128;w.z=zero;
 sub_0821FEB4(p,zero,0x2001,zero,16,&v,&w);
 sub_0821FF84(p,sub_082003B8,s);
}
