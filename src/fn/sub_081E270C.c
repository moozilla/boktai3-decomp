#include "global.h"
#define TimedPresentation_Update sub_081E270C
// Provisional name: TimedPresentation_Update
struct TimedSequence {u8 pad[0x18];s32 state,timer,exitTimer,minShift,fadeShift,duration,unused;void*handle;};
extern u16 gUnk_03005260[];void sub_081DC1DC(s32);s32 sub_08228008(void);void sub_0821AD08(void*,void*);void sub_0821A0C0(void*);
// Wait for the sequence duration or an eligible skip, then finish its exit fade.
void TimedPresentation_Update(struct TimedSequence*p){
 switch(p->state){
 case 0:
 if(p->timer>p->duration-(1<<p->fadeShift)){p->state=1;sub_081DC1DC(p->fadeShift);}
 else if(p->timer>(1<<p->minShift)&&sub_08228008()&&(gUnk_03005260[1]&1)){p->state=1;sub_081DC1DC(p->fadeShift);}
 break;
 case 1:
 if(p->exitTimer>(1<<p->fadeShift)){sub_0821AD08(p->handle,0);sub_0821A0C0(p);}
 p->exitTimer++;break;case 2:break;
 }
 p->timer++;
}
