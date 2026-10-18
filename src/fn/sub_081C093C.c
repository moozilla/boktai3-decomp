#include "global.h"
struct S {
    u8 f[0x30];
    void *a;
};
s32 RegistryF3E4_GetStateResult(void *);
void SceneActorSequence_BeginFinalWaypointMotion(void *);
void SceneActorSequence_UpdateSecondWaypointMotion(struct S *p)
{
    if (RegistryF3E4_GetStateResult(p->a) != 0)
        SceneActorSequence_BeginFinalWaypointMotion(p);
}
