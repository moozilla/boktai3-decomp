#include "global.h"

struct Pt {
    s16 x, y, z, w;
};

struct SceneNpc081D010C {
    u8 pad0[2];
    u8 state, sub, mode, pad5, kind, pad7, changed, entered;
    u8 padA[6];
    u32 unk10;
    u32 timer;
    u8 phase;
    u8 pad19[2];
    u8 angle;
    u8 pad1C[0x20 - 0x1C];
    struct {
        u8 pad[0x12];
        u16 w;
    } sub20;
    u8 pad34[0x54 - 0x34];
    struct Pt target;
    u8 pad5C[0x160 - 0x5C];
    struct Pt *posSrc;
    u8 pad164[0x23C - 0x164];
    struct Pt pos;
    u8 pad244[0x2CC - 0x244];
    void *callback;
    u8 pad2D0[0x328 - 0x2D0];
    s32 speed;
};

struct Msg081D010C {
    u8 pad[8];
    s16 x, y, z;
    s16 w;
    u16 dir;
};

u32 Math_Atan2(s32, s32);
void SceneNpc_UpdateProjectedMove(void);

void sub_081D010C(struct SceneNpc081D010C *npc, struct Msg081D010C *m)
{
    struct Pt *t;
    typeof(npc->sub20) *sp = &npc->sub20;
    s32 v, r;
    u32 ang;
    s32 zero, k;
    void *cb;
    npc->pos = *npc->posSrc;
    v = npc->speed;
    if (v >= 0)
        r = v >> 12;
    else
        r = -(-v >> 12);
    npc->pos.y = r;
    {
        s32 x = m->x, y = m->y, z = m->z;
        t = &npc->target;
        t->x = x;
        t->y = y;
        t->z = z;
    }
    sp->w = m->w;
    if (m->dir <= 7)
        ang = ((u32)(m->dir << 29) + 0xA0000000) >> 24;
    else {
        struct Pt *p = &npc->pos;
        ang = Math_Atan2(t->x - p->x, t->z - p->z);
    }
    npc->angle = ang;
    k = 0x24;
    zero = 0;
    npc->state = zero;
    do {
        npc->sub = zero;
    } while (0);
    npc->mode = k;
    npc->timer = zero & 0xFFFFFFFFu;
    npc->changed = 1;
    cb = SceneNpc_UpdateProjectedMove;
    npc->kind = 0x21;
    npc->callback = cb;
    npc->unk10 = zero;
    npc->entered = 1;
    npc->phase = zero;
}
