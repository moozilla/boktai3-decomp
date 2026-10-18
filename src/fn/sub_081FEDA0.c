#include "global.h"
#include "terrain_overlay.h"
#define SpritePoolControl_Init SpriteParticle_InitParams
struct SpritePoolControl {
    u8 unknown0[0x20];
    u32 resource;
    u8 unknown24[0x34 - 0x24];
};
struct SpritePoolEntry {
    u32 flags;
    u8 unknown4[0x10 - 4];
    u32 cleared[2];
    u8 unknown18[0x38 - 0x18];
    struct SpritePoolControl control;
    u8 animation[28];
    struct TerrainOverlayDrawable drawable;
};
void SpriteParticle_InitParams(struct SpritePoolControl *);
void Sprite_InitUnlinked(struct TerrainOverlayDrawable *, void *, s32);
static inline void ClearEntryFields(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
void SpritePool_InitEntry(void *pool, struct SpritePoolEntry *entry, s32 index)
{
    struct SpritePoolControl *control = &entry->control;
    SpriteParticle_InitParams(control);
    entry->flags = 0;
    ClearEntryFields(entry->cleared);
    AnimData_Load(entry->animation, control->resource);
    Sprite_InitUnlinked(&entry->drawable, entry->animation, 0x11);
}
