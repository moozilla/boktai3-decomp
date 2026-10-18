#include "global.h"
void Collider_Remove(u8 *);
void SceneActorEffects_ReleaseResources(u8 *);
u32 Skeleton_Destroy(u8 *p)
{
    Collider_Remove(p + 0x624);
    SceneActorEffects_ReleaseResources(p);
    return 0;
}
