#include "global.h"
struct GlyphTrailObj {
    u32 flags;
    u8 pad[12];
    u16 frame;
};
struct GlyphTrail {
    u8 p0[0x1c];
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 p2e[0x40];
    u8 dir;
    u8 p6f;
    u16 timer;
};
void Task_Kill(void *);
#define ActorGlyphTrail_OnPhase sub_08237CDC
void ActorGlyphTrail_OnPhase(struct GlyphTrail *p)
{
    p->timer++;
    if (p->timer == 1) {
        p->frame = 4;
    } else if (p->timer == 4) {
        u8 *dp = &p->dir;
        struct GlyphTrailObj *o = (struct GlyphTrailObj *)((u8 *)p + 0x1c);
        s32 d = *dp;
        if (d & 1)
            o->frame = 7;
        else if ((d >> 1) & 1)
            o->frame = 8;
        else
            o->frame = 6;
        if (d <= 2)
            o->flags &= ~12;
        else if (d <= 4)
            o->flags = (o->flags & ~4) | 8;
        else if (d <= 5)
            o->flags |= 12;
        else
            o->flags = (o->flags | 4) & ~8;
    } else if (p->timer > 6) {
        Task_Kill(p);
    }
}
