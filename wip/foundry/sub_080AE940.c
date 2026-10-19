#define SceneActorReaction_CheckRouteApproach sub_080AE940
#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorRouteApproachContext {
    u8 unknown0[0x6C8];
    u32 minimumDistance, maximumDistance;
    u8 unknown6D0[4];
    u16 flags;
};
struct SceneActorReactionDestination {
    u8 unknown0[12];
    struct ContactPosition position;
};
void sub_080B2858(void *, u32);
u8 sub_080B335C(void *);
s32 sub_0824923C(void *, void *);
static inline u8 Approach_CheckRoute(struct SceneActorRegistryResetView *actor, void *route)
{
    struct ContactPosition *position = &actor->position;
    struct ContactPosition *destination =
        &((struct SceneActorReactionDestination *)actor->fieldDC)->position;
    s32 result;
    if (!sub_0821F204(route, position, destination)) {
        SceneActorPlacement_ClearFlags(&actor->flags2D4, ~0x200);
        return 0;
    }
    result = sub_0821F7A8(route, position, destination);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    SceneActorPlacement_ClearFlags(&actor->flags2D4, ~0x200);
    return 0;
}
void sub_080AE940(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRouteApproachContext *context = actor->context;
    u8 *group = (u8 *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    } else if (actor->distanceF0 > context->maximumDistance ||
               actor->distanceF0 < context->minimumDistance) {
        sub_080B2858(actor, 1);
    } else if (sub_080B335C(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (!Approach_CheckRoute(actor, *(void **)(group + 0x1FC))) {
            sub_0824923C(actor, *(void **)(group + 0x224));
        } else {
            u16 *flags = &context->flags;
            u32 mask = ~0x40;
            u32 value = *flags & mask;
            u8 zeroByte = 0;
            u32 zeroWord = 0;
            *flags = value;
            {
                u32 behavior = 21;
                actor->state2A8 = zeroByte;
                actor->phase2A9 = zeroByte;
                actor->behavior2AA = behavior;
                actor->counter2AC = zeroWord;
                actor->field2B1 = 1;
            }
        }
    }
}
