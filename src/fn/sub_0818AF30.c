#include "global.h"
void Collider_Init(void *, u32, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, s32, s32);
void Collider_SetPosition(void *, s32, s32);
void sub_0818AF30(void *a, u16 b, s32 c, s32 d, s32 e, s32 f, s32 g)
{
    Collider_Init(a, b, 0x2101, 0, 0x10, e, d);
    if (f != 0)
        Collider_SetCallback(a, f, g);
    Collider_SetPosition(a, c, 0);
}
