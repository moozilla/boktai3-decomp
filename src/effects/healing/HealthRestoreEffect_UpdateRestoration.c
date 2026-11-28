#include "effects/healing/healing_effect.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine.h"

s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_StopSong(u32);

void HealthRestoreEffect_UpdateRestoration(struct HealthRestoreEffect *effect)
{
    s32 done, step;
    const u16 *table;

    if (effect->timer == 0) {
        effect->owner->currentStatA = 0;
        effect->timer++;
    }
    done = 0;
    step = Div(effect->owner->maximumStatA, 150);
    if (step == 0)
        step = 1;
    if ((effect->owner->currentStatA += step) >= effect->owner->maximumStatA) {
        effect->owner->currentStatA = effect->owner->maximumStatA;
        Sound_StopSong(0x259);
        done = 1;
    }
    table = gRandomTable;
    effect->icon.position.x = effect->targetPosition.x + Mod(HealingEffect_Random(table), 3) - 1;
    effect->icon.position.y = effect->targetPosition.y + Mod(HealingEffect_Random(table), 3) - 1;
    if ((effect->frame & 7) == 0) {
        HealthRestoreEffect_InitRisingParticle(&effect->particles[effect->nextParticle]);
        if (++effect->nextParticle > 7)
            effect->nextParticle = 0;
    }
    if ((effect->frame & 15) == 0) {
        HealthRestoreEffect_InitSparkle(&effect->particles[effect->nextParticle]);
        if (++effect->nextParticle > 7)
            effect->nextParticle = 0;
    }
    effect->frame++;
    if (done) {
        effect->state = HealthRestoreEffect_UpdateCompletion;
        effect->timer = 0;
    }
}
