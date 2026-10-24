#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: group/flag induction, divisor setup and saved-head address reuse.
struct Vec {
    s16 x, y, z, pad;
};
struct Position {
    u32 xy, z;
};
struct Segment {
    u8 pad0[0x1c];
    struct Vec position;
    u8 pad24[0xe1];
    u8 flags;
    u8 pad106[0xe];
    struct Vec target;
    u8 pad11c[12];
};
struct Group {
    u8 pad0[0x3d4];
    struct Segment segments[4];
    u8 pad874[0x66];
    s16 count;
};
struct Drawable {
    u8 pad0[0x1c];
    struct Position position;
};
struct Actor {
    u32 pad0;
    struct Drawable *drawable;
    u8 pad8[0x3c8];
    struct Group *group;
};
s32 Div(s32, s32);
void TentacleActor_TargetMarkedSegment(void *);
void TentacleActor_UpdateSegmentHeadings(void *);
void sub_080A9BD8(void *, void *, void *, s32, s32);
static inline u8 test(struct Segment *p, u32 mask)
{
    if (p->flags & mask)
        return 1;
    return 0;
}
static inline void difference(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline s32 magnitude(struct Vec *v)
{
    s32 x = v->x;
    s32 sum = x * x;
    s32 y = v->y;
    sum += y * y;
    {
        s32 z = v->z;
        sum += z * z;
    }
    return sum;
}
// confidence=low: advances marked segments and bridges gaps along the chain.
#define TentacleActor_UpdateSegmentPositions sub_080A9DB0
void TentacleActor_UpdateSegmentPositions(struct Actor *p, s32 argDivisor, s32 threshold)
{
    register s32 divisor asm("r8") = argDivisor;
    struct Group *g = p->group;
    struct Vec delta;
    struct Position head;
    struct Vec firstDelta;
    s32 i;
    s32 first;
    s32 run;
    asm("" : "=m"(delta));
    head = p->drawable->position;
    asm("" : : "m"(head));
    for (i = 0; i < g->count; i++) {
        struct Segment *seg = &g->segments[i];
        if (test(seg, 2) && (!test(seg, 1))) {
            difference(&delta, &seg->target, (struct Vec *)((((u8 *)g) + (i * 296)) + 0x3f0));
            if (magnitude(&delta) > threshold) {
                {
                    u8 k2 = 1;
                    if (divisor < k2)
                        divisor = 1;
                }
                *((s16 *)((((u8 *)g) + (i * 296)) + 0x3f0)) += Div(delta.x, divisor);
                *((s16 *)((((u8 *)g) + (i * 296)) + 0x3f2)) += Div(delta.y, divisor);
                *((s16 *)((((u8 *)g) + (i * 296)) + 0x3f4)) += Div(delta.z, divisor);
            }
        }
    }

    TentacleActor_TargetMarkedSegment(p);
    head = p->drawable->position;
    first = 255;
    run = 0;
    for (i = 0; i < g->count; i++) {
        struct Segment *seg = &g->segments[i];
        if (!test(seg, 1)) {
            if (test(seg, 2)) {
                if (first != 255)
                    sub_080A9BD8(p, &head, (struct Vec *)((((u8 *)g) + (i * 296)) + 0x3f0), first,
                                 first + run);
                else {
                    difference(&firstDelta, (void *)(&head),
                               (struct Vec *)((((u8 *)g) + (i * 296)) + 0x3f0));
                    if (magnitude(&firstDelta) > 0x1a44) {
                        (*((struct Vec *)((((u8 *)g) + (i * 296)) + 0x3f0))).x +=
                            Div(firstDelta.x, 4);
                        (*((struct Vec *)((((u8 *)g) + (i * 296)) + 0x3f0))).y +=
                            Div(firstDelta.y, 4);
                        (*((struct Vec *)((((u8 *)g) + (i * 296)) + 0x3f0))).z +=
                            Div(firstDelta.z, 4);
                    }
                }
                first = 255;
                {
                    u16 k1 = 296;
                    head = *((struct Position *)((struct Vec *)((((u8 *)g) + (i * k1)) + 0x3f0)));
                }
                run = 0;
            } else {
                if (first == 255)
                    first = i;
                run++;
            }
        }
    }

    TentacleActor_UpdateSegmentHeadings(p);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080A9DB0);
#endif
