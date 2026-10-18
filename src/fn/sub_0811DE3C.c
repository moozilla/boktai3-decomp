#include "global.h"
s32 sub_0811DD94(u8 *, s32);
s32 GameFlag_FindFirstSetIndex(u8 *a, s32 n)
{
    s32 i;
    for (i = 0; i < n; i++) {
        if (sub_0811DD94(a, i))
            return i;
    }
    return 0;
}
