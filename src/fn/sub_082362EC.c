// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"

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

extern const u16 gUnk_0203B400[];
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
    return *(const u16 *)(index * 2 + 0x0203B400);
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
// fuzzy: 91.8%
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
            register u32 table asm("r3") = 0x0203B400;
            u32 *seed = &gRandomIndex;
            u16 index = *seed + 1;
            u32 mask = 1023;
            index &= mask;
            {
                u32 raw = *(const u16 *)(index * 2 + table);
                u32 smallMask = 15;
                raw &= smallMask;
                state->angle = raw + angle - 9;
            }
            index = (index + 1) & mask;
            *seed = index;
            state->speed = (*(const u16 *)(index * 2 + (u32)table) & 15) + 25;
        }
        Store16(&state->vx, Scale12(state->speed * gUnk_086149C4[(state->angle + 64) & 255]));
        {
            register u32 zero asm("r2") = 0;
            state->vy = zero;
        }
        {
            register s32 speed asm("r0") = state->speed;
            Store16(&state->vz, Scale12(speed * gUnk_086149C4[state->angle]));
        }
        state->update = RadialBurstEffect_UpdateParticle;
        {
            register u32 zero asm("r3") = 0;
            state->timer = zero;
        }
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
