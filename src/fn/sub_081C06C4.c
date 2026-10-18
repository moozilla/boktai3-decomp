#include "global.h"
struct S {
    u8 f[0x50];
    void (*cb)(void *);
    u32 n;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void sub_081C06F8(void *);
void SceneActorSequence_BeginFadeIn(struct S *p)
{
    ScreenTransition_SendCommand(0, 6, 4, 4, 4, 0xFFFF, 0);
    p->cb = sub_081C06F8;
    p->n = 0;
}
