// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_BeginDeactivation sub_0801E5B8
void sub_0801EF60(void);
void SceneObjects_BeginDeactivation(u8*p,u16 id){int i=0;asm("" : "+r"(i));if(i<*(u16*)(p+0x1a)){register u8 **base asm("r5")=(u8**)(p+0x42c);register int three asm("sb");register int zero asm("r8");register int two asm("ip");asm("" : : "r"(base));three=3;zero=i;two=2;asm("" : "+r"(three),"+r"(zero),"+r"(two));do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id && (u8)(s[0x37]-1)<=1){s[0x37]=three;*(void**)(s+0x30)=sub_0801EF60;s[0x39]=two;s[0x3a]=zero;}i++;}while(i<*(u16*)(p+0x1a));}}
