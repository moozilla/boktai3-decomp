// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_SetActive sub_0801E630
void sub_0801EF5C(void);
void SceneObjects_SetActive(u8*p,u16 id){int i=0;if(i<*(u16*)(p+0x1a)){register u8 **base asm("r6")=(u8**)(p+0x42c);register int one asm("r8");register int zero asm("r5");asm("" : : "r"(base));one=1;asm("" : "+r"(one));zero=0;asm("" : "+r"(zero));do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id){s[0x37]=one;*(void**)(s+0x30)=sub_0801EF5C;s[0x39]=zero;s[0x3a]=zero;}i++;}while(i<*(u16*)(p+0x1a));}}
