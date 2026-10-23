// MATCHING_CONCESSION: register-pinning
#include "global.h"

// The two barriers preserve distinct addition tails for the first two ranges.
#define Selection_GetDisplayFrame sub_0822D604
s32 Selection_GetDisplayFrame(s32 selection)
{
    if (selection <= 29) {
        selection += 0x151;
        asm("" : "+r"(selection));
    } else if (selection <= 31) {
        selection += 0x152;
        asm("" : "+r"(selection) : "i"(0));
    } else if (selection <= 35)
        selection = 0x173;
    else if (selection <= 38)
        selection = 0x172;
    else if (selection > 54)
        selection = 0x151;
    else
        selection += 0x14D;
    return selection;
}
