#define RouteActor_UpdatePeriodicAnimation sub_0809EE2C
#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_animation.h"
s32 Mod(s32, s32);
struct RouteActorAnimationState {
    u8 unknown0[0x3D4];
    u32 flags;
    u8 unknown3D8[0x15];
    u8 phase;
};
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 TestFlag(struct RouteActorAnimationState *state, u32 mask)
{
    if (state->flags & mask)
        return 1;
    return 0;
}
void sub_0809EE2C(struct SceneActorRegistryActor *actor, s32 tick)
{
    struct RouteActorAnimationState *state = actor->context;
    if (TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    } else if (!TestFlag(state, 4)) {
        if (!Mod(tick, 90)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
            state->phase += 40;
        } else if (actor->field2B3) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        }
    }
}
