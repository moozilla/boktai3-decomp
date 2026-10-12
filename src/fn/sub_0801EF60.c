// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObject_AnimateInactive sub_0801EF60
void sub_0822B3BC(int);void sub_0801EF58(void);
void SceneObject_AnimateInactive(u8*s){if((*(u32*)(s+0x38)&0xffff00)==0x200)sub_0822B3BC(0xcf);if(++s[0x3a]>7){s[0x39]--;s[0x3a]=0;if(s[0x39]==0){s[0x37]=0;{u16 *flags=(u16*)(*(u8**)(s+0x2c)+6);register int old asm("r1")=*flags;register int result asm("r0");asm("" : "+r"(old));result=2;asm("" : "+r"(result));result|=old;asm("" : "+r"(result));*flags=result;}asm("" ::: "memory");{u16 *flags=(u16*)(*(u8**)(s+0x2c)+6);register int old asm("r1")=*flags;register int result asm("r0");asm("" : "+r"(old));result=3;asm("" : "+r"(result));result|=old;asm("" : "+r"(result));*flags=result;}asm("" ::: "memory");(*(u8**)(s+0x2c))[4]=((*(u8**)(s+0x2c))[4]&0xf0)|(s[0x3b]+1);*(void**)(s+0x30)=sub_0801EF58;}}}
