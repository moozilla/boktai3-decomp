#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define AnimatedParticle_PriorityFromFlags sub_080154D0
int AnimatedParticle_PriorityFromFlags(int flags){register int p asm("r0")=flags&0xf000;register int result asm("r1");asm("" : "+r"(p));p=(p>>12)-1;result=2;if((u32)p<=3)result=p;asm("" : "+r"(result));return result;}
