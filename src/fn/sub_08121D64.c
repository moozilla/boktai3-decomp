#define SpriteParticle_GetOrCreatePool SceneSpriteBurst_EnsureSingleton
#define gSpriteParticlePool            gSpriteGroup
#include "global.h"
#include "ram.h"

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
struct SpriteParticlePool *SceneSpriteBurst_EnsureSingleton(void);
struct SpriteParticleSlot *SpriteParticle_AllocateSlot_08121D14(struct SpriteParticlePool *);
void Sprite_Init(void *, void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);

s32 SpriteParticle_SpawnBatch(struct SpriteParticlePosition *position, s32 count, s32 layer,
                              s32 sequence, u32 palette)
{
    struct SpriteParticlePool *pool;
    struct SpriteParticleSlot *slot;
    struct SpriteParticleDisplay *sprite;
    s32 i;
    if (!position)
        return -1;
    if (gSpriteGroup == 0) {
        pool = SceneSpriteBurst_EnsureSingleton();
        if (!pool)
            return -1;
    } else {
        pool = gSpriteGroup;
    }
    for (i = 0; i < count; i++) {
        slot = SpriteParticle_AllocateSlot_08121D14(pool);
        if (!slot)
            return -1;
        sprite = &pool->sprites[slot->index];
        slot->position = *position;
        Sprite_Init(sprite, pool->spriteData, 0);
        Anim_SetSequence(pool->animations[slot->index], pool->data, (u16)sequence, 0, 0);
        AnimData_SelectPaletteBank(sprite->animation, palette);
        sprite->flags |= 1;
        sprite->layer = layer;
        slot->active = 1;
    }
    return 0;
}
