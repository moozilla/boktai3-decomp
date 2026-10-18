#include "global.h"
struct S {
    u8 f[0x30];
    void *a;
    u8 g[0x1c];
    void (*cb)(void *);
    u32 n;
};
void RegistryF3E4_ClearState(void *);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void SceneActorSequence_UpdateCompletionDelay(void *);
void SceneActorSequence_BeginFadeOut(struct S *p)
{
    RegistryF3E4_ClearState(p->a);
    ScreenTransition_SendCommand(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->cb = SceneActorSequence_UpdateCompletionDelay;
    p->n = 0;
}
