#include "global.h"

struct ActorHold {
    u8 pad[0xAA];
    u8 st;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 f1;
    u8 f2;
    u8 pad3[0xC4 - 0xB1];
    u32 cnt;
    u8 pad4[0x318 - 0xC8];
    u16 cooldown;
    u8 pad31A[0x383 - 0x31A];
    u8 flag383;
    u8 pad384[4];
    u8 src388;
};
static inline u8 TakeFlag(struct ActorHold *p)
{
    if (p->f2) {
        p->f2 = 0;
        p->f1 = 0;
        return TRUE;
    }
    return FALSE;
}
void ActorAction_FinishRecoveryAnimation(struct ActorHold *arg)
{
    struct ActorHold *p = arg;
    struct ActorHold *base = p;
    u8 t;
    if (TakeFlag(p)) {
        u32 v = 0x22;
        p->st = v;
    }
    t = p->f1;
    if (t) {
        u8 s = base->src388;
        u32 z = 0;
        p->b1 = z;
        p->b2 = z;
        p->b3 = s;
        p->cnt = z;
        p->f2 = 1;
        base->flag383 = 1;
        base->cooldown = 60;
    }
    p->cnt++;
}
