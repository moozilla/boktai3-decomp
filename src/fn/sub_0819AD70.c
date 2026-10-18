#include "global.h"

void SpriteComponent_Init(u8 *);
void SpriteComponent_Suspend(u8 *);
void SceneOrbitHazard_SetState(u8 *, u32);

u32 SceneOrbitHazard_Init(u8 *p)
{
    u8 *q = p + 0x18;
    SpriteComponent_Init(q);
    SpriteComponent_Suspend(q);
    SceneOrbitHazard_SetState(p, 0);
    return 0;
}
