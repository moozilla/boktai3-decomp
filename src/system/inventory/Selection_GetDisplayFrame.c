#include "global.h"

s32 Selection_GetDisplayFrame(s32 selection)
{
    s32 result;
    if (selection <= 29)
        return selection + 0x151;
    if (selection <= 31) {
        selection += 0x152;
        goto done;
    }
    if (selection <= 35) {
        result = 0x173;
        goto resultDone;
    }
    if (selection <= 38) {
        result = 0x172;
        goto resultDone;
    }
    if (selection > 54) {
        result = 0x151;
        goto resultDone;
    }
    selection += 0x14d;
done:
    return selection;
resultDone:
    return result;
}
