#include "global.h"

void Collider_SetPosition(void *, void *, s32);

s32 Ivy_UpdateColliderPosition(u8 *s)
{
    Collider_SetPosition(s + 0x134, s + 0x5c, 0);
    return 1;
}
