#include "effects/healing/healing_effect.h"
#include "engine/engine.h"
#include "engine/engine_globals.h"
#include "engine/engine_pause_flags.h"

void Sound_ResumePersistentSongs(void);

void HealthRestoreEffect_UpdateCompletion(struct HealthRestoreEffect *effect)
{
    if (effect->timer == 30) {
        HealthRestoreEffect_StartParticleBurst(effect);
        Sound_PlaySong(0x285);
        effect->icon.flags |= 1;
    }
    effect->timer++;
    if (effect->timer > 59) {
        Sound_ResumePersistentSongs();
        Engine_ClearPauseFlags(4);
        gEngineFlags &= ~1;
        Task_Kill(effect);
    }
}
