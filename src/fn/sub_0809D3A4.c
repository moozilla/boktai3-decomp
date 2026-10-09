#include "global.h"
extern u8 *gUnk_02000580;
static inline u8 Test(u8 *s) {
 u8 *p=s+0xDC;
 u32 mask=2;
 if(p[15]&mask)return 1;
 return 0;
}
u32 sub_0809D3A4(u8 *s) {
 u8 *p;
 s32 distance;
 if(!Test(s))goto zero;
 p=s+0xDC;
 { s32 mask=-3; mask &=p[15]; p[15]=mask; }
 distance=*(s16*)(s+0x5E)-*(s16*)(gUnk_02000580+0x32);
 if(distance<0)distance=-distance;
 if(distance>0x200)goto zero;
 { u32 *a=(u32*)(s+0xF0); u32 *b=(u32*)(s+0xF8);
 if(*a >= *b)goto zero; }
 goto one;
zero:
 return 0;
one:
 { u32 mask=0x80; u16 *flags=(u16*)(s+0x2D6); mask |=*flags; *flags=mask; }
 { u32 mask=0x10; mask |=p[15];p[15]=mask; }
 return 1;
}
