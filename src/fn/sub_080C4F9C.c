#include "global.h"

#ifdef NONMATCHING
// fuzzy: 92.1%
// foundry draft (worker best 86.8%); what differs is mostly register allocation and instruction order
struct Seq {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unkA, speed, elapsed;
};
struct Spr {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[0x36];
    struct Seq seq;
};
struct Inner {
    u8 pad0[0x624];
    u8 b624;
    u8 pad625[3];
    u32 w628;
    u8 pad62c[0x50];
    u32 w67c;
    u16 h680, h682, h684, h686, h688, h68a;
    u8 b68c, b68d, b68e, b68f, b690, b691;
    u8 pad692[2];
    u8 a694[8];
    u8 a69c[8];
    u8 a6a4[8];
    u16 h6ac, h6ae;
};
struct Obj {
    u8 pad0[4];
    struct Spr *spr;
};

void Memory_Clear(void *, int);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, int);

void sub_080C4F9C(u8 *p)
{
    struct Inner *in = *(struct Inner **)(p + 0x3d0);
    struct Spr *spr;
    struct Seq *s;
    const u16 *cmd;
    in->b624 = 0;
    in->w628 = 0;
    in->w67c = 0;
    in->h682 = 0;
    in->h680 = 0;
    in->h684 = 0;
    in->h686 = 0;
    in->h688 = 0;
    in->h68a = 0;
    in->b68c = 0;
    in->b68d = 0;
    in->b68e = 0;
    in->b68f = 0;
    in->b690 = 0;
    in->b691 = 0;
    Memory_Clear(in->a694, 8);
    Memory_Clear(in->a69c, 8);
    Memory_Clear(in->a6a4, 8);
    Memory_Clear((u8 *)in + 0x674, 8);
    in->h6ac = 0;
    in->h6ae = 0;
    SceneActorMovement_ResolveRoutePosition(p);
    SceneActorAnimation_SelectDirectionalEntry(p, 0);
    spr = ((struct Obj *)p)->spr;
    s = &spr->seq;
    cmd = (const u16 *)((u8 *)s->commands + s->index * 2);
    spr->frame = *cmd >> 6;
    {
        u32 m = 1;
        u32 fl = m;
        fl &= s->flags;
        if (fl != (((*cmd & 0x30) >> 4) & m))
            spr->flags |= 4;
        else
            spr->flags &= ~4;
    }
    {
        u32 m = 2;
        u8 fl = s->flags & m;
        if (fl != (((*cmd & 0x30) >> 4) & m))
            spr->flags |= 8;
        else
            spr->flags &= ~8;
    }
    s->elapsed++;
    if (s->elapsed >= s->duration) {
        s->elapsed = 0;
        if (s->flags & 4) {
            if (s->index == 0) {
                if (s->flags & 8)
                    s->elapsed = s->duration;
                else
                    s->index = s->frameCount - 1;
            } else
                s->index--;
        } else {
            s->index++;
            if (s->index >= s->frameCount) {
                if (s->flags & 8) {
                    s->elapsed = s->duration;
                    s->index = s->frameCount - 1;
                } else
                    s->index = 0;
            }
        }
        cmd = (const u16 *)((u8 *)s->commands + s->index * 2);
        s->frameWeight = *cmd & 15;
        s->duration = (s->frameWeight * s->speed) >> 6;
        if (!s->duration)
            s->duration = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C4F9C);
#endif
