#include "global.h"
s32 sub_08212FC4(void *);s32 Div(s32,s32);s32 Mod(s32,s32);
void sub_0821980C(void *,void *,u16,u32);
s32 sub_082130D4(u8 *p) {
 s32 v=sub_08212FC4(p),a=Div(v,10),b=Mod(v,10),c=Div(10,10),d=Mod(10,10);
 sub_0821980C(p+0x1e4,p+0x24,a,0);
 sub_0821980C(p+0x244,p+0x24,b,0);
 sub_0821980C(p+0x2a4,p+0x24,c,0);
 sub_0821980C(p+0x304,p+0x24,d,0);
 return 0;
}
