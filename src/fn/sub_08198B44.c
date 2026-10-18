#include "global.h"

void ScaledShadow_Destroy(u8 *);
void Collider_Remove(u8 *);
void Anim_Remove(u8 *);

void SceneEffect_DestroyActor(u8 *p)
{
    ScaledShadow_Destroy(p + 0x88);
    Collider_Remove(p + 0xF4);
    if (*(p + 0x2C)) {
        Anim_Remove(p + 0x28);
    }
}
