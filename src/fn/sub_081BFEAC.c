#include "global.h"
void SceneHud_DrawNumericStatus(u32);
void SceneHud_DrawClock(u32);
void sub_0824923C(u8 *, u32);
void sub_081BFEAC(u8 *p)
{
    u8 *q = p + 0x175E;
    SceneHud_DrawNumericStatus(*q);
    SceneHud_DrawClock(*q);
    sub_0824923C(p, *(u32 *)(p + 0x17F0));
}
