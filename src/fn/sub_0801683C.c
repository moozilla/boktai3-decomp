#include "global.h"
struct Controller{u8 pad[24];u8 state,first,count,enabled;u16 timers[4],periods[4];s8 current[4],target[4];};
extern struct Controller*gUnk_02000070;void sub_0821910C(void);
#define PaletteLevels_Reset sub_0801683C
void PaletteLevels_Reset(void){struct Controller*p=gUnk_02000070;int i;if(p){sub_0821910C();p->state=0;p->first=0;p->count=0;p->enabled=0;for(i=0;i<=3;i++){p->timers[i]=0;p->periods[i]=0;p->current[i]=0;p->target[i]=0;}}}
