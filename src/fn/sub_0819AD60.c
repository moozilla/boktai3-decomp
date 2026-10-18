#include "global.h"

void SpriteComponent_Destroy(u8 *);

u32 SceneOrbitHazard_OnDestroy(u8 *p)
{
    SpriteComponent_Destroy(p + 0x18);
    return 0;
}
