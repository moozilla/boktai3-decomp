#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 85.3%
// Pointer allocation, shake arithmetic and target-loop layout still differ.
#define Hresvelgr_UpdateMovementAndTargets sub_081A26D8
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void sub_081A2430(void *);
void sub_08225B74(void *);
void SceneActors_UpdateAnimation(void *, void *, void *);
void SceneActors_UpdateTargetMetrics(void *);
s32 Math_Atan2(s32, s32);
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 shift(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
struct MovementView {
    u8 pad0[0x48];
    u8 body[12];
    s16 pos[3];
    u8 pad5a[0x42];
    u32 flags;
    u8 padA0[0x28];
    union {
        s32 distance[18];
        struct {
            u8 pad[4];
            u8 angles[0x44];
        } heading;
    } metrics;
    s16 *targets[2];
    u8 pad118[0x9c];
    u16 display[4];
    u8 pad1bc[0x154];
    u16 shake;
    u8 pad312[0x6a];
    u16 targetCount;
};
static inline u32 flags(u8 *p, u32 mask)
{
    return W(0x9c) & mask;
}
void Hresvelgr_UpdateMovementAndTargets(u8 *input)
{
    u8 *p = input;
    register struct MovementView *view asm("r9") = (void *)p;
    struct MovementView *actor = (void *)p;
    u8 *body = p + 0x48;
    s32 i;
    asm("" : "+r"(view));
    sub_081A2430(p);
    sub_08225B74(body);
    if (!flags(p, 0x800000))
        SceneActors_UpdateAnimation(p, p + 0x194, p + 0x174);
    actor->display[0] = *(u16 *)(body + 0xc);
    actor->display[1] = *(u16 *)(body + 0xe);
    actor->display[2] = *(u16 *)(body + 0x10);
    if (actor->shake) {
        u32 angle;
        s32 d;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
        angle = gUnk_0203B400[gUnk_03005308];
        d = shift(gUnk_086149C4[(angle + 64) & 255] * (actor->shake << 2));
        actor->display[0] += d;
        d = shift(gUnk_086149C4[angle & 255] * (actor->shake << 2));
        actor->display[2] += d;
        actor->shake--;
    }
    SceneActors_UpdateTargetMetrics(view);
    for (i = 0; i < actor->targetCount; i++) {
        u8 *loopBody = view->body;
        s16 *target = view->targets[i];
        if (target) {
            s32 x = target[0] - *(s16 *)(loopBody + 12);
            s32 z = target[2] - *(s16 *)(loopBody + 16);
            view->metrics.heading.angles[i] = Math_Atan2(x, z);
            view->metrics.distance[i] = x * x + z * z;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A26D8);
#endif
