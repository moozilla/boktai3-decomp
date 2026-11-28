#include "effects/healing/healing_effect.h"
#include "engine/engine.h"
#include "engine/engine_globals.h"

void Camera_RequestShake(s32);

void HealthBurstEffect_UpdateFadeOut(struct HealthBurstEffect *effect)
{
    Camera_RequestShake((16 - effect->timer) * 2);
    if (effect->timer <= 5)
        Collider_Refresh(&effect->collider);
    else if (effect->timer == 6)
        gEngineFlags &= ~1;
    effect->timer++;
    if (effect->timer > 15)
        Task_Kill(effect);
}
