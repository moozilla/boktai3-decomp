#include "npc/otenko/scene_npc_transition_palette.h"

struct TransitionEffect *Otenko_CreateTransitionEffect(struct ContactPosition *);
s32 Otenko_IsTransitionEffectFinished(struct TransitionEffect *);
void Otenko_DestroyTransitionEffect(struct TransitionEffect *);

// confidence=low: animation 7 with a delayed transition effect and palette fade.
void Otenko_UpdateTransitionFade(void *ctx, struct OtenkoActor *npc, s32 tick)
{
    struct OtenkoEffectState *effect = &npc->effect;
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 7, 1, 0, 1);
    }
    npc->displayFlags |= 1;
    if (tick == 30) {
        struct ContactPosition position;
        struct ContactPosition *origin = &position;
        *origin = npc->body.position;
        origin->y += 0x109;
        effect->transition = Otenko_CreateTransitionEffect(origin);
    }
    if (tick > 30) {
        if ((tick & 7) >= 7 - (tick >> 3)) {
            u32 mask = 0x300;
            npc->displayFlags |= mask;
        }
        if (tick > 0x55) {
            u32 mask = 0x300;
            npc->displayFlags |= mask;
        }
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        Otenko_ApplyTransitionPalette(npc, tick);
        {
            struct SceneAnimation *graphics = &npc->display.resource.sequence.graphics;
            graphics->palette = npc->workPalette;
        }
        if (Otenko_IsTransitionEffectFinished(effect->transition)) {
            Otenko_DestroyTransitionEffect(effect->transition);
            npc->done = 1;
        }
    }
}
