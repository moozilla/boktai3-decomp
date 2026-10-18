#include "global.h"
struct BlinkingSprite {
    u32 flags;
    u8 unknown04[0xee];
    u8 active;
    s8 slot;
    u8 unknownF4[2];
    s16 blinkAge;
};
struct BlinkingSpritePool {
    u8 unknown00[0x38];
    u32 used;
    u8 unknown3C[0xbd0];
    u32 stopping;
};
extern const u8 gUnk_086107B0[];
s32 Mod(s32, s32);
void SparkParticleSource_Emit(struct BlinkingSprite *);
void Sprite_Remove(void *);
void Task_Kill(void *);
s32 BlinkingSprite_UpdateExpiry(struct BlinkingSpritePool *pool, struct BlinkingSprite *sprite)
{
    struct BlinkingSprite *copy = sprite;
    s32 age = sprite->blinkAge;
    u32 rate = 2;
    if (age <= 60) {
        rate = 0;
        if (age > 40) rate = 1;
    }
    if (Mod(sprite->blinkAge, gUnk_086107B0[rate]) == 0) {
        u32 flags = copy->flags;
        u32 bit = 1;
        if (flags & bit) {
            flags &= ~1;
            copy->flags = flags;
        } else {
            flags |= bit;
            copy->flags = flags;
            SparkParticleSource_Emit(sprite);
        }
    }
    if (sprite->blinkAge > 90) {
        u32 one, flags;
        SparkParticleSource_Emit(sprite);
        flags = copy->flags;
        one = 1;
        copy->flags = flags | one;
        pool->used &= ~(one << sprite->slot);
        Sprite_Remove(sprite);
        sprite->active = 0;
        if (!pool->used) {
            pool->stopping = one;
            Task_Kill(pool);
        }
        return 1;
    } else {
        sprite->blinkAge++;
        return 0;
    }
}
