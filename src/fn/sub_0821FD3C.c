#include "global.h"
#include "ram.h"

struct NavHdr {
    u16 count;
    u16 pad;
    s32 offsets[1];
};
struct NavEntry {
    u16 count;
    u16 pad;
    s32 f4;
    s32 regionOffset;
};
struct Vec2 {
    s32 x, y;
};
struct Probe {
    u16 f0;
    u16 f2;
    u16 region;
    u16 index;
    s32 f8;
    struct Vec2 pos0;
    struct Vec2 pos1;
    struct Vec2 pos2;
};
struct World {
    u8 pad[0x14];
    struct NavHdr *nav;
};
u32 NavMesh_IsPointInsideRegion(void *, struct Vec2 *, u32);

// Declared non-void without a return statement: r0 is left unset.
u32 ProximityVolume_TranslateBounds(struct Probe *out, struct Vec2 *pos)
{
    struct NavHdr *hdr = ((struct World *)gTerrainMap)->nav;
    s32 i, j;

    out->region = 0xff;
    out->index = 0xff;
    for (i = 0; i < hdr->count; i++) {
        s32 off = i * 4;
        s32 *offs = hdr->offsets;
        struct NavEntry *e = (struct NavEntry *)((u8 *)hdr + *(s32 *)((u8 *)offs + off));
        u8 *region = (u8 *)e + e->regionOffset;
        for (j = 0; j < e->count; j++) {
            if (NavMesh_IsPointInsideRegion(region, pos, j)) {
                out->region = i;
                out->index = j;
            }
        }
    }
    out->f0 = 0;
    out->pos0 = *pos;
    out->pos2 = *pos;
    out->pos1 = *pos;
}
