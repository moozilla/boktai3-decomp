// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct SparkPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct BurstSpark {
    u32 flags;
    u8 pad4[3], priority;
    u8 pad8[4];
    void *animation;
    u16 frame;
    u8 pad12[0xA];
    struct SparkPosition position;
    u8 pad24[8];
    struct SparkPosition velocity;
    u8 direction, delay;
    u16 speed, timer;
    u8 pad3A[2], active, mode, pad3E[2];
};
struct BurstSparkPool {
    u8 pad0[0x18];
    u8 animation[0x1C];
};
extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
struct BurstSparkPool *BurstSpark_EnsurePool(void);
struct BurstSpark *BurstSpark_AllocateSlot(struct BurstSparkPool *);
void Sprite_InitUnlinked(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
static inline u32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline u32 LastRandom(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    {
        u32 offset = index * 2;
        register const u16 *lastTable asm("r3") = gUnk_0203B400;
        return *(const u16 *)(offset + (u32)lastTable);
    }
}
static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline s32 Scaled(s32 value, s32 speed)
{
    s32 sample = value;
    register s32 magnitude asm("r2") = speed;
    asm("" : : "r"(sample), "r"(magnitude));
    return Truncate(sample * magnitude);
}
// confidence=low: creates a randomized radial spark with optional palette selection.
#define BurstSpark_Create sub_08126AD4
s32 BurstSpark_Create(const struct SparkPosition *position, s32 priority, s32 delay, s32 direction,
                      s32 speed, s32 mode, s32 palette)
{
    struct BurstSparkPool *pool;
    struct BurstSpark *p, *sprite;
    const u16 *table;
    struct SparkPosition *velocity;
    if (!position)
        return 0;
    pool = gBurstSparkPool;
    if (!pool) {
        pool = BurstSpark_EnsurePool();
        if (!pool)
            return 0;
    }
    p = BurstSpark_AllocateSlot(pool);
    if (!p)
        return 0;
    sprite = p;
    Sprite_InitUnlinked(sprite, pool->animation, 0);
    sprite->frame = 0;
    sprite->flags &= ~1;
    sprite->priority = priority;
    sprite->position = *position;
    sprite->active = 1;
    sprite->timer = 0;
    sprite->direction = direction;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    sprite->delay = (RandomValue(table) & 7) + delay;
    sprite->speed = (RandomValue(table) & 7) + 1;
    velocity = &sprite->velocity;
    velocity->x = Scaled(gUnk_086149C4[(direction + 64) & 255], speed);
    velocity->y = 0;
    velocity->z = Scaled(gUnk_086149C4[direction & 255], speed);
    p->velocity.y = LastRandom() & 31;
    p->mode = mode;
    {
        s32 test = palette;
        asm("" : "+r"(test));
        if (test > 0) {
            void *anim = sprite->animation;
            register s32 bank asm("r2") = palette;
            asm("" : : "r"(anim), "r"(bank));
            AnimData_SelectPaletteBank(anim, bank + 0x153);
        }
    }
    return 0;
}
