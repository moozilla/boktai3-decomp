#include "global.h"
/* Advance a paired four-frame display every tenth update. */
extern u8 gUnk_0824FDC8[];
void sub_0824DA48(void*,const void*,u32);void sub_0821980C(void*,void*,u16,u32);
void sub_081EAAE4(u8*p){u8 frames[4][2];if(Mod(++*(s32*)(p+0x744),10)==0){sub_0824DA48(frames,gUnk_0824FDC8,8);if(++*(s32*)(p+0x740)>3)*(s32*)(p+0x740)=0;sub_0821980C(p+0x238,p+0x4e0,frames[*(s32*)(p+0x740)][0],0);sub_0821980C(p+0x2c0,p+0x4e0,frames[*(s32*)(p+0x740)][1],0);}}
