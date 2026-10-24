// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Byte-identical with the documented register-pinning concessions.
// confidence=low: coordinates paired animations with an offset motion track.
#define Vanargand_UpdateMovingPairAttack sub_081A93A0
struct Vector {
    s16 x, y, z, pad;
};
struct MovingPairActor {
    u8 pad0[0x54];
    struct Vector position;
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
    struct Vector reference;
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
static inline void SetVector(struct Vector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline void AddVector(struct Vector *v, struct Vector *a, struct Vector *b)
{
    s16 *second = &b->x;
    u16 av, bv;
    // Keep the first-field address separate until its load.
    asm("" : "+r"(second));
    av = a->x;
    bv = *second;
    {
        s16 *out = &v->x;
        *out = av + bv;
    }
    av = b->y;
    bv = a->y;
    v->y = av + bv;
    av = a->z;
    {
        // Local allocation otherwise chooses r1 for this last component.
        register u16 z asm("r2") = b->z;
        v->z = av + z;
    }
}
void Vanargand_UpdateMovingPairAttack(struct MovingPairActor *p)
{
    struct MovingPairActor *self = p;
    u32 mask = 0x40;
    u32 result;
    // Preserve phase as the source of the phase-zero timer resets.
    register s32 phase asm("r4");
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        p->phase = result;
        p->timer = result;
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            if (p->timer == 0) {
                if (!PairedSceneActor_ApplyToActiveHalf(p->pair, 13)) {
                    p->phase = 3;
                    p->timer = phase;
                }
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                u32 mask = 4;
                p->flags |= mask;
                p->phase = 1;
                p->timer = phase;
            }
            break;

        case 1: {
            u32 mask = 4;
            p->flags |= mask;
            {
                // Keep the tested timer value across the pair-animation calls.
                register u32 timer asm("r9") = p->timer;
                if (timer == 0) {
                    struct Vector destination;
                    s32 part = Vanargand_GetComponentWithFlag10(p->pair);
                    if (part == 0)
                        PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 14);
                    else if (part == 1)
                        PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 14);
                    else {
                        p->phase = 3;
                        p->timer = timer;
                    }
                    SetVector(&destination, 576, 512, 576);
                    AddVector(&destination, &p->reference, &destination);
                    {
                        // Derive the offset from the field; preserve reload rotation.
                        register u32 trackOffset asm("r1") =
                            (u32) & ((struct MovingPairActor *)0)->track;
                        asm("" : "+r"(trackOffset));
                        MotionTrack_Initialize((u8 *)p + trackOffset, &self->position, &destination,
                                               0, 4);
                    }
                    p->timer++;
                } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair) && IsComplete(p)) {
                    p->phase = 2;
                    p->timer = result;
                } else {
                    MotionTrack_Update(p->track);
                }
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
                u32 mask = 0x20000;
                u32 flags = p->flags | mask;
                p->flags = flags | 4;
            }
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 19);
            break;
    }
}
