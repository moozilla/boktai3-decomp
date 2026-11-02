#ifndef GUARD_TRANSIENT_SPRITE_POOL_H
#define GUARD_TRANSIENT_SPRITE_POOL_H

#include "display_animation.h"

struct TransientSpriteGraphics {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    const u8 *palette;
    u8 unknown10[0xc];
};

struct TemporarySpriteSlot {
    u8 active, unknown1;
    s16 lifetime;
    struct DisplaySequenceSprite sprite;
    struct TransientSpriteGraphics graphics;
};

struct TemporarySpritePool {
    u8 task[0x18];
    struct TemporarySpriteSlot slots[16];
};

struct TemporarySpriteSlot *TransientSpritePool_FindFreeSlot(struct TemporarySpritePool *);
struct TemporarySpriteSlot *TransientSprite_Spawn(struct DisplaySequenceSprite *source,
                                                  struct TransientSpriteGraphics *graphics,
                                                  u32 lifetime, u32 flags, s32 palette,
                                                  u8 *override);

#endif
