#define SceneActor_AdvanceTimedPhase sub_080CEAC4
#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorPhaseState { u8 unknown0[0x688]; u16 timer; };
u8 sub_080C5234(struct SceneActorRegistryResetView *);
static inline u8 TakeFlag(u8 *flag) { if (*flag) { *flag = 0; return 1; } return 0; }
void sub_080CEAC4(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorPhaseState *state = actor->context;
    u8 *direction = (u8 *)actor + 0x50;
    TakeFlag((u8 *)actor + 0x2b1);
    {
        u8 busy = sub_080C5234(actor);
        if (!busy) {
            s32 *counter;
            direction[9] = actor->fieldE9;
            counter = (s32 *)((u8 *)actor + 0x2ac);
            if (*counter > 119) {
                u32 phase = 2;
                u32 behavior = 5;
                actor->state2A8 = busy;
                actor->phase2A9 = phase;
                actor->behavior2AA = behavior;
                actor->counter2AC = busy;
                actor->field2B1 = 1;
                state->timer = busy;
            }
            (*counter)++;
        }
    }
}
