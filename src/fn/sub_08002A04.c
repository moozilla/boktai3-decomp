#include "global.h"
#include "scrolling_background.h"
#define ScrollingBackground_QueueCommand sub_08002A04
extern struct ScrollingBackground *gUnk_02000010[];
void ScrollingBackground_QueueCommand(u32 layer, u32 type, s32 length, s32 *args)
{
    struct ScrollingBackground *p = gUnk_02000010[layer];
    if (p && p->count <= 7) {
        s32 j;
        p->types[p->count] = type;
        for (j = 0; j < length; j++)
            p->args[p->count][j] = *args++;
        p->count++;
    }
}
