#include "global.h"
void FollowingSpriteEffect_SetStateCallback(u32 *p, u32 v)
{
    *p = v;
    *(u8 *)((u8 *)p + 0xcf) = 1;
}
