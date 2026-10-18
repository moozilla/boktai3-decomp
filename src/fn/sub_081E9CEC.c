#include "global.h"
extern const u32 gUnk_08613D34[];
void Callback_InvokeActor(u8 *, u32);
void Actor_DispatchAnimationPhase(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08613D34[*(u16 *)(p + 0xF5A) >> 4]);
}
