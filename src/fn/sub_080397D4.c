// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Panel {u8 pad0[0xdc];int total;u8 pade0[2];u16 weights[4];};
struct Panel*sub_08037DC0(void);
void sub_080397D4(int index,int amount){struct Panel*p=sub_08037DC0();if(p){int*total=&p->total;register int weight asm("r0")=p->weights[index];asm("" : "+r"(weight));{register int value asm("r1")=weight;asm("" : "+r"(value));value*=amount;*total+=value;}}}
