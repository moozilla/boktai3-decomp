// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObject_BeginLowering sub_0801F3CC
void sub_0822B2F8(int);void sub_0801F4C4(void);
void SceneObject_BeginLowering(u8*s){sub_0822B2F8(0x152);s[0x39]=0;s[0x3a]=0;{u16 *flags=(u16*)(*(u8**)(s+0x2c)+6);register int old asm("r1")=*flags;asm("" : "+r"(old));{register int result asm("r0")=2;asm("" : "+r"(result));result|=old;asm("" : "+r"(result));*flags=result;}}asm("" ::: "memory");*(u16*)(*(u8**)(s+0x2c)+6)|=3;s[0x37]=3;(*(u8**)(s+0x2c))[4]=((*(u8**)(s+0x2c))[4]&0xf0)|(s[0x3b]+1);asm("" ::: "memory");*(u16*)(*(u8**)(s+0x2c))&=~1;*(void**)(s+0x30)=sub_0801F4C4;}
