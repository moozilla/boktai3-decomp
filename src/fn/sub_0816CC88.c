#include "global.h"
void sub_0816616C(u8 *);
void sub_08030BF8(void);
void sub_08033468(void);
void sub_080335B4(void);
void sub_08030F20(u32,u32,u32,u32);
extern u8 *gUnk_02000710;
extern u32 gUnk_08611BA8[],gUnk_08611BC0[],gUnk_08611BD8[];
void sub_08165ACC(u8 *);
void sub_0821980C(u8 *,u8 *,u32,u32);
void sub_08166094(u8 *,u32,u32);
void sub_081660D0(u8 *,u32,s32);
void sub_08163F14(u8 *,u32,u32,u32,u32);
void sub_0816C838(u8 *);
void sub_081663A0(u8 *,u32,u32,u32,u32);
void sub_0816CC88(u8 *s)
{
    s16 *index;
    u8 *obj;
    sub_0816616C(s);
    sub_08030BF8();
    sub_08033468();
    sub_080335B4();
    sub_08030F20(0,0x12,0x1E,2);
    *(u32 *)(s+0x48F8)=gUnk_08611BA8[*(index=(s16 *)(gUnk_02000710+0x5AC))];
    *(u32 *)(s+0x48FC)=gUnk_08611BC0[*index];
    *(u32 *)(s+0x4900)=gUnk_08611BD8[*index];
    sub_08165ACC(s);
    obj=s+0x1420;
    *(u32 *)(obj+8)&=~1;
    sub_0821980C(obj,s+0x64,0x51,0);
    sub_08166094(s,0,0x18);
    sub_081660D0(s,1,-1);
    sub_08163F14(s,*(u32 *)(s+0x18),0,5,*(u32 *)(s+0x30));
    sub_0816C838(s);
    sub_081663A0(s,0,0,0,0);
}
