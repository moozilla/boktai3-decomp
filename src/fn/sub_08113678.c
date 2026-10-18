#include "global.h"
void NativeEffect_SetState(u32 *p, u32 v)
{
    *p = v;
    *(u8 *)((u8 *)p + 0xce) = 1;
}
