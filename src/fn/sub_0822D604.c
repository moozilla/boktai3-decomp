
#ifdef NONMATCHING
// fuzzy: 67.3%
// foundry draft (worker best 92.1%); what differs is mostly register allocation and instruction order
#define Selection_GetDisplayFrame sub_0822D604
#include "global.h"
s32 sub_0822D604(s32 selection)
{
    if (selection <= 29)
        selection += 0x151;
    else if (selection <= 31)
        selection += 0x152;
    else if (selection <= 35)
        selection = 0x173;
    else if (selection <= 38)
        selection = 0x172;
    else if (selection > 54)
        selection = 0x151;
    else
        selection += 0x14D;
    return selection;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0822D604);
#endif
