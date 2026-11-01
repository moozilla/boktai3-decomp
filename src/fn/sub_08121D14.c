// MATCHING_CONCESSION: register-pinning
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
struct SpriteParticleSlot *SpriteParticle_AllocateSlot_08121D14(struct SpriteParticlePool *input)
{
    struct SpriteParticlePool *p = input;
    s32 i = 0;
    u32 *used = &p->occupied;
    u32 one = 1;
    u32 offset = i;
    for (; i < 8; offset += sizeof(struct SpriteParticleSlot), i++) {
        if (!IsUsed(used, i, one)) {
            struct SpriteParticleSlot *slot = (void *)((u8 *)p->slots + offset);
            slot->index = i;
            {
                u32 mask = one;
                DEBUG_NOP();
                mask <<= i;
                *used |= mask;
            }
            return slot;
        }
    }
    return 0;
}
