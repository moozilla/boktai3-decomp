#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
/* Blink for 15 frames: bit 0 of the phase sets display flag 0x100, and each
 * phase lasts one frame longer than the last. */
void SceneCharacter_UpdateSlowingBlink(u8 *a, struct SceneActor *b)
{
    u32 t;
    u8 phase;
    if (b->animationEntered) {
        b->animationEntered = 0;
        b->finished = 0;
        b->ticks = 1;
        b->animationPhase = 1;
    }
    t = b->ticks;
    if (t > 15) {
        b->finished = 1;
        return;
    }
    phase = b->animationPhase;
    if (phase & 1)
        SceneActor_SetFlags(b, 0x100);
    if (t != 0) {
        u32 left = t - 1;
        b->ticks = left;
        if (left == 0) {
            b->animationPhase = phase + 1;
            b->ticks = b->animationPhase;
        }
    }
}
