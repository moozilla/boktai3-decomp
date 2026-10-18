#include "global.h"
struct ContactActorPool;

struct S {
    u8 f[0x9d8];
    struct ContactActorPool *contactPool;
};
struct S *SceneActors_GetBySlot(u32);
void ContactActorPool_RequestClear(struct ContactActorPool *);
void SceneActors_RequestContactPoolClear(void)
{
    struct S *p = SceneActors_GetBySlot(0);
    if (p && p->contactPool != 0)
        ContactActorPool_RequestClear(p->contactPool);
}
