#define SceneDialog_FindNextEnabledEntry sub_08176838
#include "global.h"
struct SceneDialogEntries {
    u8 pad0[0x1416];
    u8 enabled[10];
};
s32 sub_08176838(struct SceneDialogEntries *p, s32 current)
{
    s32 next = current + 1;
    s32 i;
    if (next > 9)
        next = 0;
    for (i = 0; i <= 9; i++) {
        if (p->enabled[next])
            return next;
        next++;
        if (next > 9)
            next = 0;
    }
    return current;
}
