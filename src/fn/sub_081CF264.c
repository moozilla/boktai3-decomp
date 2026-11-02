#include "scene_npc_transition_palette.h"

struct Pair {
    u32 a, b;
};

void *SceneNpc_CreateTransitionEffect(struct SceneNpcTransitionPosition *);
s32 SceneNpc_IsTransitionEffectFinished(void *);
void SceneNpc_DestroyTransitionEffect(void *);

// confidence=low: animation 7 with a delayed transition effect and palette fade.
void SceneNpc_UpdateTransitionFade(void *ctx, struct SceneNpcTransitionActor *npc, s32 tick)
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
        npc->effect = SceneNpc_CreateTransitionEffect(vp);
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
        SceneNpc_ApplyTransitionPalette(npc, tick);
        {
            struct SceneNpcPaletteBinding *subp = &npc->binding;
            subp->paletteDest = npc->palette;
        }
        if (SceneNpc_IsTransitionEffectFinished(*effect)) {
            SceneNpc_DestroyTransitionEffect(*effect);
            npc->done = 1;
        }
    }
}
