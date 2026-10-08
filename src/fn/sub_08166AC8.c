#include "global.h"
s32 sub_0822E728(u32);
s32 sub_0822EAE4(u32);
s32 sub_0822DF38(u32);
s32 sub_0822E224(u32);
s32 sub_08166AC8(u32 a, s32 b)
{
    switch (b) {
    case 1:
        if (a - 8 <= 0xb)
            return sub_0822E728(a - 8);
        break;
    case 0:
        if (a <= 7)
            return sub_0822EAE4(a);
        break;
    case 2:
        if (a <= 0xf)
            return sub_0822DF38(a);
        break;
    case 3:
        if (a <= 0xf)
            return sub_0822E224(a);
        break;
    }
    return 0;
}
