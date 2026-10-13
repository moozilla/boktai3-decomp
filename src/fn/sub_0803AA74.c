#include "global.h"
struct Drawing{u8 pad0[8];u32 flags;u8 padc[0x54];};struct Display{u8 pad0[0x18];u32 timer;u8 state,entered,finished,pad1f;u8 resource[32];struct Drawing drawings[4];};void sub_0821980C(void*,void*,int,int);void sub_0822B2F8(int);
void sub_0803AA74(struct Display*p){if(p->entered){struct Drawing*q;p->entered=0;q=p->drawings;sub_0821980C(q,p->resource,10,0);q->flags&=~1;sub_0822B2F8(660);}if(p->finished){p->finished=0;p->state=0;p->entered=1;p->timer=0;}else p->timer++;}
