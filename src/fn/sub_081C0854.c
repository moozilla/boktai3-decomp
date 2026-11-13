#include "global.h"
struct S {
    u8 f[0x30];
    void *a;
    u8 g[0x1c];
    void (*cb)(void *);
    u32 n;
};
void ContactImpactEffect_BeginTimedPosition(void *, u32, u32);
void SceneActorSequence_UpdateCenterMotion(void *);
void SceneActorSequence_BeginCenterMotion(struct S *p)
{
    ContactImpactEffect_BeginTimedPosition(p->a, 0, 0x5a);
    p->cb = SceneActorSequence_UpdateCenterMotion;
    p->n = 0;
}
