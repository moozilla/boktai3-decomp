#include "global.h"

void SceneActor_SpawnModeParticles(u8 *);

void PlayerCue_UpdatePeriodicParticles(u8 *p)
{
    u8 *c;
    s32 v;

    if (p[0x5A1] != 0) {
        c = p + 0x5A2;
        v = *c + 1;
        *c = v;
        if ((u8)v > 7) {
            SceneActor_SpawnModeParticles(p);
            *c = 0;
        }
    }
}
