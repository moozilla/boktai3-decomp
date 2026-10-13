#include "global.h"
#include "terrain_overlay.h"

u32 sub_08225984(u16);

void sub_08003B44(struct TerrainOverlay *s, u16 id)
{
    struct TerrainOverlayEntry *e = s->entries;
    s32 i = 0;
    while (i < s->actorCount) {
        if (e->used == 0)
            break;
        e++;
        i++;
    }
    if (i != s->actorCount) {
        e->actor = (struct TerrainOverlayActor *)sub_08225984(id);
        if (e->actor != 0) {
            e->used = 1;
            e->id = id;
        }
    }
}
