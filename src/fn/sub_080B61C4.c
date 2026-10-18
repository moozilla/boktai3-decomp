#include "global.h"
#include "scene_actor_registry.h"
#include "animated_hazard.h"


struct VornData {
    u8 unknown0[0x678];
};

struct VornActorExtra {
    u8 unknown0[0xB4];
    u16 arrB4[4];
    u16 arrBC[4];
    u16 arrC4[4];
    u32 arrCC[4];
};

void Vorn_SpawnHazard(struct SceneActorRegistryActor *actor, s32 index)
{
    struct VornData *ctx = (struct VornData *)actor->context;
    struct VornActorExtra *extra = (struct VornActorExtra *)actor;
    u16 b4 = extra->arrB4[index];
    u16 bc = extra->arrBC[index];
    u32 cc = extra->arrCC[index];
    u16 c4 = extra->arrC4[index];

    Collider_SetAttackProperties((u8 *)ctx + 0x678, b4, bc, 0, cc, c4);
}
