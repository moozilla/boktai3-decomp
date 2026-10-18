#include "global.h"
extern u32 gUnk_02000058;
void Sprite_Remove(u8 *);
void ScaledShadowManager_RemoveShadow(u32, u8 *);
u32 ScaledShadow_Destroy(u8 *p)
{
    Sprite_Remove(p + 0xc);
    if (gUnk_02000058 != 0)
        ScaledShadowManager_RemoveShadow(gUnk_02000058, p);
    return 0;
}
