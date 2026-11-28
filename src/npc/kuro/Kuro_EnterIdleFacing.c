#include "global.h"
#include "npc/story_character/state_actor.h"
#include "scene/motion/scene_facing.h"
static inline void facing(struct StateActor *npc, short angle)
{
    npc->angle = angle;
    npc->facing8 = SceneFacing_EightWay(angle);
    npc->facing4 = SceneFacing_FourWay(angle);
}
void Kuro_PlayTaggedAnimation(void *, s32, s32, s32, s32);
// confidence=low: aligns facing to the target angle and starts tagged animation zero on entry.
void Kuro_EnterIdleFacing(void *context, struct StateActor *p)
{
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
        facing(p, p->targetAngle);
        Kuro_PlayTaggedAnimation(p, 0, 1, 1, 4);
    }
}
