#include "global.h"
void sub_08172638(u8 *);
void sub_0816B644(void *);
void sub_08172850(void *,u32,u32);
void sub_08165B94(u8 *,u32,u32);
void sub_08165B7C(u8 *,u32,u32);
void sub_081727AC(void *,u32);
void sub_08166B7C(u8 *,u32);
void sub_08169818(u8 *);
void sub_08169838(u8 *);
void sub_08166094(u8 *,u32,u32);
void sub_08163EB8(u8 *,void *,u32);
void sub_081731E8(void);
void sub_081663A0(u8 *,u32,u32,u32,u32);
u32 sub_08166ED4(u8 *,u32,u32,u32,u32);
void sub_08166E14(u8 *,u32,u32,u32,u32,u32);
void sub_081724D4(u8 *);
void sub_08172AF4(u8 *s)
{
    u8 *p,*q;
    u32 zero,result;
    sub_08172638(s);
    p=s+0x4384;
    sub_0816B644(p);
    sub_08172850(p,0x61,0);
    sub_08165B94(s,4,1);
    sub_08165B7C(s,6,1);
    sub_08165B7C(s,5,1);
    q=s+0x15C0;
    p=s+0x4387;
    sub_081727AC(q,*p);
    sub_08166B7C(s,*p<=7?0:1);
    sub_08169818(s);
    sub_08169838(s);
    sub_08166094(s,0,0);
    sub_08163EB8(s,sub_081731E8,0);
    {struct Flags {u32 pad[2],flags;};
     struct Flags *f=(struct Flags *)(s+0x2A40);f->flags|=1;}
    sub_08030BF8();
    sub_08033590();
    sub_081663A0(s,0,1,0,zero=0);
    result=sub_08166ED4(s,0,0,1,zero);
    sub_08166E14(s,3,14,result,zero,0x47);
    sub_081724D4(s);
}
