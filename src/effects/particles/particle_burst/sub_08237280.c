#include "global.h"

// confidence=low: initializes eight 0x30-byte sprite entries at 0x18 with animation 0x1C1A, frame 12,
// origin (-8, -8), shaped like SpriteGroup_InitFourSprites.
struct PuffEntry {
    u32 flags;
    u8 pad4[0xb];
    u8 f0F;
    u8 pad10[0x18];
    void *resource;
    u8 state;
    u8 phase;
    u16 timer;
};
struct PuffPool {
    u8 unknown0[0x18];
    struct PuffEntry entries[8];
};
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

void sub_08237280(struct PuffPool *p)
{
    u32 zero = 0;
    struct PuffEntry *entry = p->entries;
    u8 *state = &p->entries[0].state;
    struct PuffEntry *sprite = entry;
    s32 offset = -8;
    s32 i = 7;
    do {
        state[0] = zero;
        state[1] = zero;
        entry->timer = zero;
        entry->resource = AnimData_FindEntry(0x1C1A);
        ParticleSprite_Initialize(sprite, entry->resource, 0);
        Effect_SetOrigin(sprite, offset, offset);
        SpriteDrawable_SelectTileFrame(sprite, entry->resource, 12);
        entry->flags |= 1;
        entry->f0F = 2;
        entry++;
        state += 0x30;
        sprite++;
        i--;
    } while (i >= 0);
}
