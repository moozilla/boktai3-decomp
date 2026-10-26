#include "global.h"

/* Motion and activity fields following the drawable sprite header. */
struct PoolParticleTail {
    u32 position[2];
    u8 unk_08[8];
    u8 active, angle, riseSpeed, expandDelay;
    u8 animationPhase, angularSpeed, lifetime, state;
    u16 radius;
    u8 kind, unk_1B;
    s16 velocityX, velocityY, velocityZ, unk_22;
};
struct PoolParticle {
    u32 flags;
    u8 unk_04[11], renderMode, priority;
    u8 unk_11[7];
    struct PoolParticleTail tail;
};
struct ParticlePool {
    u8 unk_00[0x70];
    void *resource;
    struct PoolParticle particles[16];
};
extern u32 gRandomIndex;
void *AnimData_FindEntry(u32);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

static inline u32 RandomByte(void)
{
    u32 offset;
    u32 *index = &gRandomIndex;
    u32 next = *index + 1;
    u32 mask = 1023;
    *index = next & mask;
    offset = *index * 2;
    return *(const u8 *)(offset + RANDOM_TABLE_ADDR);
}
static inline s32 RandomValue(const u16 *table, u32 *index, u32 mask)
{
    u32 offset;
    *index = (*index + 1) & mask;
    offset = *index * 2;
    return *(const u16 *)(offset + (u32)table);
}

/* Tentative name: ParticlePool_InitParticles. Kind is retained from the slot. */
void ParticlePool_InitParticles(struct ParticlePool *pool)
{
    s32 i;
    u32 *index;
    u32 mask;
    u32 base = RandomByte();
    s32 angleOffset;
    u32 zero;
    struct PoolParticle *sprite;
    struct PoolParticleTail *tail;
    pool->resource = AnimData_FindEntry(0x1C1E);
    i = 0;
    index = &gRandomIndex;
    mask = 1023;
    zero = 0;
    tail = &pool->particles[0].tail;
    sprite = pool->particles;
    angleOffset = 0;
    for (; i < 16; i++) {
        const u16 *table;
        ParticleSprite_Initialize(sprite, pool->resource, 1);
        Effect_SetOrigin(sprite, -4, -4);
        Effect_SetPalette(sprite, 1);
        table = (const u16 *)RANDOM_TABLE_ADDR;
        tail->angle = base + angleOffset + (RandomValue(table, index, mask) & 7);
        tail->angularSpeed = (RandomValue(table, index, mask) & 3) + 7;
        tail->radius = ((RandomValue(table, index, mask) >> 3) & 0x5F) + 0x60;
        tail->riseSpeed = ((i & 3) << 1) + 1;
        tail->expandDelay = (RandomValue(table, index, mask) & 0x1F) + 8;
        {
            vu32 fill = zero;
            CpuSet((const void *)&fill, tail->position, 0x05000002);
        }
        sprite->renderMode = 2;
        sprite->priority = 0xEC;
        {
            u8 phase = RandomValue(table, index, mask) & 7;
            tail->animationPhase = phase;
            switch (tail->kind) {
                case 0:
                    if (phase <= 3)
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 2);
                    else
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 3);
                    break;
                case 1:
                    if (phase <= 3)
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 4);
                    else
                        SpriteDrawable_SelectTileFrame(sprite, pool->resource, 5);
                    break;
            }
        }
        tail->animationPhase = zero;
        tail->state = zero;
        tail->lifetime = zero;
        tail->active = zero;
        tail = (struct PoolParticleTail *)((u8 *)tail + sizeof(*sprite));
        table = (const u16 *)RANDOM_TABLE_ADDR;
        sprite++;
        angleOffset += 16;
    }
}
