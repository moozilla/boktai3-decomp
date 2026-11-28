#include "effects/healing/healing_effect.h"
#include "engine/engine.h"
#include "engine/engine_pause_flags.h"
#include "engine/math/math_shift.h"
#include "engine/ram.h"

s32 Div(s32, s32);
void MusicSlots_StopAllAndRetainResumable(void);

// confidence=low: the 16-frame entry of the health restore effect. Stops the music, then slides
// the sprite from its start to its target position with a sine bob; afterwards it starts the
// shake.
void sub_08234DC8(struct HealthRestoreEffect *effect)
{
    if (effect->timer == 0) {
        MusicSlots_StopAllAndRetainResumable();
        Engine_SetPauseFlags(4);
    } else if (effect->timer == 1) {
        Sound_PlaySong(0xE6);
        effect->icon.flags &= ~1;
    }
    if (++effect->timer > 15) {
        effect->icon.flags &= ~2;
        HealthRestoreEffect_StartParticleBurst(effect);
        effect->state = HealthRestoreEffect_UpdateShake;
        effect->timer = 0;
        Sound_PlaySong(0x284);
        return;
    }
    effect->icon.scaleX = effect->icon.scaleY = effect->timer + 34;
    effect->icon.position.x = Div(effect->targetPosition.x * effect->timer +
                                      effect->startPosition.x * (16 - effect->timer),
                                  16);
    effect->icon.position.y =
        Div(effect->targetPosition.y * effect->timer +
                effect->startPosition.y * (16 - effect->timer),
            16) -
        Math_ShiftTowardZero(gUnk_086149C4[Div(effect->timer << 7, 20) & 255] << 4, 12);
}
