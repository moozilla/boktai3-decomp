#include "global.h"
struct Drawing{u8 pad0[8];u32 flags;u8 padc[0x54];};struct Display{u8 pad0[0x1d];u8 entered,finished;u8 pad1f[0x21];struct Drawing drawings[4];};
void sub_0803A744(struct Display*p){if(p->entered){struct Drawing*q;int i;u32 mask;p->entered=0;q=p->drawings;mask=1;i=3;do{q->flags|=mask;i--;q++;}while(i>=0);}if(p->finished)p->finished=0;}
