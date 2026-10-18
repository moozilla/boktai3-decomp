#include "global.h"

void sub_0821BC68(u8 *, u8 *);
void SceneBody_SetMovementProbe(u8 *, u8 *, s32, s32);

void RoamingObjectB_InitHomeBounds(u8 *p)
{
    u8 *q = p + 0xac;
    sub_0821BC68(q, p + 0x28);
    SceneBody_SetMovementProbe(p + 0x1c, q, 0x1e, 0x1e);
}
