
#ifdef NONMATCHING
// fuzzy: 99.2%
// foundry draft (worker best 99.2%); what differs is mostly register allocation and instruction order
#define ScenePanel_AdvanceConditionalPairState sub_081A7AD0
#include "global.h"
struct ScenePanelPairState {
 u8 unknown0[0xE1];
 u8 threshold:8;
 u8 unknownE2[0x56C-0xE2];
 void *pair;
 u8 unknown570[0x5C4-0x570];
 u8 state;
};
s32 ScenePair_CountPositiveCounters(void *);
void sub_081A7AD0(struct ScenePanelPairState *panel) {
 s32 count;
 switch(panel->state) {
 case 3:
 count=ScenePair_CountPositiveCounters(panel->pair);
 if(count==2) panel->state=4;
 else if(count==0) panel->state=6;
 break;
 case 4:
 if(ScenePair_CountPositiveCounters(panel->pair)!=2) goto reset;
 if(panel->threshold<=42) panel->state=8;
 else panel->state=7;
 break;
 case 6:
 if(ScenePair_CountPositiveCounters(panel->pair)!=0) goto reset;
 panel->state=12; break;
 case 7: case 8:
 if(ScenePair_CountPositiveCounters(panel->pair)!=2) goto reset;
 panel->state=5; break;
 case 11: panel->state=6; break;
 case 5: case 9: case 10: case 12: default:
 reset: panel->state=3; break;
 }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A7AD0);
#endif
