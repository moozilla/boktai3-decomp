#include "global.h"

void SceneActor_SpawnModeParticles(u8 *);

void Player_EmitPeriodicEffect(u8 *p)
{
    u16 *q;

    SceneActor_SpawnModeParticles(p);
    q = (u16 *)(p + 0x442);
    *q = *q + 1;
}
