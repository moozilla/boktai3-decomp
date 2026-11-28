#include "npc/otenko/scene_npc_transition_palette.h"

// confidence=low: Otenko_UpdateTransitionFade's palette fade alone: copies the start bank on
// entry, fades to the end bank over ticks 0x50-0x6F, and shows the end bank while the
// callback reports 1.
void sub_081CF398(void *ctx, struct OtenkoActor *npc, s32 tick)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        CpuSet(gPaletteBankData + 0x2B00, npc->workPalette, 0x04000008);
        Actor_PlayAnimationEntry(npc, 7, 1, 0, 1);
    }
    if ((u8)npc->animationCallback(npc) == 1) {
        struct SceneAnimation *graphics = &npc->display.resource.sequence.graphics;
        graphics->palette = gPaletteBankData + 0x2AE0;
    } else {
        Otenko_ApplyTransitionPalette(npc, tick);
        {
            struct SceneAnimation *graphics = &npc->display.resource.sequence.graphics;
            graphics->palette = npc->workPalette;
        }
    }
}
