#include "global.h"
struct Vec {
    s16 x, y, z;
};
struct Tagged {
    u16 tag, pad2, phase, entered;
    u32 ticks;
    s16 velocity;
    u16 duration;
    u16 x, y, z;
};
struct Npc {
    u8 pad0[7], done, pad8, entered;
    u8 padA[4];
    u16 flags;
    u8 pad10[9], facing4, facing8, heading;
    u8 pad1c[4];
    struct Tagged tagged;
    u8 pad38[0x1c];
    struct Vec target;
    u8 pad5c[9], angle;
    u8 pad66[2];
    struct Vec position;
};
void TaggedSceneNpc_PlayTaggedAnimation(void *, s32, s32, s32, s32);
s32 DisplayAnimation_TestFinished(void *);
u16 Sqrt(u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline void setFlags(struct Npc *p)
{
    u32 mask = 0x400;
    p->flags = mask | p->flags;
}
static inline s32 half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
// Remaining difference: entry-flag pointer lifetime in the airborne phase.
void TaggedSceneNpc_UpdateJumpToTarget(void *manager, struct Npc *p)
{
    struct Tagged *new_var;
    struct Tagged *t = &p->tagged;
    struct Vec *pos = &p->position;
    if (p->entered) {
        u32 root;
        p->entered = 0;
        p->done = 0;
        p->heading = 64;
        p->angle = 64;
        p->facing8 = 5;
        p->facing4 = 2;
        TaggedSceneNpc_PlayTaggedAnimation(p, 7, 2, 0, 4);
        t->velocity = 0;
        t->x = pos->x;
        t->y = pos->y;
        t->z = pos->z;
        root = Sqrt(-200 * (p->target.y - t->y));
        if (Mod(root, 10) > 4)
            t->duration = Div(root, 10) + 1;
        else
            t->duration = Div(root, 10);
    }
    setFlags(p);
    switch (t->phase) {
        case 0: {
            struct Tagged *a = &p->tagged;
            a->entered = 0;
            if (DisplayAnimation_TestFinished((u8 *)p + 0xd4)) {
                a->phase = 1;
                a->entered = 1;
                a->ticks = 0;
            }
            break;
        }
        case 1: {
            struct Tagged *a = &p->tagged;
            u32 entered = a->entered;
            s32 dx, dz, curve;
            a->entered = 0;
            new_var = a;
            if (entered)
                TaggedSceneNpc_PlayTaggedAnimation(p, 8, 2, 0, 4);
            dx = p->target.x - t->x;
            dz = p->target.z - t->z;
            pos->x = t->x + Div(t->ticks * dx, t->duration);
            pos->z = t->z + Div(t->ticks * dz, t->duration);
            curve = half(-(s32)t->ticks * t->ticks);
            pos->y = t->velocity * t->ticks + t->y + curve;
            if (t->ticks >= t->duration) {
                pos->y = p->target.y;
                new_var->phase = 2;
                new_var->entered = 1;
                new_var->ticks = 0;
            }
            break;
        }
        case 2: {
            struct Tagged *a = &p->tagged;
            u32 entered = a->entered;
            a->entered = 0;
            if (entered)
                TaggedSceneNpc_PlayTaggedAnimation(p, 9, 2, 0, 4);
            if (DisplayAnimation_TestFinished((u8 *)p + 0xd4))
                p->done = 1;
            break;
        }
    }
}
