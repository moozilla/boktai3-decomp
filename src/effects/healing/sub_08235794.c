#include "effects/healing/healing_effect.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine.h"

s32 Mod(s32, s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

// confidence=low: the health burst's shake, as in HealthRestoreEffect_UpdateShake: jitters for
// 8 frames, starts the burst at frame 30 (sound 0x285), fades the screen at frame 29 in mode 1,
// then after frame 44 moves on to the bursts (mode 0) or sub_082356E4.
void sub_08235794(struct HealthBurstEffect *effect)
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
    if (effect->timer == 30) {
        FlickerParticle_StartSecondaryBurst(effect);
        Sound_PlaySong(0x285);
        effect->icon.flags |= 1;
    }
    if (effect->mode == 1) {
        if (effect->timer == 29)
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1FFF, 1);
    }
    if (effect->timer > 44) {
        if (effect->mode == 0)
            effect->state = HealthBurstEffect_UpdateBursts;
        else
            effect->state = sub_082356E4;
        effect->timer = 0;
    }
}
