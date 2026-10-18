#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorCheckpointState {
    u8 unknown0[0x6ce];
    u16 currentTick, targetTick;
    u8 unknown6d2[4];
    u16 savedTick;
};
struct SceneActorCheckpointSequence {
    u8 unknown0[8];
    u16 frame;
    u8 unknownA[4];
    u16 timer;
};
void sub_080D62CC(struct SceneActorRegistryResetView *);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActor_UpdateAnimationCheckpoint(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorCheckpointState *state = actor->context;
    struct SceneActorCheckpointSequence *sequence = (void *)((u8 *)actor->drawable + 0x48);
    if (TakeFlag(&actor->field2B2)) {
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        mask = 0x100;
        actor->flags2D4 |= mask;
    }
    if (sequence->frame > 2) {
        if (sequence->frame == 4 && sequence->timer == 0)
            sub_080D62CC(actor);
    }
    if (actor->field2B3) {
        state->savedTick = state->currentTick;
        state->targetTick = state->currentTick + 30;
        actor->field2B0 = 1;
    }
}
