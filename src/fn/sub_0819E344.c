#include "global.h"
#include "debug.h"

struct ActorRestart {
    u8 pad[0xA1];
    u8 flags;
    u8 padA2[0xAB - 0xA2];
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 f1;
    u8 f2;
    u8 padB1[0x100 - 0xB1];
    u32 saved;
    u8 pad104[0x384 - 0x104];
    u32 src384;
    u8 src388;
    u8 src389;
};
static inline u8 TakeFlag(struct ActorRestart *p)
{
    if (p->f2) {
        p->f2 = 0;
        p->f1 = 0;
        return TRUE;
    }
    return FALSE;
}
void ActorState_Restart(struct ActorRestart *arg)
{
    struct ActorRestart *p = arg;
    struct ActorRestart *base = p;
    u8 t;
    if (TakeFlag(p)) {
        DEBUG_PRINT();
    }
    t = p->f1;
    if (t) {
        u8 s = base->src388;
        u32 z = 0;
        u8 k;
        u32 w;
        p->b1 = z;
        p->b2 = z;
        p->b3 = s;
        k = base->src389;
        w = base->src384;
        p->b0 = k;
        p->saved = w;
        p->flags |= 1;
    }
}
