#include "scene_npc_transition_palette.h"

// confidence=low: SceneNpc_UpdateTransitionFade's palette fade alone: copies the start bank on
// entry, fades to the end bank over ticks 0x50-0x6F, and shows the end bank while the
// callback reports 1.
void sub_081CF398(void *ctx, struct SceneNpcTransitionActor *npc, s32 tick)
{
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        CpuSet(gPaletteBankData + 0x2B00, npc->palette, 0x04000008);
        Actor_PlayAnimationEntry(npc, 7, 1, 0, 1);
    }
    if (npc->callback(npc) == 1) {
        struct SceneNpcPaletteBinding *subp = &npc->binding;
        subp->paletteDest = gPaletteBankData + 0x2AE0;
        return;
    }
    SceneNpc_ApplyTransitionPalette(npc, tick);
    {
        struct SceneNpcPaletteBinding *subp = &npc->binding;
        subp->paletteDest = npc->palette;
    }
}
