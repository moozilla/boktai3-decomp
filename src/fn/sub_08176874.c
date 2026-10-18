#define SceneDialog_FindPreviousEnabledEntry sub_08176874
#include "global.h"
struct SceneDialogEntries {
    u8 pad0[0x1416];
    u8 enabled[10];
};
s32 sub_08176874(struct SceneDialogEntries *p, s32 current)
{
    s32 previous = current - 1;
    s32 i;
    if (current == 0)
        previous = 9;
    for (i = 0; i <= 9; i++) {
        if (p->enabled[previous])
            return previous;
        if (previous == 0)
            previous = 9;
        else
            previous--;
    }
    return current;
}
