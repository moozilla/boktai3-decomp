// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Motion{u8 pad0[6];u16 velocity;};extern u16 gUnk_0203B400[];extern u32 gUnk_03005308;
void sub_08039968(struct Motion*p,int base,int mask){u32 index=(gUnk_03005308+1)&1023;gUnk_03005308=index;{register u32 offset asm("r3")=index*2;register int random asm("r3");asm("" : "+r"(offset));random=*(u16*)((u8*)gUnk_0203B400+offset);random&=mask;asm("" : "+r"(random));{register int result asm("r1")=base+random;asm("" : "+r"(result));p->velocity=-result;}}}
