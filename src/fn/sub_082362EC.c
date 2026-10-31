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

static inline s32 Scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

void RadialBurstEffect_StartBurst(struct RadialBurst *owner, struct RadialPosition *origin)
{
    s32 verticalVelocity = 0;
    u16 initialTimer = 0;
    u32 angle = (u8)NextRandom();
    struct RadialParticleState *state = &owner->particles[0].state;
    struct RadialParticle *particle = owner->particles;
    s32 i;

    for (i = 0; i < 12; i++) {
        s32 sineIndex;
        s32 sineValue;
        s32 zSpeed;
        void *drawable = &owner->particles[i];
        state->active = 1;
        {
            union {
                const u16 *random;
                const s16 *sine;
            } bank;
            s32 xSpeed;
            s32 cosineIndex;
            s32 cosine;
            const s16 *sine;
            u32 *seed;
            u16 index;
            u32 mask;
            const u16 *random;
            bank.random = (const u16 *)RANDOM_TABLE_ADDR;
            random = bank.random;
            seed = &gRandomIndex;
            index = *seed + 1;
            mask = 1023;
            index &= mask;
            {
                u32 jitterMask;
                u16 raw;
                raw = random[index];
                jitterMask = 15;
                jitterMask &= raw;
                raw = jitterMask;
                state->angle = raw + angle - 9;
                index = (index + 1) & mask;
                *seed = index;
                raw = random[index];
                raw &= 15;
                state->speed = raw + 25;
            }
            xSpeed = state->speed;
            sine = gUnk_086149C4;
            bank.sine = sine;
            cosineIndex = (state->angle + 64) & 255;
            cosine = bank.sine[cosineIndex];
            state->vx = Scale12(xSpeed * cosine);
        }
        state->vy = verticalVelocity;
        zSpeed = state->speed;
        sineIndex = state->angle;
        sineValue = gUnk_086149C4[sineIndex];
        state->vz = Scale12(zSpeed * sineValue);
        state->update = RadialBurstEffect_UpdateParticle;
        state->timer = initialTimer;
        particle->flags &= ~1;
        SpriteDrawable_SelectTileFrame(drawable, owner->graphics, state->tile);
        state->x = origin->x + state->vx;
        state->y = origin->y;
        state->z = origin->z + state->vz;
        particle->visible = 1;
        angle = (angle + 24) & 255;
        state = (struct RadialParticleState *)((u8 *)state + sizeof(*particle));
        particle++;
    }
    owner->active = 1;
}
