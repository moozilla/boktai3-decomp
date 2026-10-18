#define MenuPanel_FindEmptyOppositeHalf sub_081BDDCC
#define MenuPanel_IsEntryEmpty sub_081BDAA8
#include "global.h"
struct MenuPanel;
s32 sub_081BDAA8(struct MenuPanel *, s32);
s32 sub_081BDDCC(struct MenuPanel *panel, u32 index)
{
    s32 start, end, i;
    if (index <= 15) {
        start = 16;
        end = 32;
        goto scan;
    }
    if (index - 16 <= 15)
        goto lower;
    goto none;
found:
    return i;
lower:
    start = 0;
    end = 16;
scan:
    for (i = start; i < end; i++) {
        if (sub_081BDAA8(panel, i))
            goto found;
    }
none:
    return -1;
}
