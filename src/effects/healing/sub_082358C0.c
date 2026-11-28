#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

s32 Div(s32, s32);

// Advance an eight-step position blend, then switch handlers.
void sub_082358C0(struct HealthBurstEffect *effect)
{
    effect->icon.flags &= ~1;
    if (++effect->timer > 7) {
        effect->icon.flags &= ~2;
        effect->state = sub_08235794;
        effect->timer = 0;
        Sound_PlaySong(0x284);
    } else {
        effect->icon.scaleX = effect->icon.scaleY = effect->timer * 2 + 32;
        effect->icon.position.x = Div(effect->targetPosition.x * effect->timer +
                                          effect->startPosition.x * (8 - effect->timer),
                                      8);
        effect->icon.position.y = Div(effect->targetPosition.y * effect->timer +
                                          effect->startPosition.y * (8 - effect->timer),
                                      8);
    }
}
