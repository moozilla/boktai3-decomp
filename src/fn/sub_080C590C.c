#include "global.h"
void SceneActorAnimation_SelectDirectionalEntry(u8 *, u32);
void Golem_UpdateIdle(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
}
