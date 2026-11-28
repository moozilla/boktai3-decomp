#include "effects/healing/healing_effect.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine.h"

s32 Mod(s32, s32);

void HealthRestoreEffect_UpdateShake(struct HealthRestoreEffect *effect)
{
    effect->timer++;
    if (effect->timer < 8) {
        s32 range = 8 - effect->timer;
        const u16 *table = gRandomTable;

        effect->icon.position.x =
            effect->targetPosition.x + Mod(HealingEffect_Random(table), range) - (range >> 1);
        effect->icon.position.y =
            effect->targetPosition.y + Mod(HealingEffect_Random(table), range) - (range >> 1);
    }
    if (effect->timer > 29) {
        effect->state = HealthRestoreEffect_UpdateRestoration;
        Sound_PlaySong(0x259);
        effect->timer = 0;
    }
}
