#include "global.h"
#include "state_actor.h"
static inline void facing(struct StateActor *npc, short angle)
{
    npc->angle = angle;
    {
        s32 t = (s32)(angle + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(angle + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
}
void TaggedSceneNpc_PlayTaggedAnimation(void *, s32, s32, s32, s32);
// confidence=low: aligns facing to the target angle and starts tagged animation zero on entry.
#define SceneNpc_EnterIdleFacing sub_081D1A44
void SceneNpc_EnterIdleFacing(void *context, struct StateActor *p)
{
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
        facing(p, p->targetAngle);
        TaggedSceneNpc_PlayTaggedAnimation(p, 0, 1, 1, 4);
    }
}
