#include "global.h"
extern u32 gUnk_020000B0;
void AnimData_Load(void *, u32);
void SpritePool_SetResource(u8 *p, u32 id)
{
    if (gUnk_020000B0)
        AnimData_Load(p + 0x38, id);
}
