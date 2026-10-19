#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.5%
// foundry draft (worker best 86.7%); what differs is mostly register allocation and instruction order
struct CallbackList {
    u8 pad0[0xbd];
    u8 valueBase;
    u8 padBE[0x27c];
    u16 valueQuarter;
};
s32 CallbackList_RunMatchingState(struct CallbackList *, s32, s32);
s32 CallbackList_RunStateEight(struct CallbackList *list)
{
    s32 value = list->valueBase + (list->valueQuarter >> 2);
    value = CallbackList_RunMatchingState(list, value, 8);
}
#else
INCLUDE_ASM("asm/nonmatching", CallbackList_RunStateEight);
#endif
