#define Display_BlinkUntilTimeout sub_082212DC
#define gDisplayBlinkPeriods      gUnk_086149BC
#include "global.h"
extern const s16 gUnk_086149BC[];
s32 Mod(s32, s32);
s32 sub_082212DC(u32 *flags, s32 age, s32 slowLimit, s32 fastLimit)
{
    s32 index = 2;
    if (age <= fastLimit) {
        index = 0;
        if (age > slowLimit)
            index = 1;
    }
    if (Mod(age, gUnk_086149BC[index]) == 0) {
        if (*flags & 1)
            *flags &= ~1;
        else
            *flags |= 1;
    }
    if (age <= 60)
        index = 0;
    else {
        *flags |= 1;
        index = 1;
    }
    return index;
}
