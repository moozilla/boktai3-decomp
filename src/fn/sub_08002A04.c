#include "global.h"
#include "background_controller.h"

void ScrollingBackground_QueueCommand(u32 layer, u32 type, s32 length, s32 *args)
{
    struct ScrollingBackground *p = gUnk_02000010[layer];
    if (p && p->queue.count <= 7) {
        s32 j;
        p->queue.types[p->queue.count] = type;
        for (j = 0; j < length; j++)
            p->queue.commands[p->queue.count].args[j] = *args++;
        p->queue.count++;
    }
}
