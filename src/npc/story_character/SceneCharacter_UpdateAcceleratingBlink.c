#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
s32 Actor_UpdateAcceleratingBlink(struct SceneActor *, u32, u32);
void SceneCharacter_UpdateAcceleratingBlink(void *context, struct SceneActor *actor, u32 tick)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->ticks = 0x10;
    }
    if (Actor_UpdateAcceleratingBlink(actor, tick, 0x10)) {
        SceneActor_SetFlags(actor, 0x100);
        actor->finished = 1;
    }
}
