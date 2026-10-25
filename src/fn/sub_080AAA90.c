#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 85.1%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: mask setup order and flag-cursor strength reduction.
#include "scene_actor_reset.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Segment {
    u8 pad0[0x1c];
    struct Vec position;
    u8 pad24[0xe1];
    u8 flags;
    u8 pad106[0x16];
    struct Vec velocity;
    u8 pad124[4];
};
struct Group {
    u8 pad0[0x3d4];
    struct Segment segments[4];
    u8 pad874[0x66];
    s16 count;
};
static inline u32 wordFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D0 & mask;
}
static inline u8 inactive(struct SceneActorRegistryResetView *p)
{
    if (p) {
        u32 mask = 0x40;
        if ((p->flags2D4 & mask) || wordFlag(p, 0x20000) || p->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 test(u8 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 half(s32 v)
{
    return v >= 0 ? v >> 1 : -((-v) >> 1);
}
// confidence=low: adds per-segment impulses and halves their signed components.
#define TentacleActor_UpdateSegmentImpulses sub_080AAA90
inline static u8 test_080AAA90(u8 *p, u8 mask)
{
    if ((*p) & mask)
        return 1;
    return 0;
}

void TentacleActor_UpdateSegmentImpulses(struct SceneActorRegistryResetView *p)
{
    struct Group *g;
    s32 i;
    register u32 two asm("r9");
    register u32 one asm("r8");
    g = p->context;
    if (inactive(p))
        return;
    two = 2;
    asm("" : : "r"(two));
    one = 1;
    asm("" : : "r"(one));
    for (i = 0; i < g->count; i++) {
        struct Segment *s = &g->segments[i];
        if (test_080AAA90((((u8 *)g) + (i * 296)) + 0x4d9, two) &&
            (!test_080AAA90((((u8 *)g) + (i * 296)) + 0x4d9, one))) {
            *((s16 *)((((u8 *)g) + (i * 296)) + 0x3f0)) +=
                *((s16 *)((((u8 *)g) + (i * 296)) + 0x4f0));
            *((s16 *)((((u8 *)g) + (i * 296)) + 0x3f2)) +=
                *((s16 *)((((u8 *)g) + (i * 296)) + 0x4f2));
            *((s16 *)((((u8 *)g) + (i * 296)) + 0x3f4)) +=
                *((s16 *)((((u8 *)g) + (i * 296)) + 0x4f4));
            *((s16 *)((((u8 *)g) + (i * 296)) + 0x4f0)) =
                half(*((s16 *)((((u8 *)g) + (i * 296)) + 0x4f0)));
            *((s16 *)((((u8 *)g) + (i * 296)) + 0x4f2)) =
                half(*((s16 *)((((u8 *)g) + (i * 296)) + 0x4f2)));
            *((s16 *)((((u8 *)g) + (i * 296)) + 0x4f4)) =
                half(*((s16 *)((((u8 *)g) + (i * 296)) + 0x4f4)));
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080AAA90);
#endif
