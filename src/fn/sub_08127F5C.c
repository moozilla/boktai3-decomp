#define MovingSpritePool_GetOrCreate DirectionalProjectile_EnsurePool
#define MovingSpritePool_Allocate    DirectionalProjectile_AllocateSprite
#include "global.h"
#include "ram.h"
struct SpritePosition {
    u32 xy, z;
};
struct MovingSprite {
    u32 flags;
    u8 unknown04[3], priority;
    u8 unknown08[0x14];
    struct SpritePosition position;
    u8 unknown24[8];
    u8 animation[0x10];
    s16 velocityX;
    u16 initialRise;
    s16 velocityZ;
    u8 unknown42[0x56];
    u16 age;
    u8 active;
    s8 slot;
    u16 blinkAge;
    u8 unknown9E, angle;
};
struct MovingSpritePool {
    u8 unknown00[0x18];
    u8 graphics[0x1c];
    void *animationData;
};
extern const s16 gUnk_086149C4[];
struct MovingSpritePool *DirectionalProjectile_EnsurePool(void);
struct MovingSprite *DirectionalProjectile_AllocateSprite(struct MovingSpritePool *);
void TimedSprite_InitCollider(struct MovingSprite *, u32, u32, u32, u32);
void Sprite_Init(void *, void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
static inline void SelectDirection(u8 *angle, u8 *row, u8 *flip)
{
    u32 quadrant = ((*angle + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void MovingSprite_Create(struct SpritePosition *position, u32 angle, u32 arg2, u32 arg3, u32 arg4,
                         u32 arg5, s32 speed)
{
    struct MovingSpritePool *pool = gMovingSpritePool;
    struct MovingSprite *slot;
    struct MovingSprite *sprite;
    u8 row, flip;
    if (!pool) {
        pool = DirectionalProjectile_EnsurePool();
        if (!pool)
            return;
    }
    slot = DirectionalProjectile_AllocateSprite(pool);
    if (!slot)
        return;
    slot->active = 1;
    slot->age = 0;
    slot->angle = angle;
    slot->blinkAge = 0;
    TimedSprite_InitCollider(slot, arg2, arg3, arg4, arg5);
    sprite = slot;
    Sprite_Init(sprite, pool->graphics, 0);
    SelectDirection(&slot->angle, &row, &flip);
    Anim_SetSequence(slot->animation, pool->animationData, 0x12, row, flip);
    sprite->position = *position;
    slot->velocityX = Scale(gUnk_086149C4[(slot->angle + 0x40) & 0xff] * speed);
    slot->velocityZ = Scale(gUnk_086149C4[slot->angle] * speed);
    slot->initialRise = 10;
    sprite->flags |= 1;
    sprite->priority = 1;
}
