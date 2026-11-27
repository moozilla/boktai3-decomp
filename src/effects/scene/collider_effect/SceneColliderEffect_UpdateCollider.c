#include "global.h"
void SceneEffect_UpdateBlink(u32, u8 *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void SceneColliderEffect_UpdateCollider(u32 a, u8 *s)
{
    u8 *p, *q;
    SceneEffect_UpdateBlink(a, s);
    p = s + 0x44;
    q = s + 0x1c;
    Collider_SetPosition(p, q, 0);
    Collider_Refresh(p);
}
