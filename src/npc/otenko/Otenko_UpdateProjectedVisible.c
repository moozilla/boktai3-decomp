#include "npc/otenko/otenko_actor.h"

void GroundEffect_EnableProjection(struct SceneGroundEffect *);
s32 Player_GetHorizontalVelocity(s32);

void Otenko_UpdateProjectedVisible(void *unused, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        GroundEffect_EnableProjection(&npc->groundEffect);
        Actor_PlayAnimationEntry(npc, 0, 1, 0, 4);
    }
    {
        u32 m = 0x100;
        npc->displayFlags = m | npc->displayFlags;
    }
    npc->projection.position.x += Player_GetHorizontalVelocity(0);
    npc->animationCallback(npc);
}
