#include "global.h"
void Callback_InvokeActor(void *, u32);
u32 ActorGlyphTrail_UpdateTask(u8 *s)
{
    Callback_InvokeActor(s, *(u32 *)(s + 0x13c));
    return 0;
}
