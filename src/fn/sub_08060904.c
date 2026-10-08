#include "global.h"

s32 sub_0822D6E0(s32);

s32 sub_08060904(void)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (sub_0822D6E0(i) == 0xff) return i;
    }
    return -1;
}
