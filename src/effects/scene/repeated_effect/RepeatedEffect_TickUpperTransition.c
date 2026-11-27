#include "effects/scene/repeated_effect.h"
#include "engine/engine_globals.h"
void Sound_PlaySong(s32);
void ImpactBurst_EmitChargeSparkle(u32, u32, u32, u32, struct RepeatedEffectPosition *);
static inline u32 RepeatedEffect_OffsetCoordinate(u16 value, const u8 *sample)
{
    s32 base = value - 127;
    return base + *sample;
}
/* Adds two consecutive random-table bytes, centered on zero, to position.x and position.z. */
#define REPEATED_EFFECT_SCATTER_POSITION(position)                    \
    {                                                                  \
        const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;             \
        u32 index = gRandomIndex;                                      \
        const u8 *sampleX;                                             \
        const u8 *sampleZ;                                             \
        index++;                                                       \
        index &= 1023;                                                 \
        sampleX = (const u8 *)&table[index];                           \
        (position).x = RepeatedEffect_OffsetCoordinate((position).x, sampleX); \
        index++;                                                       \
        index &= 1023;                                                 \
        gRandomIndex = index;                                          \
        sampleZ = (const u8 *)&table[index];                           \
        (position).z = RepeatedEffect_OffsetCoordinate((position).z, sampleZ); \
    }
void RepeatedEffect_TickUpperTransition(struct RepeatedEffect *effect)
{
    if (!effect->stateFrame)
        Sound_PlaySong(0x15C);
    effect->stateFrame++;
    if (effect->stateFrame <= 7) {
        struct RepeatedEffectPosition position;
        effect->sprite.frame = 6;
        if (!(effect->stateFrame & 3)) {
            position = effect->sprite.position;
            REPEATED_EFFECT_SCATTER_POSITION(position);
            ImpactBurst_EmitChargeSparkle(1, 0, 1, 64, &position);
        }
    } else if (effect->stateFrame <= 15) {
        struct RepeatedEffectPosition position;
        effect->sprite.frame = 7;
        effect->stateFrame++;
        if (!(effect->stateFrame & 3)) {
            position = effect->sprite.position;
            REPEATED_EFFECT_SCATTER_POSITION(position);
            ImpactBurst_EmitChargeSparkle(1, 0, 1, 64, &position);
        }
    } else {
        effect->sprite.frame = 8;
        RepeatedEffect_SetState(effect, 2, RepeatedEffect_TickUpperState);
    }
}
