#include "player_cue.h"

void SceneActor_SpawnModeParticles(struct Player *);
void PlayerCue_UpdatePeriodicParticles(struct Player *p)
{
    u8 *frame;
    s32 next;
    if (PlayerCue_Messages(p)->periodicParticlesEnabled != 0) {
        frame = &PlayerCue_Messages(p)->periodicParticleFrame;
        next = *frame + 1;
        *frame = next;
        if ((u8)next > 7) {
            SceneActor_SpawnModeParticles(p);
            *frame = 0;
        }
    }
}
