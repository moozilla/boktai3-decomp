#include "global.h"
#include "engine_globals.h"

/* Tentative name: selects a route point while avoiding the two recent indices. */

extern u16 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *Path_GetPointData(u8 *);

struct SceneActorRouteHistory {
    u8 unk_00[2];
    u8 index;
    u8 unk_03;
    u8 *data;
    u8 *entry;
    u8 previousIndex;
};

static inline u32 Random(void)
{
    u16 *table = gUnk_0203B400;
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}

void SceneActorRoute_SelectRandomNextPoint(struct SceneActorRouteHistory *p)
{
    s32 previous = p->index;
    s32 i;
    s32 next;

    for (i = 0; i < 4; i++) {
        next = Mod(Random(), *p->data);
        if (next != previous && next != p->previousIndex)
            break;
    }
    if (next == previous || next == p->previousIndex) {
        next = previous + 1;
        if (next >= *p->data)
            next = 0;
    }
    p->previousIndex = previous;
    p->index = next;
    p->unk_03 = 0;
    p->entry = Path_GetPointData(p->data) + p->index * 8;
}
