#include "npc/otenko/scene_npc_transition_palette.h"
#include "objects/scene_object_slot.h"

void *Otenko_CreateTransitionEffect(struct SceneNpcTransitionPosition *);
s32 Otenko_IsTransitionEffectFinished(void *);
void Otenko_DestroyTransitionEffect(void *);

// confidence=low: animation 7 with a delayed transition effect and palette fade.
void Otenko_UpdateTransitionFade(void *ctx, struct SceneNpcTransitionActor *npc, s32 tick)
{
    void **effect = &npc->effect;
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 7, 1, 0, 1);
    }
    npc->flags |= 1;
    if (tick == 30) {
        struct SceneNpcTransitionPosition v;
        struct SceneNpcTransitionPosition *vp = &v;
        *(struct Pair *)vp = *(struct Pair *)&npc->pos;
        vp->y += 0x109;
        npc->effect = Otenko_CreateTransitionEffect(vp);
    }
    if (tick > 30) {
        if ((tick & 7) >= 7 - (tick >> 3)) {
            u32 m = 0x300;
            npc->flags |= m;
        }
        if (tick > 0x55) {
            u32 m = 0x300;
            npc->flags |= m;
        }
    }
    if (npc->callback(npc) != 1) {
        Otenko_ApplyTransitionPalette(npc, tick);
        {
            struct SceneNpcPaletteBinding *subp = &npc->binding;
            subp->paletteDest = npc->palette;
        }
        if (Otenko_IsTransitionEffectFinished(*effect)) {
            Otenko_DestroyTransitionEffect(*effect);
            npc->done = 1;
        }
    }
}
