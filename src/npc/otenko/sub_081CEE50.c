#include "npc/otenko/otenko_actor.h"
#include "scene/motion/scene_facing.h"
#include "engine/engine.h"

s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
s32 Callback_InvokeActor(void *, void *);
void Actor_OnIdleUpdate(void);

void sub_081CEE50(void *ctx, struct OtenkoActor *npc)
{
    struct OtenkoEffectState *effect = &npc->effect;
    u32 play = 0;
    if (npc->animationEntered) {
        s32 a;
        u32 two;
        u32 f;
        npc->animationEntered = 0;
        npc->done = 0;
        a = npc->targetAngle;
        npc->body.direction = a;
        npc->facing8 = SceneFacing_EightWay(a);
        npc->facing4 = SceneFacing_FourWay(a);
        Actor_PlayAnimationEntry(npc, 4, 2, 0, 4);
        two = 2;
        f = npc->commandFlags;
        if (!(two & f))
            play = 1;
        {
            s32 three = 3;
            npc->commandFlags = f & -three;
        }
        {
            u32 mask = 0x100;
            npc->displayFlags = mask | npc->displayFlags;
        }
        effect->bobOffset = 0;
    }
    if ((u8)Callback_InvokeActor(npc, npc->animationCallback) != 1) {
        u32 one, zero;
        if (play)
            Sound_PlaySong(0x11E);
        one = 1;
        zero = 0;
        npc->displayFlags |= one;
        if (DisplayAnimation_TestFinished(&npc->display)) {
            void (*cb)(void);
            npc->done = one;
            Actor_PlayAnimationEntry(npc, 0, 1, 0, 4);
            cb = Actor_OnIdleUpdate;
            npc->prevState = 2;
            npc->stateCallback = cb;
            npc->stateFrame = 0;
            npc->animationEntered = one;
            npc->animationPhase = zero;
            if (!(npc->commandFlags & one)) {
                Sound_PlaySong(0x120);
                npc->commandFlags = ~one & npc->commandFlags;
            }
        }
    }
}
