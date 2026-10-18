#include "global.h"
void SceneActorAnimation_SelectProfileEntry(u8 *, u32);
void Slime_ResetAnimation(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectProfileEntry(p, 1);
}
