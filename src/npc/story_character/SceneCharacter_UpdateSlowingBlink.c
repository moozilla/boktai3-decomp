#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
/* Blink for 15 frames: bit 0 of the phase sets display flag 0x100, and each
 * phase lasts one frame longer than the last. */
void SceneCharacter_UpdateSlowingBlink(void *context, struct SceneActor *actor)
{
    u32 t;
    u8 phase;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        actor->ticks = 1;
        actor->animationPhase = 1;
    }
    t = actor->ticks;
    if (t > 15) {
        actor->finished = 1;
        return;
    }
    phase = actor->animationPhase;
    if (phase & 1)
        SceneActor_SetFlags(actor, 0x100);
    if (t != 0) {
        u32 remaining = t - 1;
        actor->ticks = remaining;
        if (remaining == 0) {
            actor->animationPhase = phase + 1;
            actor->ticks = actor->animationPhase;
        }
    }
}
