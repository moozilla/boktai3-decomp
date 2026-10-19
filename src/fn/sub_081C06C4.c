#include "global.h"
struct S {
    u8 f[0x50];
    void (*cb)(void *);
    u32 n;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void SceneActorSequence_UpdateFadeIn(void *);
void SceneActorSequence_BeginFadeIn(struct S *p)
{
    ScreenTransition_SendCommand(0, 6, 4, 4, 4, 0xFFFF, 0);
    p->cb = SceneActorSequence_UpdateFadeIn;
    p->n = 0;
}
