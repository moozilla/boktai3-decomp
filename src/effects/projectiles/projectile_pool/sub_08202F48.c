#include "global.h"
struct TurnState {
    u8 pad0[4];
    u16 limit;
    u8 angle;
    s8 step;
};
struct TurningProjectile {
    u32 timer;
    u32 flags;
    u8 pad8[0x20];
    struct TurnState turn;
    u8 pad30[0xbc - 0x30];
    s16 pos[3];
    u8 padc2[0x120 - 0xc2];
    s16 *target;
};
s32 Math_Atan2(s32, s32);
void sub_08202F48(struct TurningProjectile *p)
{
    struct TurnState *t = &p->turn;
    s16 *target;
    if (p->timer < t->limit && (target = p->target) != NULL) {
        s16 *pos = p->pos;
        u8 angle = Math_Atan2(target[0] - pos[0], target[2] - pos[2]);
        s8 diff;
        u8 cur = t->angle;
        s32 sa = (s8)angle;
        diff = sa - cur;
        if (diff > t->step)
            t->angle += t->step;
        else if (diff < t->step)
            t->angle -= t->step;
        else
            t->angle = angle;
        {
            u32 one = 1;
            p->flags |= one;
        }
    }
}
