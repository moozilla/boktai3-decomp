#include "global.h"
struct S {
    u8 f[0x5b8];
    u32 a;
};
void Callback_InvokeActor(void *, u32);
void AnimatedObject_AdvanceThreeFrameCycle(void *);
void SegmentDisplay_UpdateScreenBrightness(void *);
u32 ControlPanel_OnTaskUpdate(struct S *p)
{
    Callback_InvokeActor(p, p->a);
    AnimatedObject_AdvanceThreeFrameCycle(p);
    SegmentDisplay_UpdateScreenBrightness(p);
    return 0;
}
