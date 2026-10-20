#include "global.h"

struct Npc081D1BA8 {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[0xF];
    u8 facing4, facing8, angle;
    u8 pad1C[0x65 - 0x1C];
    u8 motionAngle;
};
static inline void facing(struct Npc081D1BA8 *npc, short angle)
{
    npc->motionAngle = angle;
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
void TaggedSceneNpc_PlayTaggedAnimation(void *, int, int, int, int);

void SceneNpc_TurnTowardAngle(void *ctx, struct Npc081D1BA8 *npc)
{
    if (npc->entered) {
        s32 zero = 0;
        npc->entered = zero;
        npc->done = zero;
        TaggedSceneNpc_PlayTaggedAnimation(npc, 0, 1, 0, 4);
    }
    {
        s32 diff = npc->angle - npc->motionAngle;
        s32 ad = diff;
        if (ad < 0)
            ad = -ad;
        if (ad <= 8)
            npc->motionAngle += diff;
        else {
            diff = (u8)diff;
            if (diff > 0x7f)
                npc->motionAngle -= 8;
            else
                npc->motionAngle += 8;
        }
    }
    facing(npc, npc->motionAngle);
    TaggedSceneNpc_PlayTaggedAnimation(npc, 0, 1, 0, 4);
    if (npc->motionAngle == npc->angle)
        npc->done = 1;
}
