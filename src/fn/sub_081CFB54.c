#include "global.h"
struct SceneNpc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[6];
    u32 frame;
    u8 pad14[4];
    u8 phase, facing4, facing8, angle;
    u8 pad1C[0x65 - 0x1C];
    u8 motionAngle;
    u8 pad66[0x2e4 - 0x66];
    u8 (*callback)(struct SceneNpc *);
};
void Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
static inline void facing(struct SceneNpc *npc, u32 angle)
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
#define SceneNpc_UpdateTurnInPlace sub_081CFB54
void SceneNpc_UpdateTurnInPlace(void *ctx, struct SceneNpc *npc)
{
    u8 facing8;
    if (npc->entered) {
        u32 zero = 0;
        npc->entered = zero;
        npc->done = zero;
        Actor_PlayAnimationEntry(npc, 0, 0, 0, 4);
        npc->phase = zero;
    }
    if (npc->callback(npc) == 1)
        return;
    facing8 = npc->facing8;
    switch (npc->phase) {
        case 2:
            facing(npc, (npc->motionAngle + 4) & 0xff);
            if (npc->frame & 0x20) {
                npc->frame = 0;
                npc->phase++;
            }
            break;
        case 0:
        case 4:
            facing(npc, (npc->motionAngle - 4) & 0xff);
        case 1:
        case 3:
            if (npc->frame & 0x10) {
                npc->frame = 0;
                npc->phase++;
            }
            break;
        case 5:
            npc->done = 1;
            break;
    }
    if (facing8 != npc->facing8)
        Actor_PlayAnimationEntry(npc, 0, 0, 0, 4);
}
