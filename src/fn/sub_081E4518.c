#include "global.h"
/* Finish a delayed controller callback, selecting the time-of-day sound. */
struct S {u8 pad[0xf88];u8 timer;u8 gap[15];void (*callback)(void*);};
extern u8 gUnk_03005430[];
void sub_0822B2F8(u32);void sub_08228D88(void);
void sub_081E4518(struct S *p){if(--p->timer==0){p->callback=0;if((u8)(gUnk_03005430[14]-1)<=2)sub_0822B2F8(0x120);else sub_08228D88();}}
