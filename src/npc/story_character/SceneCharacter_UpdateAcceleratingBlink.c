#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
s32 Actor_UpdateAcceleratingBlink(u8 *, u32, u32);
void SceneCharacter_UpdateAcceleratingBlink(u8 *a, struct SceneActor *b, u32 c)
{
    if (b->animationEntered) {
        b->animationEntered = 0;
        b->finished = 0;
        b->ticks = 0x10;
    }
    if (Actor_UpdateAcceleratingBlink(b, c, 0x10)) {
        SceneActor_SetFlags(b, 0x100);
        b->finished = 1;
    }
}
