#include "global.h"
#include "ram.h"
void AnimData_Load(void *, u32);
u32 SceneActor14_InitManager(u8 *p)
{
    *(u32 *)(p + 0x34) = 0;
    gSpriteEffectPool = p;
    AnimData_Load(p + 0x18, 0x84EE);
    return 0;
}
