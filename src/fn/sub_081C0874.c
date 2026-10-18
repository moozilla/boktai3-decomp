#include "global.h"
struct S {
    u8 f[0x30];
    void *a;
    u8 g[0x20];
    u32 n;
};
s32 RegistryF3E4_GetStateResult(void *);
void ScreenTransition_BeginCornerMotion(void *);
void SceneActorSequence_UpdateCenterMotion(struct S *p)
{
    p->n = p->n + 1;
    if (RegistryF3E4_GetStateResult(p->a) != 0)
        ScreenTransition_BeginCornerMotion(p);
}
