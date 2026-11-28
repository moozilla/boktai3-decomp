#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
/* Blink forever: toggle the phase every 16 frames; odd phases set display
 * flag 0x100. */
void SceneCharacter_UpdateContinuousBlink(u8 *a, struct SceneActor *b)
{
    u8 phase;
    u32 t;
    if (b->animationEntered) {
        b->animationEntered = 0;
        b->finished = 0;
        b->ticks = 0x10;
    }
    phase = b->animationPhase;
    if (phase & 1)
        SceneActor_SetFlags(b, 0x100);
    t = b->ticks;
    if (t != 0) {
        t--;
        b->ticks = t;
        if (t == 0) {
            b->animationPhase = (phase + 1) & 1;
            b->ticks = 0x10;
        }
    }
}
