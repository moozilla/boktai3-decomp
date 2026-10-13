// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Panel{u8 pad0[0x24];u32 timer;u8 state,entered;};struct Panel*sub_08037DC0(void);void sub_08039834(void*);
void sub_08039870(void){struct Panel*p=sub_08037DC0();if(p){register int one asm("r0")=1;register u8*state asm("r1")=&p->state;register int zero asm("r2")=0;asm("" : "+r"(one),"+r"(state),"+r"(zero));*state=one;state++;asm("" : "+r"(state));*state=one;p->timer=zero;sub_08039834(p);}}
