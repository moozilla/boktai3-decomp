#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
/* Blink forever: toggle the phase every 16 frames; odd phases set display
 * flag 0x100. */
void SceneCharacter_UpdateContinuousBlink(void *context, struct SceneActor *actor)
{
    u8 phase;
    u32 t;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->ticks = 0x10;
    }
    phase = actor->animationPhase;
    if (phase & 1)
        SceneActor_SetFlags(actor, 0x100);
    t = actor->ticks;
    if (t != 0) {
        t--;
        actor->ticks = t;
        if (t == 0) {
            actor->animationPhase = (phase + 1) & 1;
            actor->ticks = 0x10;
        }
    }
}
