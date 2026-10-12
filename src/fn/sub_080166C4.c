#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Controller{u8 pad[24];u8 state,first,count,enabled;u16 timers[4],periods[4];s8 current[4],target[4];};
extern struct Controller*gUnk_02000070;struct Controller*sub_08016674(int,int);
#define PaletteLevels_Configure sub_080166C4
int PaletteLevels_Configure(int mode,int first,int count,u8*initial,u8*target,u16*periods){register struct Controller*initialManager asm("r0")=gUnk_02000070;struct Controller*p;int i;asm("" : "+r"(initialManager));if(!initialManager){p=sub_08016674(0,0);if(!p)return -1;}else p=initialManager;p->state=mode;p->first=first;p->count=count;p->enabled=15;for(i=0;i<=3;i++){p->timers[i]=0;p->periods[i]=periods[i];p->current[i]=initial[i]&15;p->target[i]=target[i]&15;}return 0;}
