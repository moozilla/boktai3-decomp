#include "global.h"
#include "engine.h"
#include "ram.h"

struct RadialPosition {
    s16 x, y, z;
};

struct RadialParticleState {
    s16 x, y, z;
    u8 unk_06[0xA];
    u8 active;
    u8 tile;
    u8 angle;
    u8 unk_13;
    u16 timer;
    u16 speed;
    u8 unk_18[8];
    s16 vx, vy, vz;
    u8 unk_26[2];
    void (*update)(void *);
};

struct RadialParticle {
    u32 flags;
    u8 unk_04[0xB];
    u8 visible;
    u8 unk_10[8];
    struct RadialParticleState state;
};

struct RadialBurst {
    u8 unk_00[0x13A];
    u8 active;
    u8 unk_13B;
    void *graphics;
    struct RadialParticle particles[12];
};

extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void RadialBurstEffect_UpdateParticle(void *);

static inline u32 NextRandom(void)
{
    u32 *seed = &gRandomIndex;
    u16 index = *seed + 1;
    u32 mask = 1023;
    index &= mask;
    *seed = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}

static inline void Store16(s16 *dst, s32 value)
{
    *dst = value;
}

static inline s32 Scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 84.8%
void RadialBurstEffect_StartBurst(struct RadialBurst *owner, struct RadialPosition *origin)
{
    u32 angle = (u8)NextRandom();
    struct RadialParticleState *state = &owner->particles[0].state;
    struct RadialParticle *particle = owner->particles;
    void *drawable = particle;
    s32 i = 11;

    do {
        state->active = 1;
        {
            const void *lookup = (const void *)RANDOM_TABLE_ADDR;
            u32 *seed = &gRandomIndex;
            u16 index = *seed + 1;
            u32 mask = 1023;
            u32 speed;
            index &= mask;
            {
                u32 raw = *(const u16 *)(index * 2 + (u32)lookup);
                u32 smallMask = 15;
                raw &= smallMask;
                state->angle = raw + angle - 9;
            }
            index = (index + 1) & mask;
            *seed = index;
            {
                u32 raw = *(const u16 *)(index * 2 + (u32)lookup);
                u32 smallMask = 15;
                raw &= smallMask;
                raw += 25;
                state->speed = raw;
            }
            speed = state->speed;
            lookup = gUnk_086149C4;
            Store16(&state->vx, Scale12(speed * ((const s16 *)lookup)[(state->angle + 64) & 255]));
        }
        state->vy = 0;
        {
            s32 speed = state->speed;
            Store16(&state->vz, Scale12(speed * gUnk_086149C4[state->angle]));
        }
        state->update = RadialBurstEffect_UpdateParticle;
        state->timer = 0;
        particle->flags &= ~1;
        SpriteDrawable_SelectTileFrame(drawable, owner->graphics, state->tile);
        state->x = origin->x + state->vx;
        state->y = origin->y;
        state->z = origin->z + state->vz;
        particle->visible = 1;
        angle = (angle + 24) & 255;
        state = (struct RadialParticleState *)((u8 *)state + sizeof(struct RadialParticle));
        drawable = (u8 *)drawable + sizeof(struct RadialParticle);
        particle++;
        i--;
    } while (i >= 0);
    owner->active = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", RadialBurstEffect_StartBurst);
#endif
