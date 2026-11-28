#include "global.h"
struct MenuPanel;
s32 MenuPanel_IsEntryEmpty(struct MenuPanel *, s32);
s32 MenuPanel_FindEmptyOppositeHalf(struct MenuPanel *panel, u32 index)
{
    s32 start, end, i;
    if (index <= 15) {
        start = 16;
        end = 32;
    } else if (index - 16 <= 15) {
        start = 0;
        end = 16;
    } else {
        return -1;
    }
    for (i = start; i < end; i++) {
        if (MenuPanel_IsEntryEmpty(panel, i))
            return i;
    }
    return -1;
}
