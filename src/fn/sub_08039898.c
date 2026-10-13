// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Panel{u8 pad0[0x24];u32 timer;u8 state,entered;};struct Panel*sub_08037DC0(void);
void sub_08039898(void){struct Panel*p=sub_08037DC0();if(p){register int state asm("r1")=12;register u8*dest asm("r0")=&p->state;register int zero asm("r2")=0;asm("" : "+r"(state),"+r"(dest),"+r"(zero));*dest=state;p->entered=1;p->timer=zero;}}
