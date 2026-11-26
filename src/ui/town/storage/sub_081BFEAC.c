#include "global.h"
void SceneHud_DrawNumericStatus(u32);
void SceneHud_DrawClock(u32);
void Callback_InvokeActor(u8 *, u32);
void Storage_Update(u8 *p)
{
    u8 *q = p + 0x175E;
    SceneHud_DrawNumericStatus(*q);
    SceneHud_DrawClock(*q);
    Callback_InvokeActor(p, *(u32 *)(p + 0x17F0));
}
