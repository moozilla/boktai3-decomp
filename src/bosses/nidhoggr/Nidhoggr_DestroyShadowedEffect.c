#include "global.h"

void Collider_Remove(u8 *);
u32 Sprite_Remove(u8 *);
void ScaledShadow_Destroy(u8 *);

void Nidhoggr_DestroyShadowedEffect(u8 *p)
{
    Collider_Remove(p + 0x120);
    Sprite_Remove(p + 0x24);
    ScaledShadow_Destroy(p + 0x60);
}
