// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"

struct TransitionParticleState {
    u16 x, y, z;
    u8 unk_06[0xA];
    s16 active;
    s16 angle;
    s16 velocityY;
    u8 age, lifetime;
};
struct TransitionParticle {
    u32 flags;
    u8 unk_04[0x14];
    struct TransitionParticleState state;
};
struct TransitionEffect {
    u8 unk_00[0x60];
    s16 x, y, z;
    u8 unk_66[0x16];
    struct TransitionParticle particles[8];
};
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);

/* Negative angles scatter a particle around the origin; other angles start at the origin. */
void SceneNpc_SpawnTransitionParticle(struct TransitionEffect *effect, s32 angle)
{
    s32 i = 0;
    struct TransitionParticle *particle = effect->particles;
    struct TransitionParticleState *state = &particle->state;
    do {
        if (state->active == 0) {
            register const u16 *table asm("r6");
            u32 *seed;
            u32 mask;
            state->active = 1;
            state->angle = angle;
            state->age = 0;
            table = gUnk_0203B400;
            seed = &gRandomIndex;
            {
                u32 index = *seed + 1;
                mask = 0x3FF;
                *seed = index & mask;
            }
            state->lifetime = Mod(table[*seed], 10) + 10;
            if (angle < 0) {
                *seed = (*seed + 1) & mask;
                {
                    s32 random = Mod(table[*seed], 160);
                    u32 value = (u16)effect->x;
                    value -= 80;
                    state->x = value + random;
                }
                *seed = (*seed + 1) & mask;
                {
                    s32 random = Mod(table[*seed], 80);
                    u32 value = (u16)effect->y;
                    value -= 40;
                    state->y = value + random;
                }
                *seed = (*seed + 1) & mask;
                {
                    s32 random = Mod(table[*seed], 160);
                    u32 value = (u16)effect->z;
                    value -= 80;
                    state->z = value + random;
                }
                state->velocityY = 0;
            } else {
                state->x = effect->x;
                state->y = effect->y;
                state->z = effect->z;
                state->velocityY = 15;
            }
            particle->flags &= ~1;
            return;
        }
        particle++;
        state = (struct TransitionParticleState *)((u8 *)state + sizeof(*particle));
        i++;
    } while (i < 8);
}
