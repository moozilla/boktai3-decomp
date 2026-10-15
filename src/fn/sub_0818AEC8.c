#include "global.h"
void Collider_Init(void *, u32, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, s32, s32);
s32 Collider_Refresh(void *);
void Collider_SetPosition(void *, s32, s32);
void sub_0821FF7C(void *, s32, s32, s32);
void sub_0818AEC8(void *a, u16 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h)
{
    Collider_Init(a, b, 0x4101, 0, 0x10, f, e);
    if (g != 0)
        Collider_SetCallback(a, g, h);
    Collider_Refresh(a);
    Collider_SetPosition(a, d, 0);
    sub_0821FF7C(a, c, 0x100, 0);
}
