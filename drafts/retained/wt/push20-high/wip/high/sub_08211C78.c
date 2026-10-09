#include "global.h"
s32 sub_0821172C(void*,s32);s32 sub_082116EC(s32);
s32 Div(s32,s32);s32 Mod(s32,s32);
void sub_0821980C(void*,void*,u32,u32);
u32 sub_08211C78(u8 *p)
{
 s16 *idx=(s16*)(p+0xb60);
 s32 a=sub_0821172C(p,*idx),b;
 s32 ad,am,bd,bm;
 void *anchor;
 b=sub_082116EC(*idx);
 if(*idx>5) {a--;b--;}
 ad=Div(a,10);am=Mod(a,10);bd=Div(b,10);bm=Mod(b,10);
 sub_0821980C(p+0x1e4,p+0x24,(u16)ad,0);
 anchor=p+0x24;
 sub_0821980C(p+0x244,anchor,(u16)am,0);
 sub_0821980C(p+0x2a4,anchor,(u16)bd,0);
 sub_0821980C(p+0x304,anchor,(u16)bm,0);
 return 0;
}
