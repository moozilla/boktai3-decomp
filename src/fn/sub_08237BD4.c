#include "global.h"
struct GlyphTrail {
    u8 p0[0x18];
    u8 *owner;
    u32 flags;
    u8 p20[0xC];
    u16 frame;
    u8 p2e[0x40];
    u8 dir;
    u8 p6f;
    u16 timer;
};
void Task_Kill(void *);
void sub_082377B8(void *);
void ActorGlyphTrail_UpdateParticles(void *);
void sub_08237A28(void *);
void ActorGlyphTrail_HideParticles(void *);
void ActorGlyphTrail_SetState(void *, void (*)(void));
void TimedSprite_UpdateMotion(void);
void ActorGlyphTrail_Update(struct GlyphTrail *p)
{
    u32 state = p->owner[0x457];
    u8 *dp, *q;
    u16 *tp;
    if (state != 3) {
        Task_Kill(p);
        return;
    }
    sub_082377B8(p);
    if (p->owner[0x458] == 5) {
        ActorGlyphTrail_HideParticles(p);
        p->flags &= ~1;
        p->frame = p->p6f;
        ActorGlyphTrail_SetState(p, TimedSprite_UpdateMotion);
        return;
    }
    ActorGlyphTrail_UpdateParticles(p);
    if (p->owner[0x458] == 2) {
        u16 *tmp = &p->timer;
        state &= *tmp;
        tp = tmp;
        if (state == 3)
            sub_08237A28(p);
        q = &p->p6f;
        *q = 0;
        dp = q;
    } else {
        p->flags &= ~1;
        q = &p->p6f;
        *q = 1;
        dp = q;
        tp = &p->timer;
    }
    if ((*tp >> 2) & 1)
        p->frame = *dp + 1;
    else
        p->frame = *dp;
    (*tp)++;
}
