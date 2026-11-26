#include "npc/otenko/otenko_actor.h"

struct Msg081D010C {
    u8 pad[8];
    s16 x, y, z;
    s16 w;
    u16 dir;
};

void Otenko_UpdateProjectedMove(void);

void sub_081D010C(struct OtenkoActor *npc, struct Msg081D010C *m)
{
    struct ContactPosition *t;
    struct OtenkoEffectState *effect = &npc->effect;
    s32 v, r;
    u32 ang;
    s32 zero, k;
    void (*cb)(void);
    npc->savedPosition = *(const struct ContactPosition *)npc->display.position;
    v = npc->projection.height;
    if (v >= 0)
        r = v >> 12;
    else
        r = -(-v >> 12);
    npc->savedPosition.y = r;
    {
        s32 x = m->x, y = m->y, z = m->z;
        t = &npc->target;
        t->x = x;
        t->y = y;
        t->z = z;
    }
    effect->dashDuration = m->w;
    if (m->dir <= 7)
        ang = ((u32)(m->dir << 29) + 0xA0000000) >> 24;
    else {
        struct ContactPosition *p = &npc->savedPosition;
        ang = Math_Atan2(t->x - p->x, t->z - p->z);
    }
    npc->targetAngle = ang;
    k = 0x24;
    zero = 0;
    npc->stage = zero;
    do {
        npc->stageEntered = zero;
    } while (0);
    npc->state = k;
    npc->timer = zero & 0xFFFFFFFFu;
    npc->commandEntered = 1;
    cb = Otenko_UpdateProjectedMove;
    npc->prevState = 0x21;
    npc->stateCallback = cb;
    npc->stateFrame = zero;
    npc->animationEntered = 1;
    npc->animationPhase = zero;
}
