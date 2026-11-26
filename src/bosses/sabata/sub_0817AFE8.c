#include "global.h"

void SceneActors_SetParticleControlMode(u8 *p, u8 v)
{
    p[0x51f] = v;
}
