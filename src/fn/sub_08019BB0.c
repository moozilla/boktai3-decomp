#include "global.h"
#include "ram.h"
void AnimData_Load(void *, u32);
void SpritePool_SetResource(u8 *p, u32 id)
{
    if (gSpritePoolManager)
        AnimData_Load(p + 0x38, id);
}
