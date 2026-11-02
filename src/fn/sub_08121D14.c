#include "global.h"

struct SpriteParticlePosition {
    u32 xy, z;
};
struct SpriteParticleSlot {
    struct SpriteParticlePosition position;
    u8 unk08[2];
    u8 active;
    s8 index;
};
struct SpriteParticleDisplay {
    u32 flags;
    u8 unk04[3];
    u8 layer;
    u8 unk08[4];
    void *animation;
    u8 unk10[0x1c];
};
struct SpriteParticlePool {
    u8 unk00[0x18];
    void *data;
    struct SpriteParticleDisplay sprites[8];
    u8 spriteData[0x1c];
    u8 animations[8][0x10];
    u32 occupied;
    struct SpriteParticleSlot slots[8];
};
// The same function as SceneParticle_AllocateSlot over this pool's layout.
static inline u8 IsUsed(u32 *used, u32 index, u32 one)
{
    if (*used & (one << index))
        return 1;
    return 0;
}

static inline void ReserveSlot(u32 *used, s32 index)
{
    *used |= 1U << index;
}

struct SpriteParticleSlot *SpriteParticle_AllocateSlot_08121D14(struct SpriteParticlePool *pool)
{
    s32 i = 0;
    u32 *used = &pool->occupied;

    for (; i < 8; i++) {
        if (!IsUsed(used, i, 1)) {
            struct SpriteParticleSlot *slot = &pool->slots[i];
            slot->index = i;
            ReserveSlot(used, i);
            return slot;
        }
    }
    return NULL;
}
