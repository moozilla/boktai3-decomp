#include "global.h"
#include "ram.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct DropEntry {
    u8 pad0[0x1C];
    u32 renderFlags;
    u8 pad20[0x48 - 0x20];
    u8 sequence[0x10];
    void *animation;
    u8 pad5C[0x9C - 0x5C];
    s16 x, y, z;
    u8 padA2[0xAE - 0xA2];
    u16 kind;
    u8 active, tick;
    u8 padB2[2];
    void (*callback)(struct DropEntry *);
};
struct DropManager {
    u8 pad0[0x18];
    struct DropEntry entries[16];
};
struct DropManager *SpriteDrop_CreateManager(void);
s32 Terrain_GetCellClearance(void *);
void SpriteDrop_UpdateEntry(struct DropEntry *);
void SpriteDrop_OnExpire(struct DropEntry *);
void SpatialIndex_InitEntry(void *, struct PackedVec *, struct PackedVec *, struct PackedVec *, s32,
                            s32, s32, s32, s32, void (*)(struct DropEntry *), struct DropEntry *);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);

#define E (m->entries[i])
// confidence=low: claims a free drop entry at cell (x, y) and returns its index, or -1.
s32 sub_081160CC(s32 x, s32 y, s32 kind)
{
    struct DropManager *m;
    s32 i;
    if (!gCompactParticlePool) {
        m = SpriteDrop_CreateManager();
        if (!m)
            return -1;
    } else
        m = gCompactParticlePool;
    for (i = 0; i < 16; i++) {
        if (!E.active) {
            struct PackedVec size, offset, position;
            s32 clearance, duration;
            struct DropEntry *e;
            E.x = x << 3;
            E.y = y << 3;
            E.z = E.y - 96;
            e = &E;
            clearance = Terrain_GetCellClearance(&e->x);
            if (clearance < E.y)
                E.y = clearance;
            E.active = 1;
            E.tick = 1;
            E.callback = SpriteDrop_UpdateEntry;
            E.kind = kind;
            if ((u16)kind == 0)
                duration = 600;
            else
                duration = 200;
            size.x = 8;
            size.y = 6;
            size.z = 10;
            offset.x = 0;
            offset.y = -size.y;
            offset.z = -2;
            position.x = E.x;
            position.y = E.y - (E.z + 96);
            position.z = E.z;
            SpatialIndex_InitEntry((u8 *)e + 0x5C, &position, &size, &offset, 12, 5, E.kind + 3,
                                   duration, 12, SpriteDrop_OnExpire, e);
            Anim_SetSequence(e->sequence, E.animation, 2, (u8)E.kind, 0);
            E.renderFlags |= 1;
            return i;
        }
    }
    return -1;
}
#undef E
