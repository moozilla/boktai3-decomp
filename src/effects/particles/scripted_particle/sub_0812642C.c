#include "effects/particles/scripted_particle.h"
#include "engine/engine.h"
#include "engine/math/monster_particle_random.h"

s32 Mod(s32, s32);

void SpriteParticle_Create(const struct ContactPosition *position, s32 rise, s32 riseRange,
                           s32 speed, s32 speedRange, u32 riseDelay, s32 angle, s32 angleRange,
                           u32 durationScale, u32 priority, u32 unused, u32 flags, u32 hiddenFrames,
                           u32 graphicsId, u32 sequenceResourceId, u32 sequenceId, u32 row,
                           u32 paletteBank)
{
    struct ScriptedParticlePool *pool = (void *)gScriptedParticlePool;
    struct ScriptedParticle *particle;
    struct SpriteRenderResource *graphics;
    const u16 *table;
    if (!pool) {
        pool = ScriptedParticle_EnsurePool();
        if (!pool)
            return;
    }
    particle = ScriptedParticle_AllocateSlot(pool);
    if (!particle)
        return;
    particle->state = 1;
    particle->unknown66 = 0;
    particle->sprite.position = *position;
    graphics = &particle->graphics;
    AnimData_Load(graphics, graphicsId);
    pool->sequenceData = ScriptProc_Start(0x922E, (u16)sequenceResourceId);
    Sprite_Init(&particle->sprite, graphics, 0);
    Anim_SetSequence(&particle->sequence, pool->sequenceData, (u16)sequenceId, (u8)row, 0);
    AnimData_SelectPaletteBank(graphics, paletteBank);
    table = gRandomTable;
    particle->rise = Mod(ParticleRandom_ReadDirectSample(table), riseRange) + rise;
    particle->speed =
        (Mod(ParticleRandom_ReadDirectSample(table), speedRange) - (speedRange >> 1)) + speed;
    particle->angle = angle + Mod(ParticleRandom_ReadDirectSample(table), angleRange);
    particle->riseDelay = riseDelay;
    particle->durationScale = durationScale;
    particle->riseSteps = 12;
    particle->sprite.flags |= 1 | flags;
    particle->sprite.priority = priority;
    particle->hiddenFrames = hiddenFrames;
}
