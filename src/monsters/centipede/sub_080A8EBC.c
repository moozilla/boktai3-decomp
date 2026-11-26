#include "global.h"
struct Entry080A8EBC {
    u8 pad0[0x4CC - 0x3D4];
    u16 f4CC;
    u8 pad4CE[0x4D9 - 0x4CE];
    u8 flags;
    u8 pad4DA[0x128 - 0x4DA + 0x3D4];
};
struct Body080A8EBC {
    u8 pad0[0x3d4];
    struct Entry080A8EBC entries[4];
    u8 pad874[0x8da - 0x874];
    s16 count;
};
inline static u8 TestMask(struct Entry080A8EBC *e, char mask)
{
    if (e->flags & mask)
        return 1;
    return 0;
}
void TentacleActor_AssignSegmentPhases(struct Body080A8EBC *g)
{
    s32 i;
    s32 k = 0;
    for (i = 0; i < g->count; i++) {
        if (!TestMask(&g->entries[i], 5)) {
            g->entries[i].f4CC = k;
            k++;
            if (k > 3)
                k = 0;
        }
    }
}
