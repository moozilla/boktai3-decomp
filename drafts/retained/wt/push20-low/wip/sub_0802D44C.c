#include "global.h"
void sub_0802CD04(u8 *);
void sub_08042588(void *,u32,u32,u32,void *);
u32 sub_0802D080(u8 *,u32,u32,u32,u32,u32);
void sub_0802C9A0(u8 *,void *,void *,void *);
void sub_0802CA1C(u8 *,void *,void *,void *);
void sub_0802D1C8(void);
void sub_0802D1B0(void);
void sub_0802D334(void);
void sub_0802D380(void);
void sub_0802D218(void);
struct FD44 {u8 pad[0x11c];void *fn11c,*fn120,*fn124;u16 h128,h12a;u8 b12c,b12d;u8 pad2[2];u32 w130,w134;};
struct VD44 {u32 x:16;u32 y:16;u32 z:16;};
s32 sub_0802D44C(u8 *p)
{
 struct VD44 a,b,c,d;
 u32 zero;
 sub_0802CD04(p);
 sub_08042588(p+0x184,0xd495,0xaf44,0,p+0xc);
 zero=0;
 sub_0802D080(p,1,zero,1,zero,zero);
 {u32 mask=2;
 *(u32 *)(p+0x50)|=mask;}
 a.x=0x20; a.y=0x80; a.z=0x20;
 b.x=zero; b.y=0x80; b.z=zero;
 sub_0802C9A0(p,&a,&b,sub_0802D1C8);
 c.x=0x20; c.y=0x80; c.z=0x20;
 d.x=zero; d.y=0x80; d.z=zero;
 sub_0802CA1C(p,&c,&d,sub_0802D1B0);
 {struct FD44 *q=(struct FD44 *)p;
 q->fn120=sub_0802D334;
 q->fn124=sub_0802D380;
 q->h128=zero;
 q->b12c=1;
 q->w130=zero;
 q->fn11c=sub_0802D218;
 q->h12a=zero;
 q->b12d=1;
 q->w134=zero;}
 return 0;
}
