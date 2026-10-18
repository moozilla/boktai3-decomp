#include "global.h"

u32 SceneBody_SetPositionTarget(u8 *p, u32 v)
{
    *(u32 *)(p + 0x24) = v;
    return 1;
}
