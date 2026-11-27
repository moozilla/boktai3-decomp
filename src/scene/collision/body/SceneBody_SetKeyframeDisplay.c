#include "global.h"

u32 SceneBody_SetKeyframeDisplay(u8 *p, u32 v)
{
    *(u32 *)(p + 0x30) = v;
    return 1;
}
