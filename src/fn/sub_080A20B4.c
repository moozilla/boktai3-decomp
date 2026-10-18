#include "global.h"
#include "scene_actor_behavior.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
void sub_08249244(void *, void *, s32, void *);
static inline u8 Unavailable(struct SceneActorBehaviorView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u32 TestFlag(struct SceneActorBehaviorContactView *s, u32 mask)
{
    return s->flags & mask;
}
void SceneActorContact_ApplyWithinBounds(struct SceneActorBehaviorContactView *source,
                                         struct SceneActorBehaviorContactView *destination,
                                         struct SceneActorBehaviorView *actor)
{
    struct ContactPosition point;
    s32 difference;
    s16 center, radius;
    if (Unavailable(actor))
        return;
    if (TestFlag(source, 0x1000)) {
        SceneActorBehavior_ApplyContactReaction(source, destination, actor);
        return;
    }
    difference = (source->position.y - destination->position.y) >> 1;
    point.x = source->position.x - difference;
    point.z = source->position.z - difference;
    center = destination->position.x;
    radius = destination->bounds.x;
    if (center + radius <= point.x)
        return;
    if (center - radius >= point.x)
        return;
    center = destination->position.z;
    radius = destination->bounds.z;
    if (center + radius <= point.z)
        return;
    if (center - radius >= point.z)
        return;
    if (source->contactCallback)
        sub_08249244(source, destination, source->contactArgument, source->contactCallback);
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
}
