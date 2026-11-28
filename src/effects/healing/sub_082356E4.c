#include "effects/healing/healing_effect.h"
#include "engine/engine.h"
#include "engine/engine_globals.h"
#include "engine/engine_pause_flags.h"

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Camera_RequestShake(s32);

void sub_082356E4(struct HealthBurstEffect *effect)
{
    if (effect->timer == 0) {
        Engine_ClearPauseFlags(4);
        Sound_PlaySong(0x67);
        ScreenTransition_SendCommand(2, 4, 0x1F, 0x1F, 0x1F, 0x1FFF, 1);
    }
    if (effect->timer <= 5)
        Collider_Refresh(&effect->collider);
    else if (effect->timer == 6)
        gEngineFlags &= ~1;
    if (++effect->timer > 47) {
        Task_Kill(effect);
        return;
    }
    if (effect->timer <= 15)
        Camera_RequestShake(32);
    else
        Camera_RequestShake(48 - effect->timer);
}
