#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
void ScaledShadowManager_RemoveShadow(u32, u8 *);
u32 ScaledShadow_Destroy(u8 *p)
{
    Sprite_Remove(p + 0xc);
    if (gScaledShadowManager != 0)
        ScaledShadowManager_RemoveShadow(gScaledShadowManager, p);
    return 0;
}
