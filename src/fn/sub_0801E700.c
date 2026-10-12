// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_Lock sub_0801E700
void SceneObjects_Lock(u8*p,u16 id){int i=0;if(i<*(u16*)(p+0x1a)){register u8**base asm("r5")=(u8**)(p+0x42c);register int one asm("r7");asm("" : : "r"(base));one=1;asm("" : : "r"(one));do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id)s[0x36]|=1;i++;}while(i<*(u16*)(p+0x1a));}}
