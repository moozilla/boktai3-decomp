// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: coordinates paired animations with an offset motion track.
#define Vanargand_UpdateMovingPairAttack sub_081A93A0
struct MovingPairActor {
    u8 pad0[0x54];
    s16 position[4];
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[2];
    u8 phase, frame;
    u32 timer;
    u8 padA8[7];
    u8 finished, pending, pendingAnimation;
    u8 padB2[0x104 - 0xB2];
    void *callback;
    u8 pad108[0x17C - 0x108];
    s16 reference[4];
    u8 pad184[0x534 - 0x184];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *pair;
};
void Vanargand_SetPendingCue(void *, u16);
u8 Callback_InvokeActor(void *, void *);
s32 PairedSceneActor_ApplyToActiveHalf(void *, s32);
s32 PairedSceneActor_AreSelectedHalvesFinished(void *);
s32 Vanargand_GetComponentWithFlag10(void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
static inline u8 IsComplete(struct MovingPairActor *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}
static inline void SetVector(s16 *v, s32 x, s32 y, s32 z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
static inline void AddVector(s16 *v, s16 *a, s16 *b)
{
    u16 av = a[0], bv = b[0];
    v[0] = av + bv;
    av = b[1];
    bv = a[1];
    v[1] = av + bv;
    av = a[2];
    bv = b[2];
    v[2] = av + bv;
}
inline static void SetVector_081A93A0(s16 *v, s32 x, s32 y, s32 z)
{
    {
        typeof(v[0]) *q1 = &v[0];
        *q1 = x;
    }
    v[1] = y;
    v[2] = z;
}

inline static void AddVector_1(u16 bv, s16 *v, s16 *a, s16 *b)
{
    u16 av = a[0];
    v[0] = av + bv;
    av = b[1];
    bv = a[1];
    v[1] = av + bv;
    av = a[2];
    bv = b[2];
    v[2] = av + bv;
}

void Vanargand_UpdateMovingPairAttack(struct MovingPairActor *p)
{
    struct MovingPairActor *self = p;
    u32 mask = 0x40;
    u32 result;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    if (result)
        return;
    result = Callback_InvokeActor(p, p->callback);
    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        p->phase = result;
        p->timer = result;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                if (!PairedSceneActor_ApplyToActiveHalf(p->pair, 13)) {
                    p->phase = 3;
                    p->timer = 0;
                }
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                u32 mask = 4;
                p->flags |= mask;
                p->phase = 1;
                p->timer = 0;
            }
            break;

        case 1: {
            u32 mask = 4;
            p->flags |= mask;
            if (p->timer == 0) {
                s16 destination[4];
                s32 part = Vanargand_GetComponentWithFlag10(p->pair);
                if (part == 0)
                    PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 14);
                else if (part == 1)
                    PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 14);
                else {
                    p->phase = 3;
                    p->timer = 0;
                }
                SetVector_081A93A0(destination, 576, 512, 576);
                asm("" : : : "memory");
                AddVector_1(destination[0], destination, p->reference, destination);
                MotionTrack_Initialize(p->track, self->position, destination, 0, 4);
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair) && IsComplete(p)) {
                p->phase = 2;
                p->timer = result;
            } else {
                MotionTrack_Update(p->track);
            }
            break;
        }

        case 3:
            p->finished = 1;
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 2);
            break;

        case 2:
            p->finished = 1;
            {
                p->flags |= 0x20000 | 4;
            }
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 19);
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A93A0);
#endif
