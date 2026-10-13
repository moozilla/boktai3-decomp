#include "global.h"
#include "terrain_overlay.h"


void sub_08003B84(struct TerrainOverlay *s, u16 id)
{
    struct TerrainOverlayEntry *e = s->entries;
    s32 i;
    for (i = 0; i < s->actorCount; e++, i++) {
        if (e->id == id) {
            e->used = 0;
            e->actor = 0;
            e->id = 0;
            return;
        }
    }
}
