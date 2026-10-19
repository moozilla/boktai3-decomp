#define SceneActorApproach_UpdateRetreat sub_080BCF34
#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorApproachContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 minimumDistance, maximumDistance;
};
void sub_080B7C10(struct SceneActorRegistryResetView *);
static inline u8 TestFlags(struct SceneActorApproachContext *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}
static inline s32 Direction(s32 angle) { return ((angle >= 0 ? angle >> 5 : -(-angle >> 5)) + 3) & 7; }
static inline void ClearFlags(u32 *flags, u32 mask) { *flags &= mask; }
static inline void SetRetreat(struct SceneActorRegistryResetView *actor)
{
                u32 next = 17;
                actor->state2A8 = 0;
                actor->phase2A9 = 0;
                actor->behavior2AA = next;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
                ((struct SceneActorAnimationView *)actor)->nextDirection = Direction(actor->fieldE9);
}
void sub_080BCF34(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachContext *context = actor->context;
    if (!SceneActorReaction_TakePrepared(actor)) {
        if (TestFlags(context, 0x100)) {
            if (actor->distanceF0 > context->maximumDistance) {
                SetRetreat(actor);
            } else {
                ClearFlags(&context->flags, ~0x100);
                sub_080B7C10(actor);
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    }
}
