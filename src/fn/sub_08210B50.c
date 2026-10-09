#include "global.h"
s32 sub_08210A8C(void *);s32 Div(s32,s32);s32 Mod(s32,s32);
void sub_0821980C(void *,void *,u16,u32);
s32 sub_08210B50(u8 *p) {
 s32 v=sub_08210A8C(p),a=Div(v,10),b=Mod(v,10),c=Div(27,10),d=Mod(27,10);
 sub_0821980C(p+0x1c4,p+0x24,a,0);
 sub_0821980C(p+0x224,p+0x24,b,0);
 sub_0821980C(p+0x284,p+0x24,c,0);
 sub_0821980C(p+0x2e4,p+0x24,d,0);
 return 0;
}
