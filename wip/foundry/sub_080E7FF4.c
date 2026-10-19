#define SceneActor_CheckRouteCollision sub_080E7FF4
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_motion.h"
extern u32 gUnk_030053F4;
extern u32 gUnk_02000560[];
static inline u8 collisionMode(void)
{
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}
static inline void clearCollisionFlag(u16 *flags)
{
    u32 mask = ~0x200;
    *flags &= mask;
}
static inline u8 resolveCollision(struct SceneActorRegistryResetView *actor, u8 *group)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = &actor->position;
    struct SceneActorMotionSubject *target = (void *)actor->fieldDC;
    struct ContactPosition *destination = &target->position;
    s32 result;
    if (sub_0821F204(route, position, destination) &&
        (result = sub_0821F7A8(route, position, destination))) {
        if (result != 2)
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    clearCollisionFlag(&actor->flags2D4);
    return 0;
}
static inline s32 targetCategory(struct SceneActorMotionSubject *target)
{
    u32 value = target->playerIndex;
    s32 result = -1;
    if (value <= 3)
        result = value;
    return result;
}
static inline u32 categoryDisabled(s32 category, u32 mask)
{
    return gUnk_02000560[category] & mask;
}
s32 sub_080E7FF4(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    struct SceneActorMotionSubject *target;
    if (collisionMode()) {
        s32 category;
        target = (void *)actor->fieldDC;
        if (!target)
            goto fail;
        category = targetCategory(target);
        if (category < 0)
            goto fail;
        if (!categoryDisabled(category, 8))
            goto resolve;
        goto fail;
    } else {
        target = (void *)actor->fieldDC;
        if (!target)
            goto fail;
    }
resolve:
    if (resolveCollision(actor, group))
        return 1;
fail:
    return 0;
}
