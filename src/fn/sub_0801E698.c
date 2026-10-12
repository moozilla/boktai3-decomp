// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_SetInactive sub_0801E698
void sub_0801EF58(void);
void SceneObjects_SetInactive(u8*p,u16 id){int i=0;if(i<*(u16*)(p+0x1a)){register u8 **base asm("r6")=(u8**)(p+0x42c);register int two asm("ip");register int zero asm("r5");register void *fn asm("r8");asm("" : : "r"(base));zero=0;asm("" : "+r"(zero));fn=sub_0801EF58;asm("" : "+r"(fn));two=2;asm("" : "+r"(two));do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id){s[0x37]=zero;*(void**)(s+0x30)=fn;s[0x39]=two;s[0x3a]=zero;}i++;}while(i<*(u16*)(p+0x1a));}}
