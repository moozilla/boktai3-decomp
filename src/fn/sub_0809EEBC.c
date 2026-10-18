#define RouteActor_UpdateAnimationSequence sub_0809EEBC
#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_animation.h"
void sub_080A0874(u8 *);
struct RouteActorAnimationState {
    u8 unknown0[0x3D8];
    u8 sequencePhase;
};
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
void sub_0809EEBC(struct SceneActorRegistryActor *actor)
{
    struct RouteActorAnimationState *state = actor->context;
    if (TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 16);
        state->sequencePhase = 0;
    }
    if (state->sequencePhase)
        sub_080A0874((u8 *)actor);
    if (actor->field2B3) {
        switch (state->sequencePhase) {
            case 0:
            case 1:
            case 2:
            case 3:
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 14);
                break;
            case 5:
                actor->field2B0 = 1;
                return;
        }
        state->sequencePhase++;
    }
}
